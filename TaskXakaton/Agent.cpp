#include "Agent.hpp"
#include "Macros.hpp"

Agent::Agent(std::string ip, int port, std::string AgentName) {
    this->ssh = ssh_new();

    ssh_options_set(this->ssh, SSH_OPTIONS_HOST, ip.c_str());
    ssh_options_set(this->ssh, SSH_OPTIONS_USER, AgentName.c_str());
    ssh_options_set(this->ssh, SSH_OPTIONS_PORT, &port);
}

int Agent::ConnectUbuntuServer_ssh(std::string AgentPassword) {
    this->Connectssh = ssh_connect(this->ssh);
    if (this->Connectssh != SSH_OK) {
        this->ErrorMsg = "Error: connecting to server. " + (std::string)ssh_get_error(this->ssh) + ".\n";
        ssh_free(this->ssh);
        return ERROR;
    }

    this->Connectssh = ssh_userauth_password(this->ssh, nullptr, AgentPassword.c_str());
    if (this->Connectssh != SSH_AUTH_SUCCESS) {
        this->ErrorMsg = "Error: authentication. " + (std::string)ssh_get_error(this->ssh) + ".\n";
        ssh_disconnect(this->ssh); ssh_free(this->ssh);
        return ERROR;
    }
    return OK;
}

int Agent::ExecCommand(std::string Command) {
    this->Channelssh = ssh_channel_new(this->ssh);
    if (this->Channelssh == nullptr) {
        this->ErrorMsg = "Error: create SSH channel.\n";
        ssh_disconnect(this->ssh);
        ssh_free(this->ssh);
        return ERROR;
    }

    this->Connectssh = ssh_channel_open_session(this->Channelssh);
    if (this->Connectssh != SSH_OK) {
        this->ErrorMsg = "Error: create SSH channel. " + (std::string)ssh_get_error(this->ssh) + ".\n";
        ssh_channel_free(this->Channelssh);
        ssh_disconnect(this->ssh);
        ssh_free(this->ssh);
        return ERROR;
    }

    this->Connectssh = ssh_channel_request_exec(this->Channelssh, Command.c_str());
    if (this->Connectssh != SSH_OK) {
        this->ErrorMsg = "Error: execute command. " + (std::string)ssh_get_error(this->ssh) + ".\n";
        ssh_channel_close(this->Channelssh);
        ssh_channel_free(this->Channelssh);
        ssh_disconnect(this->ssh);
        ssh_free(this->ssh);
        return ERROR;
    }

    return OK;
}

int Agent::ReplyLinux(std::string idClient) {
    char buffer[1024];
    int nbytes;
    std::string bufferStr;
    std::vector<std::string> lines;

    while ((nbytes = ssh_channel_read(this->Channelssh, buffer, sizeof(buffer), 0)) > 0) {
        bufferStr.append(buffer, nbytes);
    }

    std::istringstream stream(bufferStr);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    for (const auto& l : lines) {
        if (idClient == l) {
            return ERROR;
        }
    }

    return OK;
}

int Agent::LoadFile(std::string idClient,std::string nameFile) {
    std::string scpDir = "/home/agent/storagefileusers/" + idClient;
    this->scp = ssh_scp_new(this->ssh, SSH_SCP_WRITE, scpDir.c_str());
    if (this->scp == nullptr) {
        this->ErrorMsg = "Error: allocating SCP session. " + (std::string)ssh_get_error(this->ssh) + ".\n";
        ssh_disconnect(this->ssh);
        ssh_free(this->ssh);
        return ERROR;
    }

    this->Connectssh = ssh_scp_init(this->scp);
    if (this->Connectssh != SSH_OK) {
        this->ErrorMsg = "Error: initializing SCP session. " + (std::string)ssh_get_error(this->ssh) + ".\n";
        ssh_scp_free(this->scp);
        ssh_disconnect(this->ssh);
        ssh_free(this->ssh);
        return ERROR;
    }

    std::ifstream file(nameFile.c_str(), std::ios::binary | std::ios::ate);
    if (!file) {
        this->ErrorMsg = "Error: open local file.\n";
        ssh_scp_close(this->scp);
        ssh_scp_free(this->scp);
        ssh_disconnect(this->ssh);
        ssh_free(this->ssh);
        return ERROR;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    this->Connectssh = ssh_scp_push_file(this->scp, nameFile.c_str(), fileSize, 0);
    if (this->Connectssh != SSH_OK) {
        this->ErrorMsg = "Error: pushing file to server. " + (std::string)ssh_get_error(this->ssh) + ".\n";
        file.close();
        ssh_scp_close(this->scp);
        ssh_scp_free(this->scp);
        ssh_disconnect(this->ssh);
        ssh_free(this->ssh);
        return ERROR;
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        ssh_scp_write(this->scp, buffer, sizeof(buffer));
    }
    ssh_scp_write(this->scp, buffer, file.gcount());
    return OK;
}

Agent::~Agent() {
    ssh_channel_close(this->Channelssh);
    ssh_channel_free(this->Channelssh);
    ssh_disconnect(this->ssh);
    ssh_free(this->ssh);
}
