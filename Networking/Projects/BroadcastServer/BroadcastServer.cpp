#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <strsafe.h>
#include <vector>
#include <stdio.h>  

// Link to the library
using namespace std;
#pragma comment(lib, "ws2_32.lib")  

#define MAX_BUFFER_SIZE 1024
#define PORT 3779

int main(int argc, char* argv[])
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// socket api  
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}

	// socket  
	SOCKET connect_socket;
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connect_socket)
	{
		err = WSAGetLastError();
		printf("error! error code is %d\n", err);
		return -1;
	}

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = INADDR_BROADCAST;

	bool bOpt = true;
	// SO_REUSEADDR SOL_SOCKET
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	int nAddrLen = sizeof(SOCKADDR);

	char buff[MAX_BUFFER_SIZE] = "";
	int nLoop = 0;
	while (1)
	{
		nLoop++;
		sprintf(buff, "%8d From Teddy", nLoop);

		//  
		int nSendSize = sendto(connect_socket, buff, strlen(buff), 0, (SOCKADDR*)&sin, nAddrLen);
		if (SOCKET_ERROR == nSendSize)
		{
			err = WSAGetLastError();
			printf("error!, error code is %d\n", err);
			return -1;
		}
		printf("Send: %s\n", buff);
		Sleep(500);
	}

	return 0;
}