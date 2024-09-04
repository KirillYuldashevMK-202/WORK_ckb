#include "Backend.hpp"
#include "Macros.hpp"

SOCKET ConnectBackend() {
    WSADATA Data;
    int Status = WSAStartup(MAKEWORD(2, 2), &Data);

    if (Status != 0) {
        return ERROR;
    }

    in_addr ip;
    Status = inet_pton(AF_INET, "127.0.0.1", &ip);

    if (Status <= 0) {
        WSACleanup();
        return ERROR;
    }

    sockaddr_in ServerInfo;
    ServerInfo.sin_family = AF_INET;
    ServerInfo.sin_addr = ip;

    SOCKET ServerSock = socket(AF_INET, SOCK_STREAM, 0);

    if (ServerSock == INVALID_SOCKET) {
        WSACleanup();
        return ERROR;
    }

    ServerInfo.sin_port = htons(27015);
    Status = connect(ServerSock, (sockaddr*)&ServerInfo, sizeof(ServerInfo));

    if (ServerSock == INVALID_SOCKET) {
        WSACleanup();
        return ERROR;
    }
    return ServerSock;
}

std::string recvBackend(SOCKET ServerSock) {
    char MsgBuffer[SIZEMSGBUF] = { 0 };
    std::string Infobackend;
    int recvSize = recv(ServerSock, MsgBuffer, sizeof(MsgBuffer), 0);
    if (recvSize > 0) {
        MsgBuffer[recvSize] = '\0';
        Infobackend = (std::string)MsgBuffer;
    }
    return Infobackend;
}