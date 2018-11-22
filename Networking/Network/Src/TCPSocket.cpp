#include "Precompiled.h"
#include "TCPSocket.h"

using namespace Network;

TCPSocket::TCPSocket()
	: mSocket(INVALID_SOCKET)
{
}

//----------------------------------------------------------------------------------------------------

TCPSocket::~TCPSocket()
{
	Close();
}

//----------------------------------------------------------------------------------------------------

bool TCPSocket::Open()
{
	ASSERT(mSocket == INVALID_SOCKET, "[TCPSocket] Socket already opened!");
	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		LOG("[TCPSocket] Failed to open socket.");
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------

void TCPSocket::Close()
{
	if (mSocket != INVALID_SOCKET)
	{
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}
}

//----------------------------------------------------------------------------------------------------

bool TCPSocket::SetNoDelay(bool noDelay)
{
	int delayArg = noDelay ? 1 : 0;
	int result = setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&delayArg, sizeof(int));
	if (result == SOCKET_ERROR)
	{
		LOG("[TCPSocket] Failed to set no delay. Error: %d", WSAGetLastError());
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------

bool TCPSocket::SetNonBlocking(bool nonBlocking)
{
	u_long arg = nonBlocking ? 1 : 0;
	int result = ioctlsocket(mSocket, FIONBIO, &arg);
	if (result == SOCKET_ERROR)
	{
		LOG("[TCPSocket] Failed to set non blocking mode. Error: %d", WSAGetLastError());
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------

bool TCPSocket::Connect(const SocketAddress& address)
{
	if (mSocket == INVALID_SOCKET && !Open())
	{
		return false;
	}

	int result = connect(mSocket, &address.mSockAddr, address.GetSize());
	if (result == SOCKET_ERROR)
	{
		int lastError = WSAGetLastError();
		if (lastError == WSAEWOULDBLOCK)
		{
			fd_set write, err;
			FD_ZERO(&write);
			FD_ZERO(&err);
			FD_SET(mSocket, &write);
			FD_SET(mSocket, &err);

			TIMEVAL timeout = { 10,0 }; // timeout after 10 seconds
			result = select(0, NULL, &write, &err, &timeout);
			if (result == 0)
			{
				LOG("[TCPSocket] Failed to connect socket.");
				return false;
			}
			else
			{
				if (FD_ISSET(mSocket, &write))
				{
					return true;
				}
				if (FD_ISSET(mSocket, &err))
				{
					LOG("[TCPSocket] Select error.");
					return false;
				}
			}
		}
		else
		{
			LOG("[TCPSocket] Failed to connect socket. Error %d", lastError);
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------

bool TCPSocket::Bind(const SocketAddress& address)
{
	if (mSocket == INVALID_SOCKET && !Open())
	{
		return false;
	}

	int result = bind(mSocket, &address.mSockAddr, address.GetSize());
	if (result == SOCKET_ERROR)
	{
		LOG("[TCPSocket] Failed to bind socket. Error: %d", WSAGetLastError());
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------

bool TCPSocket::Listen(int backLog)
{
	int err = listen(mSocket, backLog);
	if (err == SOCKET_ERROR)
	{
		LOG("[TCPSocket] Failed to listen for connection. Error: %d", WSAGetLastError());
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------

TCPSocket* TCPSocket::Accept(SocketAddress& fromAddress)
{
	socklen_t length = fromAddress.GetSize();
	SOCKET newSocket = accept(mSocket, &fromAddress.mSockAddr, &length);
	if (newSocket == INVALID_SOCKET)
	{
		int lastError = WSAGetLastError();
		if (lastError == WSAEWOULDBLOCK)
		{
			return nullptr;
		}
		else
		{
			LOG("[TCPSocket] Failed to accept connection. Error: %d", WSAGetLastError());
			return nullptr;
		}
	}

	TCPSocket* newConnection = new TCPSocket();
	newConnection->mSocket = newSocket;
	return newConnection;
}

//----------------------------------------------------------------------------------------------------

int TCPSocket::Send(const void* buffer, int len)
{
	int bytesSent = send(mSocket, static_cast<const char*>(buffer), len, 0);
	if (bytesSent == SOCKET_ERROR)
	{
		LOG("[TCPSocket] Failed to send data. Error: %d", WSAGetLastError());
		return SOCKET_ERROR;
	}
	return bytesSent;
}

//----------------------------------------------------------------------------------------------------

int TCPSocket::Receive(void* buffer, int len)
{
	int bytesRead = recv(mSocket, static_cast<char*>(buffer), len, 0);
	if (bytesRead == SOCKET_ERROR)
	{
		int lastError = WSAGetLastError();
		if (lastError == WSAEWOULDBLOCK)
		{
			bytesRead = 0;
		}
		else
		{
			LOG("[TCPSocket] Failed to receive data. Error: %d", WSAGetLastError());
			return SOCKET_ERROR;
		}
	}
	return bytesRead;
}