//====================================================================================================
// Filename:	SocketAddress.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "SocketAddress.h"

using namespace Network;

//----------------------------------------------------------------------------------------------------

SocketAddress::SocketAddress()
{
	GetAsSockAddrIn()->sin_family = AF_INET;
	GetIP4() = INADDR_ANY;
	GetPort() = 0;
}

//----------------------------------------------------------------------------------------------------

SocketAddress::SocketAddress(const char* host, uint16_t port)
{
	GetAsSockAddrIn()->sin_family = AF_INET;
	GetPort() = htons(port);
	inet_pton(AF_INET, host, &GetIP4());
}

//----------------------------------------------------------------------------------------------------

SocketAddress::SocketAddress(uint16_t port)
{
	GetAsSockAddrIn()->sin_family = AF_INET;
	GetIP4() = INADDR_ANY;
	GetPort() = htons(port);
}

//----------------------------------------------------------------------------------------------------

SocketAddress::SocketAddress(const sockaddr& sockAddr)
{
	memcpy(&mSockAddr, &sockAddr, sizeof(sockaddr));
}

//----------------------------------------------------------------------------------------------------

const char* SocketAddress::GetHost() const
{
	return inet_ntoa(GetAsSockAddrIn()->sin_addr);
}