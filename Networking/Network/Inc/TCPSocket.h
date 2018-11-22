#ifndef INCLUDED_NETWORK_TCPSOCKET_H
#define INCLUDED_NETWORK_TCPSOCKET_H

#include "SocketAddress.h"

namespace Network {

	class TCPSocket
	{
	public:
		TCPSocket();
		~TCPSocket();
		
		bool Open();
		void Close();
		bool SetNoDelay(bool noDelay);
		bool SetNonBlocking(bool nonBlocking);

		bool Connect(const SocketAddress& address);
		bool Bind(const SocketAddress& address);
		bool Listen(int backLog = 10);
		TCPSocket* Accept(SocketAddress& fromAddress);

		int Send(const void* buffer, int len);
		int Receive(void* buffer, int len);

		uint32_t GetClientID() { return cID; }
		void SetClientID(uint32_t id) { cID = id; }

	private:
		TCPSocket(const TCPSocket&) = delete;
		TCPSocket& operator=(const TCPSocket&) = delete;

		SOCKET mSocket;
		uint32_t cID = 0;
	};

} // namespace Network

#endif // #ifndef INCLUDED_NETWORK_TCPSOCKET_H