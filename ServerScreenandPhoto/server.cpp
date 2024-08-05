#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <fstream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define SIZEMSGBUF 10000000

class MyServerTCP {
public:
	int Status;
	SOCKET Server;
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
			std::cout << "WSAStartup Error : " << Status << std::endl;
			exit(1);
		}
	}

	void CreateSocket() {
		this->Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (this->Server == INVALID_SOCKET) {
			std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
			closesocket(this->Server);
			WSACleanup();
			exit(1);
		}
	}

	void BindingAdress() {
		this->Status = inet_pton(AF_INET, "127.0.0.1", &Address.sin_addr);
		if (this->Status != 1) {
			std::cout << "inet_pton Error : " << WSAGetLastError() << std::endl;
			closesocket(this->Server);
			WSACleanup();
			exit(1);
		}
		this->Status = bind(this->Server, (sockaddr*)&(this->Address), sizeof(this->Address));
		if (this->Status == SOCKET_ERROR) {
			std::cout << "Bind Error : " << WSAGetLastError() << std::endl;
			closesocket(this->Server);
			WSACleanup();
			exit(1);
		}
		this->Status = listen(this->Server, SOMAXCONN);
		if (this->Status == SOCKET_ERROR) {
			std::cout << "Listen Error : " << WSAGetLastError() << std::endl;
			closesocket(this->Server);
			WSACleanup();
			exit(1);
		}
	}

	void CreateServer() {
		Startup();
		CreateSocket();
		BindingAdress();
		std::cout << "...........Server created..........." << std::endl;
	}

	~MyServerTCP() {
		closesocket(this->Server);
		WSACleanup();
	}

};

class ProcessingClient {
public:
	ProcessingClient() {};

	void SendCommandClient(const std::string Msg, SOCKET Client) {
		send(Client, Msg.c_str(), Msg.size(), 0);
	}

	void RecvBmpClient(SOCKET recvClient) {
		char* MsgBuffer = new char[SIZEMSGBUF];
		std::string namefile;
		memset(MsgBuffer, 0, 0);
		int recvSizeF = recv(recvClient, MsgBuffer, SIZEMSGBUF, 0);
		if (recvSizeF < 0) {
			closesocket(recvClient);
			return;
		}
		MsgBuffer[recvSizeF] = '\0';
		std::ofstream FileClient(MsgBuffer, std::ios::binary);
		if (FileClient.is_open()) {
			memset(MsgBuffer, 0, 0);
			int recvSizeD = recv(recvClient, MsgBuffer, SIZEMSGBUF, 0);
			if (recvSizeD < 0) {
				closesocket(recvClient);
				return;
			}
			MsgBuffer[recvSizeD] = '\0';
			FileClient.write(MsgBuffer, recvSizeD);
			FileClient.close();

		}
	}

	~ProcessingClient() {};
};


int main() {
	MyServerTCP ServerMain;
	ServerMain.CreateServer();
	ProcessingClient ClientMain;

	sockaddr_in Client;
	int SizeClient = sizeof(Client);
	SOCKET ClientSocket = accept(ServerMain.Server, (sockaddr*)&Client, &SizeClient);
	if (ClientSocket != INVALID_SOCKET) {
		std::cout << "Client connected" << std::endl;
	}

	while (true) {
		std::string commandMain;
		getline(std::cin, commandMain);
		ClientMain.SendCommandClient(commandMain, ClientSocket);
		ClientMain.RecvBmpClient(ClientSocket);
	}

  return 0;
}
