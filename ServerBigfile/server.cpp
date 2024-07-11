#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>


#define SIZEBUF 32768

#pragma comment(lib, "Ws2_32.lib")

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

    //Хранения сокетов для использования select
    fd_set Mainset;
    fd_set Tmpset;
    FD_ZERO(&Mainset);
    FD_SET(Server, &Mainset);
    SOCKET max = Server;

    std::vector <SOCKET> clientsSocket;


    //Работа сервера, основной цикл
    while (true) {
        Tmpset = Mainset;
        //Определение сокетов, которые готовы выполняться
        Status = select(max + 1, &Tmpset, nullptr, nullptr, nullptr);
        if (Status < 0) {
            std::cout << "Select Error: " << WSAGetLastError() << std::endl;
            closesocket(Server);
            WSACleanup();
            return 1;
        }
        //Обработка всех сокетов
        for (int i = 0; i <= max; ++i) {
            //Проверка был ли установлен сокет 
            if (FD_ISSET(i, &Tmpset)) {
                //Установка соединения 
                if (i == Server) {
                    sockaddr_in Client;
                    int SizeClient = sizeof(Client);

                    SOCKET Client_status = accept(Server, (sockaddr*)&Client, &SizeClient);
                    if (Client_status == INVALID_SOCKET) {
                        std::cout << "Client connection error: " << WSAGetLastError() << std::endl;
                    }
                    //Добавление клиентского сокета
                    else {
                        std::cout << "Client connected" << std::endl;
                        FD_SET(Client_status, &Mainset);
                        clientsSocket.push_back(Client_status);
                        if (Client_status > max) {
                            max = Client_status;
                        }
                    }
                }
                else {
                    //Обработка запроса клиента
                    char buffer[SIZEBUF] = { 0 };
                    int bytesReceived = recv(i, buffer, SIZEBUF, 0);

                    if (bytesReceived <= 0) {
                        std::cout << "Client disconnected" << std::endl;
                        closesocket(i);
                        FD_CLR(i, &Mainset);
                    }
                    else {
                        std::string command;
                        std::string Filename;
                        int startfilename = 0;
                        //Парсинг запроса клиента
                        for (int j = 0; j < bytesReceived; ++j) {
                            if (buffer[j] == ' ') {
                                startfilename = j + 1;
                                break;
                            }
                            command.push_back(buffer[j]);
                        }
                        for (int j = startfilename; j < bytesReceived; ++j) {
                            if (buffer[j] == ' ' || buffer[j] == '\0') {
                                break;
                            }
                            Filename.push_back(buffer[j]);
                        }
                        //Модуль загрузки файла
                        if (command == "Upload") {
                            //Считывание информации и загрузка в файл
                            send(i, "OK", 3, 0);
                            char DataFileUnload[SIZEBUF] = { 0 };
                            int bytesRead;
                            std::ofstream newFile(Filename, std::ios::app);
                            if (!newFile.is_open()) {
                                std::cerr << "Failed to create file" << std::endl;
                                return 1;
                            }
                            if ((bytesRead = recv(i, DataFileUnload, SIZEBUF, 0)) > 0) {
                                newFile.write(DataFileUnload, bytesRead);
                            }
                            newFile.close();
                        }
                        //Модуль выгрузки файла
                        else if (command == "Unload") {
                            //Загрузка имен всех файлов в вектор
                            std::string path = ".";
                            std::vector<std::string> filename_vector;
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
                            send(i, filename_str.c_str(), filename_str.size(), 0);
                            //Поиск нужного файла для выгрузки и отправка клиенту результата поиска
                            memset(buffer, 0, SIZEBUF);
                            recv(i, buffer, SIZEBUF, 0);
                            std::string nameFileUnload = buffer;
                            int searchfile = -1;
                            for (int k = 0; k < filename_vector.size(); ++k) {
                                if (filename_vector[k] == nameFileUnload) {
                                    searchfile = k;
                                }
                            }
                            //Отправка сообщения клиенту о результате поиска
                            if (searchfile == -1) {
                                send(i, "File not found", 14, 0);
                            }
                            //Считывания и оправка информации клиенту
                            else {
                                send(i, "Unload............", 19, 0);
                                char DataFileUnload[SIZEBUF] = { 0 };
                                nameFileUnload.erase(0, 2);
                                std::ifstream FileUnload(nameFileUnload, std::ios::binary);
                                while (FileUnload.read(DataFileUnload, SIZEBUF) || FileUnload.gcount() > 0) {
                                    int bytesRead = FileUnload.gcount();
                                    int bytesSent = 0;
                                    while (bytesSent < bytesRead) {
                                        int sent = send(i, DataFileUnload + bytesSent, bytesRead - bytesSent, 0);
                                        if (sent == -1) {
                                            FileUnload.close();
                                            break;
                                        }
                                        bytesSent += sent;
                                    }
                                }
                                FileUnload.close();
                                shutdown(i, SD_SEND);
                            }
                        }
                        //Модуль закрытия сервера
                        else if (command == "Exit") {
                            closesocket(Server);
                            WSACleanup();
                            return 0;
                        }
                    }
                }
            }
        }
    }
    closesocket(Server);
    WSACleanup();
    return 1;
}
