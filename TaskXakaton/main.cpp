#include "Agent.hpp"
#include "MinIO.hpp"
#include "Postgre.hpp"
#include "Macros.hpp"
#include "Backend.hpp"
#include <thread>
#include <sstream>

void DownloadFile(Agent& MainAgent, WorkDB& MainDB, const std::string& IDuser, const std::string& nameFile) {
    int ErrDB = MainDB.AddFilesUsers(IDuser.c_str(), nameFile.c_str());
    if (ErrDB == OK) {
        std::cout << "File Download" << std::endl;
    }
    else if (ErrDB == USERSEXISTS) {
        std::cout << "Such a file already exists" << std::endl;
        return;
    }
    else if (ErrDB == ERROR) {
        std::cout << MainDB.ErrorMsg << std::endl;
        return;
    }

    FileMinio MainMinio("fileclient", nameFile.c_str(), nameFile.c_str(), "qwerty123", "qwerty1234");
    MainMinio.Download();

    MainAgent.LoadFile(IDuser.c_str(), nameFile.c_str());
}

int main() {
    // Подключение к серверу
    Agent MainAgent("192.168.44.137", 22, "agent");
    if (MainAgent.ConnectUbuntuServer_ssh("qwerty123") == ERROR) {
        std::cout << MainAgent.ErrorMsg << std::endl;
        exit(1);
    }

    // Подключение к базе данных
    WorkDB MainDB("127.0.0.1", "5050", "postgres", "postgres", "qwerty123");

    // Подключение к бэкенду (по сокету)
    SOCKET Backend = ConnectBackend();

    while (true) {
        std::string CommandBackend = recvBackend(Backend);
        
        if (CommandBackend == "") {
            continue;
        }

        std::stringstream ss(CommandBackend);
        std::string line;
        std::vector<std::string> linesBackend;
        while (std::getline(ss, line, '\n')) {
            linesBackend.push_back(line);
            std::cout << line<<std::endl;
        }

        if (linesBackend[0] == "CreateFolder") {
            std::string IDuser = linesBackend[1];
            if (MainAgent.ExecCommand("ls storagefileusers") == ERROR) {
                std::cout << MainAgent.ErrorMsg << std::endl;
                continue;
            }
            if (MainAgent.ReplyLinux(IDuser) != ERROR) {
                std::string MainLinux = "mkdir storagefileusers/" + IDuser;
                if (MainAgent.ExecCommand(MainLinux.c_str()) == ERROR) {
                    std::cout << MainAgent.ErrorMsg << std::endl;
                    continue;
                }
            }
        }

        if (linesBackend[0] == "DownloadFile") {
            std::string IDuser = linesBackend[1];
            std::string nameFile = linesBackend[2];
    
            std::thread downloadThread(DownloadFile, std::ref(MainAgent), std::ref(MainDB), IDuser, nameFile);
            downloadThread.detach();
        }

        if (linesBackend[0] == "DeleteFile") {
            std::string IDuser = linesBackend[1];
            std::string nameFile = linesBackend[2];

            std::string ReqDel = "rm storagefileusers/" + IDuser + "/" + nameFile;
            MainAgent.ExecCommand(ReqDel.c_str());

            if (MainDB.DeleteFilesUsers(IDuser.c_str(), nameFile.c_str()) == ERROR) {
                std::cout << MainDB.ErrorMsg << std::endl;
            }
        }

        if (linesBackend[0] == "DeleteUser") {
            std::string IDuser = linesBackend[1];

            std::string ReqDel = "rm -r storagefileusers/" + IDuser;
            MainAgent.ExecCommand(ReqDel.c_str());
            if (MainDB.DeleteUser(IDuser.c_str()) == ERROR) {
                std::cout << MainDB.ErrorMsg << std::endl;
            }
        }
    }
    
    return 0;
}
