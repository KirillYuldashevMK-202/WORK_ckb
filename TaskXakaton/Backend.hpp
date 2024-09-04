#ifndef MODULE_BACKEND
#define MODULE_BACKEND

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

#define SIZEMSGBUF 1024

#pragma comment(lib, "Ws2_32.lib")

SOCKET ConnectBackend();

std::string recvBackend(SOCKET ServerSock);

#endif
