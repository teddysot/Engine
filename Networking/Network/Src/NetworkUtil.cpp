#include "Precompiled.h"
#include "Network.h"

using namespace Network;

bool Network::Initialize()
{
	// Initialize Winsock version 2.2
	WSAData wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result == 0)
	{
		printf("WSAStartup failed.\n");
		return 0;
	}
}

bool Network::Terminate()
{
	// Shutdown Winsock
	int result = WSACleanup();
	if (result == 0)
	{
		printf("WSACleanup failed.\n");
		return 0;
	}
}