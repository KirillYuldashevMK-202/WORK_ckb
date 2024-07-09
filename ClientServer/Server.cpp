#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <fstream>
#include <sys/types.h>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

#define SIZEBUF 1024

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
            std::cout << "Client connected" << std::endl;
            //Буфер для сообщений между сервером и клиентом
            char buffer[SIZEBUF] = { 0 };
            memset(buffer, 0, SIZEBUF);
            recv(Client_status, buffer, SIZEBUF, 0);//Считывание команды от клиента
            
            std::string command;
            std::string Filename;
            int startfilename = 0;
            for (int i = 0; i < SIZEBUF; i += 1) {
                if (buffer[i] == ' ') {
                    startfilename = i + 1;
                    break;
                }
                command.push_back(buffer[i]);
            }
            for (int i = startfilename; i < SIZEBUF; i += 1) {
                if (buffer[i] == ' ' || buffer[i] == '\0') {
                    break;
                }
                Filename.push_back(buffer[i]);
            }

            //Модуль загрузки файла
            if (command == "Upload"){
                //Считывание информации и загрузка в файл
                char DataFileUnload[SIZEBUF];
                int bytesRead;
                std::ofstream newFile(Filename);
                if (!newFile.is_open()) {
                    std::cerr << "Failed to create file" << std::endl;
                    return 1;
                }
                while ((bytesRead = recv(Client_status, DataFileUnload, SIZEBUF, 0)) > 0) {
                    newFile.write(DataFileUnload, bytesRead);
                }
                newFile.close();
            }

            //Модуль выгрузки файла
            else if (command == "Unload") {
                //Загрузка имен всех файлов в вектор
                std::string path = ".";
                std::vector <std::string> filename_vector;
                for (auto& p : std::filesystem::directory_iterator(path)) {
                    if (p.path().extension() == ".txt") {
                        filename_vector.push_back(p.path().string());
                    }
                }
                //Преобразование вектора в строку и отправка клиенту
                std::string filename_str = "File in server:\n";
                for (const auto& name : filename_vector) {
                    filename_str += name + '\n';
                }
                send(Client_status, filename_str.c_str(), filename_str.size(), 0);
                //Поиск нужного файла для выгрузки и отправка клиенту результата поиска
                std::string nameFileUnload;
                memset(buffer, 0, SIZEBUF);
                recv(Client_status, buffer, SIZEBUF, 0);
                nameFileUnload = (std::string) buffer;
                int seacrhfile = -1;
                for (int k = 0; k < filename_vector.size(); k += 1) {
                    if(filename_vector[k] == nameFileUnload){
                        seacrhfile = k;
                    }
                }
                if (seacrhfile == -1) {
                    send(Client_status, "File not directory", 19, 0);
                    continue;
                }
                else {
                    send(Client_status, "Unload............", 19, 0);
                }
                //Считывания и оправка информации клиенту
                char DataFileUnload[SIZEBUF] = { 0 };
                nameFileUnload.erase(0, 2);
                std::ifstream FileUnload(nameFileUnload, std::ios::binary);
                while (FileUnload.read(DataFileUnload, SIZEBUF) || FileUnload.gcount() > 0) {
                    int bytesRead = FileUnload.gcount();
                    int bytesSent = 0;
                    while (bytesSent < bytesRead) {
                        int sent = send(Client_status, DataFileUnload + bytesSent, bytesRead - bytesSent, 0);
                        if (sent == -1) {
                            FileUnload.close();
                            break;
                        }
                        bytesSent += sent;
                    }
                }
                shutdown(Client_status, 1);
            }

            //Модуль закрытия сервера
            else if(command == "Exit"){
                closesocket(Server);
                WSACleanup();
                break;
            }

            continue;
        }
    }
    return 1;
}
