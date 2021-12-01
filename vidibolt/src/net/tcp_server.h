#ifndef VIDIBOLT_TCP_SERVER_H
#define VIDIBOLT_TCP_SERVER_H

#include <util/volt_api.h>
#include <util/ts_deque.h>
#include <util/error_identifier.h>
#include <net/message.h>

#include <memory>

namespace Volt
{
	// Handles server side operations for inbound connections.
	class TCPServer
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	public:
		VOLT_API TCPServer(uint32_t port, bool startListener = true);
		VOLT_API TCPServer(const TCPServer& server) = delete;

		VOLT_API ~TCPServer();

		VOLT_API void operator=(const TCPServer& server) = delete;

		// Starts up operation for listening for new inbound connections.
		VOLT_API void StartListener();

		// Stops the connection listening operation.
		VOLT_API void StopListener();

		// Pushes a message to be transmitted through the connection which the recieved message came from.
		// Returns an error code in the event of a failure occurring.
		VOLT_API ErrorCode PushOutboundResponseMessage(const RecievedMessage& recvMsg, const Message& msgOut);

		// Pushes a message to be transmitted to all open connections in the queue.
		VOLT_API void BroadcastOutboundMessage(const Message& msg);

		// All pending [outbound/inbound] messages in the various open connections are [transmitted/recieved].
		VOLT_API void UpdateState();

		// Returns the messgaes recieved, the queue holds messages recieved from various connections.
		VOLT_API Deque<RecievedMessage>& GetInboundMessages();

		// Returns the port number on which connections are being listened for.
		VOLT_API const uint32_t& GetPortNumber() const;

		// Returns TRUE if the listener is actively listening for new connections, else FALSE is returned.
		VOLT_API bool IsListening() const;

		// Returns an error code indicating whether an error (relating to the listening operation) has occured.
		VOLT_API const ErrorCode& GetListenerErrorState() const;
	};
}

#endif
