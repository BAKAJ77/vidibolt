#ifndef VIDIBOLT_TCP_CONNECTION_H
#define VIDIBOLT_TCP_CONNECTION_H

#include <net/message.h>
#include <util/volt_api.h>
#include <util/ts_deque.h>
#include <util/error_identifier.h>

#include <boost/asio.hpp>
#include <memory>

using namespace boost;

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Handles transmission of data between two endpoints.
	class Connection
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	public:
		VOLT_API Connection(Deque<RecievedMessage>& msgsIn);
		VOLT_API Connection(const Connection& connection) = delete;

		VOLT_API ~Connection();
		
		VOLT_API void operator=(const Connection& connection) = delete;

		// Pushes message to pending queue. Messages in the queue are transmitted once 'FlushSocket()' is called.
		VOLT_API void PushOutboundMessage(const Message& msg);

		// Closes the socket which the connection is established through.
		VOLT_API void CloseSocket();

		// Only transmits all pending outbound messages.
		// An error code is returned in the event of an error occurring.
		VOLT_API ErrorCode TransmitOutboundOnly();

		// Transmits all pending outbound messages and recieves all pending inbound messages.
		// An error code is returned in the event of an error occurring.
		VOLT_API ErrorCode FlushSocket();

		// Returns the socket which the connection is established through.
		VOLT_API asio::ip::tcp::socket& GetSocketObject();

		// Returns TRUE if the socket is open, else FALSE is returned.
		VOLT_API bool IsSocketOpen() const;

		// Returns the ID assigned to the connection object.
		VOLT_API const uint32_t& GetID() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	typedef std::shared_ptr<Connection> ConnectionPtr;

	// Returns shared pointer of the initialized connection object.
	extern VOLT_API ConnectionPtr CreateConnection(Deque<RecievedMessage>& msgsIn);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif