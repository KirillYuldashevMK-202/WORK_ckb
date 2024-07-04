#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA Data;
    int Status = WSAStartup(MAKEWORD(2, 2), &Data);

    if (Status != 0) {
        std::cout << "WSAStartup Error: " << Status << std::endl;
        return 1;
    }

    in_addr ip;
    Status = inet_pton(AF_INET, "192.168.41.47", &ip);

    if (Status <= 0) {
        std::cout << "Error ip" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in ServerInfo;
    ServerInfo.sin_family = AF_INET;
    ServerInfo.sin_addr = ip;

    const int startPort = 1;
    const int endPort = 10000;

    for (int port = startPort; port <= endPort; ++port) {
        SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

        if (ServSock == INVALID_SOCKET) {
            std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }

        ServerInfo.sin_port = htons(port);
        const char* MsgTest = "Test port";
        Status = sendto(ServSock, MsgTest, strlen(MsgTest), 0, (sockaddr*)&ServerInfo, sizeof(ServerInfo));

        if (Status == SOCKET_ERROR) {
            std::cout << "Port " << port << " Error: " << WSAGetLastError() << std::endl;
            continue;
        }
        else {
            std::cout << "Port " << port << " sent MsgTest" << std::endl;
        }

        closesocket(ServSock);
    }
    WSACleanup();
    return 0;
}
