//====================================================================================================
// Filename:	EchoClient2.cpp
// Created by:	Peter Chan
//====================================================================================================

#include <Network/Inc/Network.h>

#define MAX_BUFFER_SIZE	1024

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
		port = static_cast<uint16_t>(atoi(argv[2]));
	}

	// Initialize Network library
	if (!Network::Initialize())
	{
		printf("Network initialization failed.\n");
		return -1;
	}

	// Create a socket using TCP/IP
	Network::TCPSocket* mySocket = new Network::TCPSocket();

	// Connect to server
	Network::SocketAddress serverInfo(hostAddress, port);
	if (!mySocket->Connect(serverInfo))
	{
		printf("connect failed.\n");
		return -1;
	}

	printf("Connected to server!\n");
	printf("Enter message: ");

	std::string message;
	getline(std::cin, message);

	printf("Sending:  %s\n", message.c_str());

	// Send user message
	int bytesSent = mySocket->Send(message.c_str(), (int)message.length());
	if (bytesSent == SOCKET_ERROR)
	{
		printf("send failed.\n");
		return -1;
	}

	// Wait for server response **Blocking**
	char buffer[MAX_BUFFER_SIZE];
	int bytesReceived = mySocket->Receive(buffer, MAX_BUFFER_SIZE - 1);
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
	}

	// Close all sockets
	SafeDelete(mySocket);

	// Shutdown Network library
	Network::Terminate();

	system("pause");
	return 0;
}