//====================================================================================================
// Filename:	EchoServer2.cpp
// Created by:	Peter Chan
//====================================================================================================

#include <Network/Inc/Network.h>

#define MAX_BUFFER_SIZE	1024

int main(int argc, char* argv[])
{
	// Defaults
	uint16_t port = 8888;

	// Check for arguments
	if (argc >= 2)
	{
		port = static_cast<uint16_t>(atoi(argv[1]));
	}

	// Initialize Network library
	if (!Network::Initialize())
	{
		printf("Network initialization failed.\n");
		return -1;
	}

	// Create a listener socket using TCP/IP
	Network::TCPSocket* listener = new Network::TCPSocket();

	// Bind the address to our listener socket and listen for connection
	Network::SocketAddress serverInfo(port);
	if (!listener->Bind(serverInfo))
	{
		printf("bind failed.\n");
		return -1;
	}
	if (!listener->Listen(10))
	{
		printf("listen failed.\n");
		return -1;
	}

	printf("Listening on port %hu...\n", port);

	// Serve forever...
	while (true)
	{
		// Accept any incoming client connection **Blocking**
		Network::SocketAddress clientAddress;
		Network::TCPSocket* client = listener->Accept(clientAddress);
		if (client == nullptr)
		{
			printf("accept failed.\n");
			continue;
		}

		printf("Client connected!\n");

		// Wait for client message
		char buffer[MAX_BUFFER_SIZE];
		int bytesReceived = client->Receive(buffer, MAX_BUFFER_SIZE - 1);
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
			int bytesSent = client->Send(buffer, bytesReceived);
			if (bytesSent == SOCKET_ERROR)
			{
				printf("send failed.\n");
				return -1;
			}
		}

		// Close client connection
		SafeDelete(client);
	}

	// Close listener socket
	SafeDelete(listener);

	// Shutdown Network library
	Network::Terminate();

	system("pause");
	return 0;
}