#ifndef MODULE_AGENT
#define MODULE_AGENT

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

class Agent {
public:
    std::string ErrorMsg;

    Agent(std::string ip, int port, std::string AgentName);

    int ConnectUbuntuServer_ssh(std::string AgentPassword);

    int ExecCommand(std::string Command);

    int DeleteFiles(std::string idClient, std::string nameFile);

    int ReplyLinux(std::string idClient);

    int LoadFile(std::string idClient, std::string nameFile);

    ~Agent();

private:
    ssh_session ssh;
    ssh_scp scp;
    int Connectssh;
    ssh_channel Channelssh;
};

#endif