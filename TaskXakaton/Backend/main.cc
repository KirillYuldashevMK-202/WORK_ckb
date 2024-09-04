#include <drogon/drogon.h>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <thread>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

#define SIZEMSGBUF 1024

class MyServerTCP {
public:
    int Status;
    SOCKET Server;
    SOCKET ClientSocket = INVALID_SOCKET;
    WSADATA Data;
    sockaddr_in Address;

    MyServerTCP(int port) {
        this->Address.sin_family = AF_INET;
        this->Address.sin_port = htons(port);
    }

    MyServerTCP() {
        this->Address.sin_family = AF_INET;
        this->Address.sin_port = htons(27015);
    }

    void Startup() {
        this->Status = WSAStartup(MAKEWORD(2, 2), &this->Data);
        if (Status != 0) {
            std::cerr << "WSAStartup Error : " << Status << std::endl;
            exit(1);
        }
    }

    void CreateSocket() {
        this->Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (this->Server == INVALID_SOCKET) {
            std::cerr << "Socket Error : " << WSAGetLastError() << std::endl;
            closesocket(this->Server);
            WSACleanup();
            exit(1);
        }
    }

    void BindingAdress() {
        this->Status = inet_pton(AF_INET, "127.0.0.1", &Address.sin_addr);
        if (this->Status != 1) {
            std::cerr << "inet_pton Error : " << WSAGetLastError() << std::endl;
            closesocket(this->Server);
            WSACleanup();
            exit(1);
        }
        this->Status = bind(this->Server, (sockaddr*)&(this->Address), sizeof(this->Address));
        if (this->Status == SOCKET_ERROR) {
            std::cerr << "Bind Error : " << WSAGetLastError() << std::endl;
            closesocket(this->Server);
            WSACleanup();
            exit(1);
        }
        this->Status = listen(this->Server, SOMAXCONN);
        if (this->Status == SOCKET_ERROR) {
            std::cerr << "Listen Error : " << WSAGetLastError() << std::endl;
            closesocket(this->Server);
            WSACleanup();
            exit(1);
        }
    }

    void AcceptClient() {
        sockaddr_in Client;
        int SizeClient = sizeof(Client);
        this->ClientSocket = accept(this->Server, (sockaddr*)&Client, &SizeClient);
        if (this->ClientSocket == INVALID_SOCKET) {
            std::cerr << "Accept Error : " << WSAGetLastError() << std::endl;
            closesocket(this->Server);
            WSACleanup();
            exit(1);
        }
    }

    void SendUserIDData(const std::string& command, const std::string& userid) {
        if (this->ClientSocket != INVALID_SOCKET) {
            std::string MainSend = command + '\n' + userid + '\n';
            send(this->ClientSocket, MainSend.c_str(), MainSend.size(), 0);
        }
    }

    void SendUserIDNameFileData(const std::string& command, const std::string& userid, const std::string& nameFile) {
        if (this->ClientSocket != INVALID_SOCKET) {
            std::string MainSend = command + '\n' + userid + '\n' + nameFile + '\n';
            send(this->ClientSocket, MainSend.c_str(), MainSend.size(), 0);
        }
    }

    void CreateServer() {
        Startup();
        CreateSocket();
        BindingAdress();
        AcceptClient(); 
    }

    ~MyServerTCP() {
        if (this->ClientSocket != INVALID_SOCKET) {
            closesocket(this->ClientSocket);
        }
        closesocket(this->Server);
        WSACleanup();
    }
};

void runBackend(MyServerTCP& server) {
    drogon::app().registerHandler("/createFolder", [&server](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        std::string userId = req->getParameter("userId");
        server.SendUserIDData("CreateFolder", userId);

        auto resp = drogon::HttpResponse::newHttpResponse();
        std::string Msg = "CreateFolder " + userId;
        resp->setBody(Msg.c_str());
        callback(resp);
        });

    drogon::app().registerHandler("/deleteUser", [&server](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        std::string userId = req->getParameter("userId");
        server.SendUserIDData("DeleteUser", userId);

        auto resp = drogon::HttpResponse::newHttpResponse();
        std::string Msg = "DeleteUser " + userId;
        resp->setBody(Msg.c_str());
        callback(resp);
        });

    drogon::app().registerHandler("/downloadFile", [&server](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        std::string userId = req->getParameter("userId");
        std::string nameFile = req->getParameter("nameFile");
        server.SendUserIDNameFileData("DownloadFile", userId, nameFile);

        auto resp = drogon::HttpResponse::newHttpResponse();
        std::string Msg = "DownloadFile " + userId + " : " + nameFile;
        resp->setBody(Msg.c_str());
        callback(resp);
        });

    drogon::app().registerHandler("/deleteFile", [&server](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        std::string userId = req->getParameter("userId");
        std::string nameFile = req->getParameter("nameFile");
        server.SendUserIDNameFileData("DeleteFile", userId, nameFile);

        auto resp = drogon::HttpResponse::newHttpResponse();
        std::string Msg = "DeleteFile " + userId + " : " + nameFile;
        resp->setBody(Msg.c_str());
        callback(resp);
        });

    drogon::app().addListener("0.0.0.0", 1024).run();
}

int main() {
    MyServerTCP ServerMain;
    ServerMain.CreateServer();

    std::thread backendThread(runBackend, std::ref(ServerMain));

    backendThread.join();

    return 0;
}
