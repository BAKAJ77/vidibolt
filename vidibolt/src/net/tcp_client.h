#ifndef VIDIBOLT_TCP_CLIENT_H
#define VIDIBOLT_TCP_CLIENT_H

#include <util/volt_api.h>
#include <util/thread_safe_deque.h>
#include <net/tcp_connection.h>
#include <net/message.h>
#include <boost/asio.hpp>

using namespace boost;

namespace Volt
{
	/*
		Handles client side operations for outbound connections.
	*/
	class VOLT_API TCPClient
	{
	private:
		asio::io_context ctx;
		asio::ip::tcp::resolver resolver;
		uint32_t port;

		ConnectionPtr outboundConnection;
		Deque<RecievedMessage> inboundMsgs;
	public:
		TCPClient(uint32_t port);
		~TCPClient();

		/*
			Connects to server-side of peer with the ipv4 address specified.
		*/
		void Connect(const std::string& address);

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
		void UpdateState();

		/*
			Returns a queue of recieved messages.
		*/
		Deque<RecievedMessage>& GetInboundMessages();

		/*
			Returns the port number that connections are being opened on by the client.
		*/
		const uint32_t& GetPortNumber() const;
	};
}

#endif