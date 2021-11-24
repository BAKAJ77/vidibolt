#ifndef VIDIBOLT_TCP_SERVER_H
#define VIDIBOLT_TCP_SERVER_H

#include <util/volt_api.h>
#include <util/thread_safe_deque.h>
#include <util/error_identifier.h>
#include <net/tcp_connection.h>
#include <net/message.h>
#include <boost/asio.hpp>
#include <thread>

namespace Volt
{
	/*
		Handles server side operations for inbound connections.
	*/
	class VOLT_API TCPServer
	{
	private:
		asio::io_context ctx;
		asio::ip::tcp::acceptor acceptor;
		std::thread listeningThread;
		uint32_t port;

		Deque<ConnectionPtr> inboundConnections;
		Deque<RecievedMessage> inboundMsgs;
		bool isListening;

		ErrorID listenerErrorState;
	private:
		/*
			Callback hander function which pushes the accepted inbound connection to the queue of open connections if
			no errors are encountered when accepting.
		*/
		void OnConnectionAccept(const system::error_code& ec, ConnectionPtr connection);
	public:
		TCPServer(uint32_t port, bool startListener = true);
		~TCPServer();

		/*
			Starts up operation for listening for new inbound connections.
		*/
		void StartListener();

		/*
			Stops the connection listening operation.
		*/
		void StopListener();

		/*
			Pushes a message to be transmitted through the connection which the recieved message came from.
		*/
		void PushOutboundResponseMessage(const RecievedMessage& recvMsg, const Message& msgOut);

		/*
			Pushes a message to be transmitted to all open connections in the queue.
		*/
		void BroadcastOutboundMessage(const Message& msg);

		/*
			All pending [outbound/inbound] messages in the various open connections are [transmitted/recieved].
		*/
		void UpdateState();

		/*
			Returns the messgaes recieved, the queue holds messages recieved from various connections.
		*/
		Deque<RecievedMessage>& GetInboundMessages();

		/*
			Returns the port number on which connections are being listened for.
		*/
		const uint32_t& GetPortNumber() const;

		/*
			Returns TRUE if the listener is actively listening for new connections, else FALSE is returned.
		*/
		bool IsListening() const;

		/*
			Returns enum value indicating if an error (relating to the listening operation) has occured.
		*/
		const ErrorID& GetListenerErrorState() const;
	};
}

#endif
