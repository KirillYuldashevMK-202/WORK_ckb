#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

#define SIZEMSGBUF 1024

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
		std::cout << "...........Chat created..........." << std::endl;
	}

	~MyServerTCP(){
		closesocket(this->Server);
		WSACleanup();
	}

};

class ProcessingClient {
public:
	std::vector<SOCKET> Clients;
	std::mutex ClientMutex;
	
	ProcessingClient() {};

	void SendMessageClient(const std::string Msg, SOCKET senderClient) {
		std::lock_guard<std::mutex>lock(this->ClientMutex);
		for (SOCKET client : this->Clients){
			if (client != senderClient) {
				send(client, Msg.c_str(), Msg.size(), 0);
			}
		}
	}
	
	void RecvMessageClient(SOCKET recvClient) {
		char MsgBuffer[SIZEMSGBUF] = { 0 };
		while (true) {
			int recvSize = recv(recvClient, MsgBuffer, sizeof(MsgBuffer), 0);
			if (recvSize < 0) {
				std::lock_guard<std::mutex>lock(this->ClientMutex);
				this->Clients.erase(std::remove(this->Clients.begin(), this->Clients.end(), recvClient), this->Clients.end());
				closesocket(recvClient);
				break;
			}
			MsgBuffer[recvSize] = '\0';
			std::string MsgClients = (std::string)MsgBuffer;
			SendMessageClient(MsgClients, recvClient);
		}	
	}

	~ProcessingClient() {};
};

int main() {
	MyServerTCP ServerMain;
	ServerMain.CreateServer();

	ProcessingClient MainClients;
	
	while (true) {
		sockaddr_in Client;
		int SizeClient = sizeof(Client);
		SOCKET ClientSocket = accept(ServerMain.Server, (sockaddr*)&Client, &SizeClient);
		if (ClientSocket != INVALID_SOCKET) {
			std::cout << "Client connected" << std::endl;
		}
		{
			std::lock_guard<std::mutex>lock(MainClients.ClientMutex);
			MainClients.Clients.push_back(ClientSocket);
		}
		std::thread recvThread(&ProcessingClient::RecvMessageClient, &MainClients, ClientSocket);
		recvThread.detach();
	}

	return 0;
}
