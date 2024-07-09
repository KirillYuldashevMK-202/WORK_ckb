#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

void UploadFile(SOCKET sock, const std::string file,int sizeFile) {
    std::ifstream FileUpload;

    FileUpload.open(file);
    if (!(FileUpload.is_open())) {
        std::cout<<"Error: open file upload"<<std::endl;
        return;
    }

    std::string command = "Upload " + file + " ";
    send(sock, command.c_str(), command.size(), 0);

    char* buffer = new char[sizeFile];
    FileUpload.get(buffer, sizeFile);
    std::string Datafile = std::string(buffer);
    send(sock, Datafile.c_str(), Datafile.size(), 0);

    delete[] buffer;
}

int main() {
    WSADATA Data;
    int Status = WSAStartup(MAKEWORD(2, 2), &Data);

    if (Status != 0) {
        std::cout << "WSAStartup Error: " << Status << std::endl;
        return 1;
    }

    in_addr ip;
    Status = inet_pton(AF_INET, "127.0.0.1", &ip);

    if (Status <= 0) {
        std::cout << "Error ip" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in ServerInfo;
    ServerInfo.sin_family = AF_INET;
    ServerInfo.sin_addr = ip;


    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

    if (ServSock == INVALID_SOCKET) {
        std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    ServerInfo.sin_port = htons(27015);
    Status = connect(ServSock, (sockaddr*)&ServerInfo, sizeof(ServerInfo));
    UploadFile(ServSock, "newnewTest.txt", 1024);

    closesocket(ServSock);
    WSACleanup();
    return 0;
}
