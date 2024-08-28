#include "Agent.hpp"
#include "MinIO.hpp"
#include "Postgre.hpp"
#include "Macros.hpp"
#include <thread>

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

    while (true) {
        std::cout << "Command: ";
        std::string ReceivingRequest;
        std::getline(std::cin, ReceivingRequest);
        if (ReceivingRequest == "Connect user") {
            std::cout << "ID users connect: ";
            std::string IDuser;
            std::cin >> IDuser;
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

        if (ReceivingRequest == "Download file") {
            std::cout << "ID users: ";
            std::string IDuser;
            std::cin >> IDuser;

            std::cout << "Name file " + IDuser + ": ";
            std::string nameFile;
            std::cin >> nameFile;

            std::thread downloadThread(DownloadFile, std::ref(MainAgent), std::ref(MainDB), IDuser, nameFile);
            downloadThread.detach();
        }

        if (ReceivingRequest == "Delete file") {
            std::cout << "ID users: ";
            std::string IDuser;
            std::cin >> IDuser;

            std::cout << "Name file " + IDuser + ": ";
            std::string nameFile;
            std::cin >> nameFile;

            std::string ReqDel = "rm storagefileusers/" + IDuser + "/" + nameFile;
            MainAgent.ExecCommand(ReqDel.c_str());
            
            if(MainDB.DeleteFilesUsers(IDuser.c_str(), nameFile.c_str()) == ERROR) {
                std::cout << MainDB.ErrorMsg << std::endl;
            }
        }

        if (ReceivingRequest == "Delete user") {
            std::cout << "ID users: ";
            std::string IDuser;
            std::cin >> IDuser;

            std::string ReqDel = "rm -r storagefileusers/" + IDuser;
            MainAgent.ExecCommand(ReqDel.c_str());
            if (MainDB.DeleteUser(IDuser.c_str()) == ERROR) {
                std::cout << MainDB.ErrorMsg << std::endl;
            }
        }
    }
    return 0;
}
