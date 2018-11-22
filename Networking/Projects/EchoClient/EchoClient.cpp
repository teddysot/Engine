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

	const char* hostAddress = "127.0.0.1";
	uint16_t port = 8888;

	// Check for arguments
	if (argc >= 2)
	{
		hostAddress = argv[1];
	}
	if (argc >= 3)
	{
		port = static_cast<uint16_t>(atoi(argv[1]));
	}

	// Initialize Winsock version 2.2
	WSAData wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup failed.\n");
		return -1;
	}

	// Creates a socket using TCP/IP
	SOCKET mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mySocket == INVALID_SOCKET)
	{
		printf("Failed to create socket.\n");
		return -1;
	}

	// Resolve host using name
	// HOSTENT* hostEntry = gethostbyname("localhost");

	// Resolve host using address
	in_addr iaHost;
	iaHost.s_addr = inet_addr(hostAddress);
	HOSTENT* hostEntry = gethostbyaddr((const char *)&iaHost, sizeof(struct in_addr), AF_INET);
	if (hostEntry == nullptr)
	{
		printf("gethostbyaddr failed.\n");
		return -1;
	}

	// Fill host address information and connect
	SOCKADDR_IN serverInfo;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	serverInfo.sin_port = htons(8888);
	result = connect(mySocket, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr));
	if (result == SOCKET_ERROR)
	{
		printf("connect failed.\n");
		return -1;
	}

	printf("Connected to server!\n");
	printf("Enter Message: ");

	std::string message;
	getline(std::cin, message);

	printf("Sending: %s\n", message.c_str());

	// Send user message
	send(mySocket, message.c_str(), (int)message.length(), 0);

	// Wait for server response **Blocking**
	char buffer[MAX_BUFFER_SIZE];
	int bytesReceived = recv(mySocket, buffer, MAX_BUFFER_SIZE - 1, 0);
	if (bytesReceived == SOCKET_ERROR)
	{
		printf("recv failed.\n");
		return -1;
	}
	if (bytesReceived == 0)
	{
		printf("Connection Closed.\n");
	}
	else
	{
		buffer[bytesReceived] = '\0';
		printf("Received: %s\n", buffer);
	}

	// Close all sockets
	closesocket(mySocket);

	system("pause");
	return 0;
}