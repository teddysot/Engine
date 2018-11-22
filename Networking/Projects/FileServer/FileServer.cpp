#include <WinSock2.h>


#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


inline void WCharToChar(const wchar_t* input, char* output, int strLen)
{
	WideCharToMultiByte(CP_UTF8, 0, input, -1, output, strLen, nullptr, nullptr);
}

constexpr int kMaxBufferSize = 2048;

using namespace std::experimental;

int main(int argc, char** argv)
{
	uint16_t port = 8888;
	auto p = filesystem::canonical(filesystem::path("../Data"));
	if (!filesystem::is_directory(p))
	{
		return -1;
	}
	std::vector<filesystem::path> files;
	std::copy(filesystem::directory_iterator(p),
		filesystem::directory_iterator(),
		std::back_inserter(files));
	std::sort(files.begin(), files.end());

	auto file = files[0].filename();

	if (argc >= 2)
	{
		port = static_cast<uint16_t>(atoi(argv[1]));
	}
	// Network::Socket socket;

	WSAData wsaData;
	// Initialize WSA
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != result)
	{
		printf("WSAStartup failed.\n");
		return -1;
	}

	// Creates a listener socket using TCP/IP
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == listener)
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
	result = bind(listener, (LPSOCKADDR)&serverInfo, sizeof(sockaddr));
	if (SOCKET_ERROR == result)
	{
		printf("Failed to bind socket to port");
		return -1;
	}

	listen(listener, 10);
	if (SOCKET_ERROR == result)
	{
		printf("Socket failed to listen");
		return -1;
	}

	printf("Listening on port %hu...\n", port);

	// Serve forever...
	while (true)
	{
		// Accept any incoming client connection **Blocking**
		SOCKET client = accept(listener, nullptr, nullptr);
		if (INVALID_SOCKET == client)
		{
			printf("Accept failed\n");
			continue;
		}

		printf("Client connected!\n");

		// Serve this client
		char buffer[kMaxBufferSize];

		int bytesReceived;
		while ((bytesReceived = recv(client, buffer, kMaxBufferSize - 1, 0)) > 0)
		{
			if (SOCKET_ERROR == bytesReceived)
			{
				printf("recv failed. \n");
				closesocket(client);
				continue;
			}
			buffer[bytesReceived] = '\0';

			if (0 == strcmp(buffer, "list"))
			{
				// send file list
				std::ostringstream sOut;
				for (auto file : files)
				{
					auto filename = file.filename().string();
					sOut << filename << ",";
				}
				auto string = sOut.str();
				send(client, string.c_str(), string.length(), 0);
			}
			else
			{
				for (auto file : files)
				{
					if (!filesystem::is_regular_file(file))
					{
						continue;
					}
					auto filename = file.filename().string();
					if (0 == strcmp(buffer, filename.c_str()))
					{
						// Send File
						/*auto fn = file.c_str();
						char fnbuf[1024];
						WCharToChar(fn, fnbuf, 1024);*/

						std::ifstream fileStream;

						int fileSize = (int)filesystem::file_size(file);
						fileStream.open(file, std::ios::in | std::ios::binary);
						char* filebuffer = new char[fileSize];
						if (!fileStream.is_open())
						{
							printf("File failed to open");
							closesocket(client);
							closesocket(listener);
							return -1;
						}
						printf("Sending %s, file size: %d bytes", filename.c_str(), fileSize);

						printf("Sending File size\n");

						std::ostringstream oStream;
						oStream << fileSize;
						auto fSizeStr = oStream.str();
						send(client, fSizeStr.c_str(), fSizeStr.size(), 0);

						fileStream.read(filebuffer, fileSize);

						int i = 0;
						int sent = send(client, buffer, fileSize, 0);
						for (; i < fileSize; i += kMaxBufferSize)
						{
							if ((fileSize - i) > kMaxBufferSize)
							{
								printf("Sending %d bytes\n", kMaxBufferSize);
								int actual = send(client, filebuffer + i, kMaxBufferSize, 0);

								printf("actual bytes sent: %d\n", actual);
								printf("Total bytes sent: %d, %.2f%% complete\n", i + kMaxBufferSize, 100.0f *(float)(i + kMaxBufferSize) / (float)fileSize);
							}
							else
							{
								printf("Sending %d bytes\n", fileSize - i);
								int actual = send(client, filebuffer + i, fileSize - i, 0);
								printf("actual bytes sent: %d\n", actual);
								printf("Total bytes sent: %d, %.2f%% complete\n", fileSize, 100.0f);
							}

						}
						delete[] filebuffer;
						fileStream.close();
						break;
					}
				}
			}
		}
		printf("Connection closed.\n");
		closesocket(client);
		// Close client connection
		// TODO:
		// - check incoming request
		// i) if "list", response with a list of file names
		// ii) if a file name is received, send that file
	}
	// Close listener socket
	closesocket(listener);

	// Shutdown Winsock
	WSACleanup();

	system("pause");
	return 0;
}