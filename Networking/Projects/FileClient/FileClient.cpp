

#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <queue>


#include <WinSock2.h>


#pragma comment(lib, "Ws2_32.lib")


#include <windows.h>
#include <cstdint>

namespace GUI
{
	namespace Colors
	{
		const uint32_t Black = 0x00;
		const uint32_t Blue = 0x01;
		const uint32_t Green = 0x02;
		const uint32_t Aqua = 0x03;
		const uint32_t Red = 0x04;
		const uint32_t Purple = 0x05;
		const uint32_t Yellow = 0x06;
		const uint32_t White = 0x07;
		const uint32_t Gray = 0x08;
		const uint32_t LBlue = 0x09;
		const uint32_t LGreen = 0x0A;
		const uint32_t LAqua = 0x0B;
		const uint32_t LRed = 0x0C;
		const uint32_t LPurple = 0x0D;
		const uint32_t LYellow = 0x0E;
		const uint32_t LWhite = 0x0F;
	}
	class ColorManager
	{
	private:
		HANDLE  hStdIn_;
		HANDLE  hStdOut_;
		CONSOLE_SCREEN_BUFFER_INFO console_Buffer_Info_;
	public:
		~ColorManager();
		static ColorManager& Get()
		{
			static ColorManager instance;
			return instance;
		}
		void SetTextColor(const uint32_t& background, const uint32_t& foreground);
		void ResetColor();
	private:
		ColorManager();
		ColorManager(ColorManager& obj) = default;
		ColorManager& operator=(ColorManager& obj) = default;
	};

	ColorManager::ColorManager()
		: hStdOut_(GetStdHandle(STD_OUTPUT_HANDLE))
		, hStdIn_(GetStdHandle(STD_INPUT_HANDLE))
	{
		GetConsoleScreenBufferInfo(hStdOut_, &console_Buffer_Info_);
	}
	ColorManager::~ColorManager()
	{
		ResetColor();
	}
	void ColorManager::SetTextColor(const uint32_t& background, const uint32_t& foreground)
	{
		uint32_t colorCode = background * 0x10 + foreground;
		SetConsoleTextAttribute(hStdOut_, colorCode);
	}
	void ColorManager::ResetColor()
	{
		SetConsoleTextAttribute(hStdOut_, console_Buffer_Info_.wAttributes);
	}

}


inline std::vector<std::string> ParseString(char* cStr, char* regex = "[\\s]+")
{
	std::regex reg(regex);
	std::string str{ cStr };
	std::sregex_token_iterator iter(str.begin(), str.end(), reg, -1);
	std::sregex_token_iterator regEnd;
	return { iter, regEnd };
}

constexpr int kMaxBufferSize = 2048;

int main(int argc, char** argv)
{
	GUI::ColorManager& colorManager = GUI::ColorManager::Get();
	const char* hostAddress = "127.0.0.1";
	uint16_t port = 8888;

	if (argc >= 2)
	{
		hostAddress = argv[1];
	}
	if (argc >= 3)
	{
		port = static_cast<uint16_t>(atoi(argv[2]));
	}

	WSAData wsaData;
	// Initialize WSA
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != result)
	{
		colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LRed);
		printf("WSAStartup failed.\n");
		return -1;
	}

	// Creates a listener socket using TCP/IP
	SOCKET sender = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sender)
	{
		colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LRed);
		printf("Failed to create socket.\n");
		return -1;
	}

	// Fill server address information

	in_addr iaHost;
	iaHost.s_addr = inet_addr(hostAddress);
	HOSTENT* hostEntry = gethostbyaddr((const char*)&iaHost, sizeof(in_addr), AF_INET);

	if (nullptr == hostEntry)
	{
		colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LRed);
		printf("hostEntry incorrect");
		return -1;
	}

	SOCKADDR_IN serverInfo;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	serverInfo.sin_port = htons(port);

	result = connect(sender, (LPSOCKADDR)&serverInfo, sizeof(sockaddr));
	if (SOCKET_ERROR == result)
	{
		colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LRed);
		printf("Failed to connect to server\n");
		return -1;
	}
	colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LGreen);
	printf("Connected to server!\n");
	bool bStillTakingCommands = true;

	colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LYellow);
	printf("Enter commmand: ");

	std::string command;
	getline(std::cin, command);

	if (command == "list")
	{
		printf("Sending: %s\n", command.c_str());

		send(sender, command.c_str(), (int)command.length(), 0);

		char buffer[kMaxBufferSize];
		const int bytesReceived = recv(sender, buffer, kMaxBufferSize - 1, 0);

		if (SOCKET_ERROR == bytesReceived)
		{
			colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LRed);
			printf("recv failed\n");
			return -1;
		}

		if (bytesReceived == 0)
		{
			colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LYellow);
			printf("Connection closed. \n");
		}
		else
		{
			buffer[bytesReceived] = '\0';
			auto list = ParseString(buffer, "[,]+");
			colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LGreen);
			printf("List Generated:\n");

			for (int i = 0; i < list.size(); ++i)
			{
				colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LYellow);
				printf("%d) ", i);
				colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LBlue);
				printf("%s\n", list[i].c_str());
			}
			colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LYellow);
			printf("Specify file number to transfer: ");

			std::string index;
			getline(std::cin, index);

			index;
			auto idx = std::stoi(index);
			send(sender, list[idx].c_str(), list[idx].size(), 0);

			std::ofstream file(list[std::stoi(index)], std::ios::binary);

			int bytesReceived;
			int totalBytesReceived = 0;

			//receive file size
			bytesReceived = recv(sender, buffer, kMaxBufferSize - 1, 0);
			if (SOCKET_ERROR == bytesReceived)
			{
				colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LRed);
				printf("recv failed\n");
				return -1;
			}
			if (0 == bytesReceived)
			{
				colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LYellow);
				printf("Connection closed. \n");
				closesocket(sender);
				return 0;
			}
			buffer[bytesReceived] = '\0';

			auto fileSize = std::stoi(buffer);

			struct Data
			{
				char Buffer[kMaxBufferSize];
				int Bytes;
			};
			Data data;

			std::vector<Data> dataVector;
			dataVector.reserve(fileSize / kMaxBufferSize);
			while ((data.Bytes = recv(sender, data.Buffer, kMaxBufferSize, 0)) > 0)
			{
				if (SOCKET_ERROR == data.Bytes)
				{
					printf("recv failed\n");
					return -1;
				}
				totalBytesReceived += data.Bytes;
				colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LYellow);
				printf("Recieving %d bytes\n", data.Bytes);
				colorManager.ResetColor();
				printf("Total Bytes Received: ");
				colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LYellow);
				printf("%d", totalBytesReceived);
				colorManager.ResetColor();
				printf(", ");
				colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LGreen);
				printf("%.2f%%", 100.0f * (float)totalBytesReceived / (float)fileSize);
				colorManager.ResetColor();
				printf(" complete\n");
				dataVector.push_back(data);
				if (totalBytesReceived >= fileSize)
				{
					break;
				}
			}

			for (const auto& d : dataVector)
			{
				file.write(d.Buffer, d.Bytes);
			}
			if (SOCKET_ERROR == bytesReceived)
			{
				colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LRed);
				printf("recv failed\n");
				closesocket(sender);
				return -1;
			}
			file.close();
			colorManager.SetTextColor(GUI::Colors::Black, GUI::Colors::LYellow);
			printf("Connection closed. \n");
		}
	}
	closesocket(sender);
	// Shutdown Winsock
	WSACleanup();
	system("pause");
	return 0;
}