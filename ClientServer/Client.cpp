#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <string>

#define SIZEBUF 1024

#pragma comment(lib, "Ws2_32.lib")

void UploadFile(SOCKET sock, const std::string file) {
    //Отправка команды и имени файла
    std::string command = "Upload " + file + ' ';
    send(sock, command.c_str(), command.size(), 0);
    char DataFileUpload[SIZEBUF] = { 0 };
    std::ifstream FileUpload(file, std::ios::binary);
    //Загрузка информации
    while (FileUpload.read(DataFileUpload, SIZEBUF) || FileUpload.gcount() > 0) {
        int bytesRead = FileUpload.gcount();
        int bytesSent = 0;
        while (bytesSent < bytesRead) {
            int sent = send(sock, DataFileUpload + bytesSent, bytesRead - bytesSent, 0);
            if (sent == -1) {
                FileUpload.close();
                break;
            }
            bytesSent += sent;
        }
    }
    shutdown(sock, 1);
}

//Загрузка файла на сервер
void UnloadFile(SOCKET sock) {
    //Команда для сервера для загрузки
    std::string command = "Unload ";
    send(sock, command.c_str(), command.size(), 0);

    //Буфер для сообщений между сервером и клиентом
    char Msgserver[SIZEBUF] = { 0 };

    //Считывания имен файлов от сервера
    memset(Msgserver, 0, SIZEBUF);
    recv(sock, Msgserver, SIZEBUF, 0);
    std::cout << Msgserver << std::endl;

    //Считывание и отправка имени файла для выгрузки
    std::string FileUnload;
    std::cout << "Enter the file name to unload: ";
    std::getline(std::cin, FileUnload);
    send(sock, FileUnload.c_str(), FileUnload.size(), 0);

    //Сообщение об открытии файла
    memset(Msgserver, 0, SIZEBUF);
    recv(sock, Msgserver, SIZEBUF, 0);
    std::cout << Msgserver << std::endl;

    //Создание и запись информации с файла сервера
    char DataFileUnload[SIZEBUF];
    int bytesRead;
    FileUnload.erase(0, 2);
    std::ofstream newFile(FileUnload);
    if (!newFile.is_open()) {
        std::cerr << "Failed to create file" << std::endl;
        return;
    }
    while ((bytesRead = recv(sock, DataFileUnload, SIZEBUF, 0)) > 0) {
        newFile.write(DataFileUnload, bytesRead);
    }
    newFile.close();
}

void Exit(SOCKET sock) {
    std::string command = "Exit ";
    send(sock, command.c_str(), command.size(), 0);
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
    UploadFile(ServSock, "Test.txt");
    //UnloadFile(ServSock);
    //Exit(ServSock);
    closesocket(ServSock);
    WSACleanup();
    return 0;
}
