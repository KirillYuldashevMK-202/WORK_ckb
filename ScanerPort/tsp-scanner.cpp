#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

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

    const int startPort = 4368;
    const int endPort = 4370;

    for (int port = startPort; port <= endPort; ++port) {
        SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

        if (ServSock == INVALID_SOCKET) {
            std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }

        ServerInfo.sin_port = htons(port);
        Status = connect(ServSock, (sockaddr*)&ServerInfo, sizeof(ServerInfo));
        
        if (Status == 0) {
            std::cout << "Port " << port << " is open" << std::endl;
        }
        else {
            std::cout << "Port " << port << " Error: " << WSAGetLastError() << std::endl;
        
        }
        closesocket(ServSock);
    }
    WSACleanup();
    return 0;
}
