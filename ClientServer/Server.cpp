#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <fstream>
#include <sys/types.h>
#include <string>
#include <fstream>

#pragma comment(lib,"Ws2_32.lib")

int main() {
    WSADATA Data;

    int Status = WSAStartup(MAKEWORD(2, 2), &Data);
    if (Status != 0) {
        std::cout << "WSAStartup Error: " << Status << std::endl;
        return 1;
    }

    SOCKET Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Server == INVALID_SOCKET) {
        std::cout << "Error initializing socket: " << WSAGetLastError() << std::endl;
        closesocket(Server);
        WSACleanup();
        return 1;
    }

    sockaddr_in Address;
    Address.sin_family = AF_INET;
    Address.sin_port = htons(27015);

    Status = inet_pton(AF_INET, "127.0.0.1", &Address.sin_addr);
    if (Status != 1) {
        std::cout << "inet_pton Error: " << WSAGetLastError() << std::endl;
        closesocket(Server);
        WSACleanup();
        return 1;
    }

    Status = bind(Server, (sockaddr*)&Address, sizeof(Address));
    if (Status == SOCKET_ERROR) {
        std::cout << "Bind Error: " << WSAGetLastError() << std::endl;
        closesocket(Server);
        WSACleanup();
        return 1;
    }

    Status = listen(Server, SOMAXCONN);
    if (Status == SOCKET_ERROR) {
        std::cout << "Listen Error: " << WSAGetLastError() << std::endl;
        closesocket(Server);
        WSACleanup();
        return 1;
    }

    std::cout << "Server created....." << std::endl;

    while (true) {
        sockaddr_in Client;
        int SizeClient = sizeof(Client);

        SOCKET Client_status = accept(Server, (sockaddr*)&Client,&SizeClient);
        if (Client_status == INVALID_SOCKET) {
            std::cout << "Error Cleint connection"<< std::endl;
            closesocket(Server);
            WSACleanup();
            return 1;
        }

        else {
            std::cout << "Client Hello!!!" << std::endl;
            char buffer[1024] = { 0 };
            memset(buffer, 0, 1024);
            recv(Client_status, buffer, 1024, 0);
            std::cout << buffer << std::endl;
            
            std::string command;
            std::string Filename;
            int startfilename = 0;
            for (int i = 0; i < 1024; i += 1) {
                if (buffer[i] == ' ') {
                    startfilename = i + 1;
                    break;
                }
                command.push_back(buffer[i]);
            }
            for (int i = startfilename; i < 1024; i += 1) {
                if (buffer[i] == ' ' || buffer[i] == '\0') {
                    break;
                }
                Filename.push_back(buffer[i]);
            }

            if (command == "Upload") {
                std::ofstream newFile(Filename, std::ios::binary);
                if (!newFile.is_open()) {
                    std::cout << "Error: open file upload" << std::endl;
                    closesocket(Client_status);
                    continue;
                }

                while (true) {
                    int read;
                    memset(buffer, 0, 1024);
                    read = recv(Client_status, buffer, 1024, 0);
                    if (read == SOCKET_ERROR) {
                        std::cout << "Error: read file"<<std::endl;
                        break;
                    }
                    if (read == 0) {
                        std::cout << "File Unload "<<Filename<<std::endl;
                        break;
                    }
                    newFile.write(buffer, read);
                    break;
                }
                newFile.close();
            }
            else if (command == "Unload") {
                
            
            }
            else if(command == "Exit"){
            
            
            
            }

        
        }
    }
}
