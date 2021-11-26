#ifndef VIDIBOLT_TCP_SERVER_H
#define VIDIBOLT_TCP_SERVER_H

#include <util/volt_api.h>
#include <util/ts_deque.h>
#include <util/error_identifier.h>
#include <net/message.h>

namespace Volt
{
	/*
		Handles server side operations for inbound connections.
	*/
	class VOLT_API TCPServer
	{
	private:
		class Implementation;
		Implementation* impl;
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
		ErrorID PushOutboundResponseMessage(const RecievedMessage& recvMsg, const Message& msgOut);

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
