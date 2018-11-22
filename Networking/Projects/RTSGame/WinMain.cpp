#include "RTS.h"
#include "World.h"

const uint16_t port = 8888;
Network::TCPSocket tcpSocket;
Network::TCPSocket* client;
bool master = false;

ImVec4 clear_col = ImColor(113, 144, 154);

enum GameState
{
	FrontEnd,
	Gameplay
};

GameState currentState = FrontEnd;

void RunFrontEnd(float deltaTime)
{
	// Do FrontEnd stuff
	// 1. Show a simple window
	{

		static char buffer[256];

		static float f = 0.0f;
		ImGui::Text("Welcome to Noob RTSGame");

		ImGui::NewLine();

		static char ipAddress[256] = "127.0.0.1";
		ImGui::InputText("IP ADDRESS: ", ipAddress, 256);

		// Check if we are client or server
		if (ImGui::Button("Start Server"))
		{
			master = true;

			Network::SocketAddress serverInfo(port);
			tcpSocket.Open();
			tcpSocket.SetNonBlocking(true);
			tcpSocket.Bind(serverInfo);
			tcpSocket.Listen();

			Network::SocketAddress clientInfo;
			while (client == nullptr)
			{
				client = tcpSocket.Accept(clientInfo);
				Sleep(100);
			}
			currentState = Gameplay;
		}
		else if(ImGui::Button("Connect to Server"))
		{
			Network::SocketAddress address(ipAddress, port);
			tcpSocket.Open();
			tcpSocket.SetNonBlocking(true);
			tcpSocket.Connect(address);
			currentState = Gameplay;
		}
	}
}

void RunGameplay(float deltaTime)
{
	// Do Gameplay stuff
	if (master)
	{
		// Save
		if (X::IsKeyPressed(X::Keys::LCONTROL))
		{
			Network::MemoryStream memoryStream;
			Network::StreamWriter writer(memoryStream);
			World::Get()->WriteSnapshot(writer);

			FILE* file = nullptr;
			fopen_s(&file, "save.dat", "wb");
			fwrite(memoryStream.GetData(), memoryStream.GetSize(), 1, file);
			fclose(file);
		}

		// Load
		if (X::IsKeyPressed(X::Keys::RCONTROL))
		{
			FILE* file = nullptr;
			fopen_s(&file, "save.dat", "rb");

			fseek(file, 0L, SEEK_END);
			int fileSize = ftell(file);
			fseek(file, 0L, SEEK_SET);

			char* fileBuffer = (char*)malloc(fileSize * sizeof(char));
			fread(fileBuffer, fileSize, 1, file);
			fclose(file);

			Network::MemoryStream memoryStream((uint8_t*)fileBuffer, fileSize, true);
			Network::StreamReader reader(memoryStream);
			World::Get()->ApplySnapshot(reader);
		}
		////////////////////////////////////////////////////
		// Sending Packages to Client
		////////////////////////////////////////////////////
		CommandList commands = World::Get()->GetCommands();
		World::Get()->ProcessCommands(commands);

		Network::MemoryStream memoryStream;
		Network::StreamWriter writer(memoryStream);
		for (auto c : commands)
		{
			writer.Write(c->GetType());
			c->Serialize(writer);
		}
		client->Send(memoryStream.GetData(), memoryStream.GetSize());

		for (auto c : commands)
		{
			X::SafeDelete(c);
		}
		////////////////////////////////////////////////////
		// Receiving Package from Client
		////////////////////////////////////////////////////
		char buffer[16384];
		int bytesReceived = client->Receive(buffer, 16384);
		if (bytesReceived > 0)
		{
			Network::MemoryStream memoryStream((uint8_t*)buffer, bytesReceived);
			Network::StreamReader reader(memoryStream);

			CommandList commands;
			while (reader.GetRemainingDataSize() > 0)
			{
				int commandType;
				reader.Read(commandType);

				ICommand* c = nullptr;
				switch ((Command)commandType)
				{
				case Command::Spawn:
				{
					c = new SpawnCommand();
					break;
				}
				case Command::Move:
				{
					c = new MoveCommand();
					break;
				}
				}

				c->Deserialize(reader);
				commands.push_back(c);
			}

			World::Get()->ProcessCommands(commands);
			for (auto c : commands)
			{
				X::SafeDelete(c);
			}
		}

	}
	else
	{
		////////////////////////////////////////////////////
		// Receive Packages from Server
		////////////////////////////////////////////////////
		char buffer[16384];
		int bytesReceived = tcpSocket.Receive(buffer, 16384);
		if (bytesReceived > 0)
		{
			Network::MemoryStream memoryStream((uint8_t*)buffer, bytesReceived);
			Network::StreamReader reader(memoryStream);

			CommandList commands;
			while (reader.GetRemainingDataSize() > 0)
			{
				int commandType;
				reader.Read(commandType);

				ICommand* c = nullptr;
				switch ((Command)commandType)
				{
				case Command::Spawn:
				{
					c = new SpawnCommand();
					break;
				}
				case Command::Move:
				{
					c = new MoveCommand();
					break;
				}
				}

				c->Deserialize(reader);
				commands.push_back(c);
			}

			World::Get()->ProcessCommands(commands);
			for (auto c : commands)
			{
				X::SafeDelete(c);
			}
		}
		////////////////////////////////////////////////////
		// Sending Packages to Server
		////////////////////////////////////////////////////
		CommandList commands = World::Get()->GetCommands();
		World::Get()->ProcessCommands(commands);

		Network::MemoryStream memoryStream;
		Network::StreamWriter writer(memoryStream);
		for (auto c : commands)
		{
			writer.Write(c->GetType());
			c->Serialize(writer);
		}
		tcpSocket.Send(memoryStream.GetData(), memoryStream.GetSize());

		for (auto c : commands)
		{
			X::SafeDelete(c);
		}
	}

	RTS::UpdateCamera(deltaTime);
	World::Get()->Update(deltaTime);

	RTS::DrawGround();
	World::Get()->Draw(master);
	RTS::DrawCursor();
}

bool GameLoop(float deltaTime)
{
	switch (currentState)
	{
	case FrontEnd: RunFrontEnd(deltaTime); break;
	case Gameplay: RunGameplay(deltaTime); break;
	}

	if (X::IsKeyPressed(X::Keys::F))
	{
		currentState = FrontEnd;
	}
	else if (X::IsKeyPressed(X::Keys::G))
	{
		currentState = Gameplay;
	}

	return X::IsKeyPressed(X::Keys::ESCAPE);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Network::Initialize();
	X::Start("RTS Game", hInstance, 1280, 768);
	World::Initialize();

	X::Run(GameLoop);

	World::Terminate();
	X::Stop();
	Network::Terminate();

	return 0;
}