#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
	// Defaults
	uint16_t port = 8888;

	// Check for arguments
	if (argc >= 2)
	{
		port = static_cast<uint16_t>(atoi(argv[1]));
	}

	// Initialize Winsock Version 2.2
	WSAData wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup failed.\n");
		return -1;
	}

	// Creating a listener socket using TCP/IP
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener == INVALID_SOCKET)
	{
		printf("Failed to create socket.\n");
		return -1;
	}

	// Fill server address information
	SOCKADDR_IN serverInfo;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = INADDR_ANY;
	serverInfo.sin_port = htons(port);

	// Bind the address to our listener socket and listen for connection
	result = bind(listener, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr));
	if (result == SOCKET_ERROR)
	{
		printf("bind failed.\n");
		return -1;
	}
	result = listen(listener, 10);
	if (result == SOCKET_ERROR)
	{
		printf("listen failed.\n");
		return -1;
	}

	printf("Listening on port %hu...\n", port);

	// Serve forever...
	while (true)
	{
		// Accept any incoming client connection **Blocking**
		SOCKET client = accept(listener, NULL, NULL);
		if (client == INVALID_SOCKET)
		{
			printf("accept failed.\n");
			continue;
		}

		printf("Client connected!\n");

		// Wait for client message
		char buffer[MAX_BUFFER_SIZE];
		int bytesReceived = recv(client, buffer, MAX_BUFFER_SIZE - 1, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			printf("recv failed.\n");
			return -1;
		}

		if (bytesReceived == 0)
		{
			printf("Connection closed.\n");
		}
		else
		{
			buffer[bytesReceived] = '\0';
			printf("Received: %s\n", buffer);

			// Echo message back
			int bytesSent = send(client, buffer, bytesReceived, 0);
			if (bytesSent == SOCKET_ERROR)
			{
				printf("Send failed.\n");
				return -1;
			}
		}

		// Close client connection
		closesocket(client);
	}

	// Close listener socket
	closesocket(listener);

	// Shutdown Winsock API
	WSACleanup();

	system("pause");
	return 0;
}