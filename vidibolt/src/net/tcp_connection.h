#ifndef VIDIBOLT_TCP_CONNECTION_H
#define VIDIBOLT_TCP_CONNECTION_H

#include <net/message.h>
#include <util/volt_api.h>
#include <util/thread_safe_deque.h>
#include <util/error_identifier.h>

#include <boost/asio.hpp>
#include <vector>
#include <memory>

using namespace boost;

namespace Volt
{
	/*
		Handles transmission of data between two endpoints.
	*/
	class VOLT_API Connection : public std::enable_shared_from_this<Connection>
	{
	private:
		asio::io_context ctx;
		asio::ip::tcp::socket socket;
		Deque<RecievedMessage>& inboundMsgs;
		Deque<Message> outboundMsgs;
	private:
		/*
			Callback handler function which checks for errors once the message data has been sent.
		*/
		void OnTransmissionCompletion(const system::error_code& ec, size_t bytesSent, system::error_code& ecOut);

		/*
			Callback handler function which receieves and reads the payload data of the incoming message once the header
			of the message has been fully recieved and read, also errors are checked for.
		*/
		void OnRecieveCompletion(const system::error_code& ec, size_t bytesRecieved, const std::vector<uint8_t>& buffer,
			system::error_code& ecOut);

		/*
			Sets up the vector buffer given by allocating the amount of memory specified.
			The data in the buffer (if any) is cleared before allocating by default but this can be prevented via the 
			'clearBuffer' parameter.
		*/
		void SetupBuffer(std::vector<uint8_t>& buffer, size_t bufferSizeReq, bool clearBuffer = true) const;
	public:
		Connection(Deque<RecievedMessage>& msgsIn);
		~Connection();

		/*
			Pushes message to pending queue. Messages in the queue are transmitted once 'FlushSocket()' is called.
		*/
		void PushOutboundMessage(const Message& msg);

		/*
			Closes the socket which the connection is established through.
		*/
		void CloseSocket();

		/*
			Transmits all pending outbound messages and recieves all pending inbound messages.
			ErrorID::NONE is returned if no error occurs, else an specific error enum value will be returned based
			on the error that occurred;
		*/
		ErrorID FlushSocket();

		/*
			Returns the socket which the connection is established through.
		*/
		asio::ip::tcp::socket& GetSocketObject();

		/*
			Returns TRUE if the socket is open, else FALSE is returned.
		*/
		bool IsSocketOpen() const;
	};

	typedef std::shared_ptr<Connection> ConnectionPtr;

	/*
		Returns shared pointer of the initialized connection object.
	*/
	extern VOLT_API ConnectionPtr CreateConnection(Deque<RecievedMessage>& msgsIn);
}

#endif