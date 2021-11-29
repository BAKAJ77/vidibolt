#ifndef VIDIBOLT_TCP_CLIENT_H
#define VIDIBOLT_TCP_CLIENT_H

#include <util/volt_api.h>
#include <util/ts_deque.h>
#include <util/error_identifier.h>
#include <net/message.h>

#include <string>
#include <memory>

namespace Volt
{
	/*
		Handles client side operations for outbound connections.
	*/
	class TCPClient
	{
	private:
		class Implementation;
		std::shared_ptr<Implementation> impl;
	public:
		VOLT_API TCPClient(uint32_t port);
		VOLT_API ~TCPClient() = default;

		/*
			Connects to server-side of peer with the ipv4 address specified.
		*/
		VOLT_API ErrorID Connect(const std::string& address);

		/*
			Closes the currently open connection.
		*/
		VOLT_API void Disconnect();

		/*
			Pushes message data into pending queue in order to be transmitted.
		*/
		VOLT_API void PushOutboundMessage(const Message& msg);

		/*
			Transmits all pending messages and recieves all pending incoming messages.
		*/
		VOLT_API ErrorID UpdateState();

		/*
			Returns a queue of recieved messages.
		*/
		VOLT_API Deque<RecievedMessage>& GetInboundMessages();

		/*
			Returns the port number that connections are being opened on by the client.
		*/
		VOLT_API const uint32_t& GetPort() const;
	};
}

#endif