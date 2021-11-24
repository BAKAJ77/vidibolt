#include <net/tcp_connection.h>
#include <util/error_handler.h>
#include <boost/bind/bind.hpp>

namespace Volt
{
	Connection::Connection(Deque<RecievedMessage>& msgsIn) :
		socket(this->ctx), inboundMsgs(msgsIn)
	{}

	Connection::~Connection() { this->CloseSocket(); }

	void Connection::OnTransmissionCompletion(const system::error_code& ec, size_t bytesSent)
	{
		// Check for errors that occured when data was transmitted
		if(ec && ec != asio::error::eof && ec != asio::error::connection_reset && ec != asio::error::not_connected)
			ErrorHandler::GetHandler().PushError(ec.message(), ErrorID::NETWORK_TRANSMIT_ERROR);

		this->CheckConnectionErrors(ec);
	}

	void Connection::OnRecieveCompletion(const system::error_code& ec, size_t bytesRecieved, const std::vector<uint8_t>& buffer)
	{
		// Check for errors that occured when data was recieving incoming data
		if (ec && ec != asio::error::eof && ec != asio::error::connection_reset && ec != asio::error::not_connected)
			ErrorHandler::GetHandler().PushError(ec.message(), ErrorID::NETWORK_TRANSMIT_ERROR);

		this->CheckConnectionErrors(ec);

		// Copy recieved data in buffer into the message object, 
		// this message is then pushed into the inbound message queue
		if (!ec)
		{
			RecievedMessage inboundMsg;
			inboundMsg.connection = this->shared_from_this();
			memcpy(&inboundMsg.transmittedMsg.header, buffer.data(), sizeof(Header));

			this->SetupBuffer(inboundMsg.transmittedMsg.payload, inboundMsg.transmittedMsg.header.sizeBytes);
			memcpy(inboundMsg.transmittedMsg.payload.data(), buffer.data() + sizeof(Header), 
				inboundMsg.transmittedMsg.header.sizeBytes);

			this->inboundMsgs.PushBackElement(inboundMsg);
		}
	}

	void Connection::CheckConnectionErrors(const system::error_code& ec)
	{
		if (ec == asio::error::eof || ec == asio::error::connection_reset || ec == asio::error::not_connected)
		{
			ErrorHandler::GetHandler().PushError(ec.message(), ErrorID::REMOTE_DISCONNECT_ERROR, false);
			this->socket.close();
		}
	}

	void Connection::SetupBuffer(std::vector<uint8_t>& buffer, size_t bufferSizeReq, bool clearBuffer) const
	{
		if(clearBuffer)
			buffer.clear();

		buffer.resize(bufferSizeReq);
	}

	void Connection::PushOutboundMessage(const Message& msg) { this->outboundMsgs.PushBackElement(msg); }
	void Connection::CloseSocket() { this->socket.close(); }

	void Connection::FlushSocket()
	{
		std::vector<uint8_t> transmitBuffer, recieveBuffer;

		// Transmit next message in the pending outbound messages queue
		if (!this->outboundMsgs.IsEmpty() && this->socket.is_open())
		{
			// Get the next outbound message in queue
			const Message OUTBOUND_MSG = this->outboundMsgs.GetFrontElement();
			this->outboundMsgs.PopFrontElement();

			// Setup the transmission buffer
			this->SetupBuffer(transmitBuffer, sizeof(Header) + OUTBOUND_MSG.header.sizeBytes);

			memcpy(transmitBuffer.data(), &OUTBOUND_MSG.header, sizeof(Header));
			memcpy(transmitBuffer.data() + sizeof(Header), OUTBOUND_MSG.payload.data(), OUTBOUND_MSG.payload.size());

			// Begin asynchronous write operation to the socket out stream
			asio::async_write(this->socket, asio::buffer(transmitBuffer.data(), transmitBuffer.size()),
				boost::bind(&Connection::OnTransmissionCompletion, this->shared_from_this(), asio::placeholders::error,
					asio::placeholders::bytes_transferred));
		}

		// Recieve the next inbound message (if any)
		if (this->socket.is_open() && this->socket.available() > 0)
		{
			// Setup recieve buffer for header data, 
			// then begin synchronous read to recieve header data of message into buffer
			this->SetupBuffer(recieveBuffer, sizeof(Header));

			system::error_code ec;
			asio::read(this->socket, asio::buffer(recieveBuffer.data(), recieveBuffer.size()), 
				asio::transfer_exactly(sizeof(Header)), ec);

			// Setup the recieve buffer, then begin asynchronous read operation from the socket in stream
			// This asynchronous read operation will only read the header part of the message
			uint32_t payloadSizeBytes = 0;
			memcpy(&payloadSizeBytes, recieveBuffer.data() + sizeof(MessageRepType), sizeof(uint32_t));
			this->SetupBuffer(recieveBuffer, sizeof(Header) + payloadSizeBytes, false);

			asio::async_read(this->socket, asio::buffer(recieveBuffer.data() + sizeof(Header), payloadSizeBytes), 
				asio::transfer_exactly(payloadSizeBytes), boost::bind(&Connection::OnRecieveCompletion, 
					this->shared_from_this(), asio::placeholders::error, asio::placeholders::bytes_transferred, 
					std::ref(recieveBuffer)));
		}

		this->ctx.restart();
		this->ctx.run();
	}

	asio::ip::tcp::socket& Connection::GetSocketObject() { return this->socket; }
	bool Connection::IsSocketOpen() const { return this->socket.is_open(); }
	ConnectionPtr CreateConnection(Deque<RecievedMessage>& msgsIn) { return std::make_shared<Connection>(msgsIn); }
}