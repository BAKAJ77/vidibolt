#ifndef VIDIBOLT_TCP_CLIENT_H
#define VIDIBOLT_TCP_CLIENT_H

#include <util/volt_api.h>
#include <util/ts_deque.h>
#include <util/error_identifier.h>
#include <net/message.h>

#include <string>

namespace Volt
{
	/*
		Handles client side operations for outbound connections.
	*/
	class VOLT_API TCPClient
	{
	private:
		class Implementation;
		Implementation* impl;
	public:
		TCPClient(uint32_t port);
		~TCPClient();

		/*
			Connects to server-side of peer with the ipv4 address specified.
		*/
		ErrorID Connect(const std::string& address);

		/*
			Closes the currently open connection.
		*/
		void Disconnect();

		/*
			Pushes message data into pending queue in order to be transmitted.
		*/
		void PushOutboundMessage(const Message& msg);

		/*
			Transmits all pending messages and recieves all pending incoming messages.
		*/
		ErrorID UpdateState();

		/*
			Returns a queue of recieved messages.
		*/
		Deque<RecievedMessage>& GetInboundMessages();

		/*
			Returns the port number that connections are being opened on by the client.
		*/
		const uint32_t& GetPort() const;
	};
}

#endif