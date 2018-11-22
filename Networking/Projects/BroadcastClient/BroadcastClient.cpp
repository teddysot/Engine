#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <string>
#include <stdio.h>  

// Link to the library

#define MAX_BUFFER_SIZE 1024
#define PORT 3779
using namespace std;
#pragma comment(lib, "ws2_32.lib")  

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

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
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

	// 
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = 0;

	//  
	SOCKADDR_IN sin_from;
	sin_from.sin_family = AF_INET;
	sin_from.sin_port = htons(PORT);
	sin_from.sin_addr.s_addr = INADDR_BROADCAST;

	//  
	bool bOpt = true;
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	//  
	err = bind(connect_socket, (SOCKADDR*)&sin, sizeof(SOCKADDR));
	if (SOCKET_ERROR == err)
	{
		err = WSAGetLastError();
		printf("error! error code is %d\n", err);
		return -1;
	}

	int nAddrLen = sizeof(SOCKADDR);
	char buff[MAX_BUFFER_SIZE] = "";
	int nLoop = 0;
	while (1)
	{
		//  
		int nSendSize = recvfrom(connect_socket, buff, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&sin_from, &nAddrLen);
		if (SOCKET_ERROR == nSendSize)
		{
			err = WSAGetLastError();
			printf("error! error code is %d\n", err);
			return -1;
		}
		buff[nSendSize] = '\0';
		printf("Recv: %s\n", buff);
	}

	return 0;
}