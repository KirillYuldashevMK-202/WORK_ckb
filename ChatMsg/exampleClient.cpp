#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <string>
#include <thread>

#define SIZEMSGBUF 1024

#pragma comment(lib, "Ws2_32.lib")

void RecvMessageServer(SOCKET recvServer) {
    char MsgBuffer[SIZEMSGBUF] = { 0 };
    while (true) {
        int recvSize = recv(recvServer, MsgBuffer, sizeof(MsgBuffer), 0);
        if (recvSize > 0) {
            MsgBuffer[recvSize] = '\0';
            std::string MsgServer = (std::string)MsgBuffer;
            std::cout << MsgServer << std::endl;
        }
    }
}

int main() {
    WSADATA Data;
    int Status = WSAStartup(MAKEWORD(2, 2), &Data);

    if (Status != 0) {
        std::cout << "WSAStartup Error : " << Status << std::endl;
        exit(1);
    }

    in_addr ip;
    Status = inet_pton(AF_INET, "127.0.0.1", &ip);

    if (Status <= 0) {
        std::cout << "inet_pton Error : " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in ServerInfo;
    ServerInfo.sin_family = AF_INET;
    ServerInfo.sin_addr = ip;


    SOCKET ServerSock = socket(AF_INET, SOCK_STREAM, 0);

    if (ServerSock == INVALID_SOCKET) {
        std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    ServerInfo.sin_port = htons(27015);
    Status = connect(ServerSock, (sockaddr*)&ServerInfo, sizeof(ServerInfo));
    std::thread(RecvMessageServer, ServerSock).detach();
    std::cout << ".......Welcome to the chat......." << std::endl;
    std::cout << "Rules: for exit (dis)" << std::endl;

    
    while (true) {
        std::string Message;
        std::getline(std::cin, Message);
        if (Message == "dis") {
            break;
        }
        std::string MainMessage = "Kirillka >>> " + Message;
        send(ServerSock, MainMessage.c_str(), MainMessage.size(), 0);
    }

    closesocket(ServerSock);
    WSACleanup();
    return 0;
}
