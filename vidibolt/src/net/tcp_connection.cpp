#include <net/tcp_connection.h>

#include <boost/bind/bind.hpp>
#include <vector>

namespace Volt
{
	class Connection::Implementation
	{
	private:
		uint32_t id;

		asio::io_context ctx;
		asio::ip::tcp::socket socket;
		Deque<RecievedMessage>& inboundMsgs;
		Deque<Message> outboundMsgs;
	private:
		/*
			Callback handler function which checks for errors once the message data has been sent.
		*/
		void OnTransmissionCompletion(const system::error_code& ec, size_t bytesSent, system::error_code& ecOut)
		{
			// Check for errors that occured when data was transmitted
			if (ec)
				ecOut = ec;
		}

		/*
			Callback handler function which receieves and reads the payload data of the incoming message once the header
			of the message has been fully recieved and read, also errors are checked for.
		*/
		void OnRecieveCompletion(const system::error_code& ec, size_t bytesRecieved, const std::vector<uint8_t>& buffer,
			system::error_code& ecOut)
		{
			// Check for errors that occured when data was recieving incoming data
			if (ec)
				ecOut = ec;

			// Copy recieved data in buffer into the message object, 
			// this message is then pushed into the inbound message queue
			if (!ec)
			{
				RecievedMessage inboundMsg;
				inboundMsg.connectionID = id;
				memcpy(&inboundMsg.transmittedMsg.header, buffer.data(), sizeof(Header));

				this->SetupBuffer(inboundMsg.transmittedMsg.payload, inboundMsg.transmittedMsg.header.sizeBytes);
				memcpy(inboundMsg.transmittedMsg.payload.data(), buffer.data() + sizeof(Header), 
					inboundMsg.transmittedMsg.header.sizeBytes);

				this->inboundMsgs.PushBackElement(inboundMsg);
			}
		}

		/*
			Sets up the vector buffer given by allocating the amount of memory specified.
			The data in the buffer (if any) is cleared before allocating by default but this can be prevented via the 
			'clearBuffer' parameter.
		*/
		void SetupBuffer(std::vector<uint8_t>& buffer, size_t bufferSizeReq, bool clearBuffer = true) const
		{
			if(clearBuffer)
				buffer.clear();

			buffer.resize(bufferSizeReq);
		}
	public:
		Implementation(Deque<RecievedMessage>& msgsIn) :
			socket(this->ctx), inboundMsgs(msgsIn)
		{
			// Assign the connection an ID
			static uint32_t idCounter = 0;
			this->id = idCounter++;
		}

		~Implementation()
		{
			this->CloseSocket();
		}

		void PushOutboundMessage(const Message& msg)
		{
			this->outboundMsgs.PushBackElement(msg);
		}

		void CloseSocket()
		{
			this->socket.close();
		}

		ErrorID FlushSocket()
		{
			std::vector<uint8_t> transmitBuffer, recieveBuffer;
			system::error_code errorCodeTransmit, errorCodeRecieve;

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
					boost::bind(&Implementation::OnTransmissionCompletion, this, asio::placeholders::error,
						asio::placeholders::bytes_transferred, std::ref(errorCodeTransmit)));
			}

			// Recieve the next inbound message (if any)
			if (this->socket.is_open() && this->socket.available() > 0)
			{
				// Setup recieve buffer for header data, 
				// then begin synchronous read to recieve header data of message into buffer
				this->SetupBuffer(recieveBuffer, sizeof(Header));

				asio::read(this->socket, asio::buffer(recieveBuffer.data(), recieveBuffer.size()), 
					asio::transfer_exactly(sizeof(Header)), errorCodeRecieve);

				if (!errorCodeRecieve)
				{
					// Setup the recieve buffer, then begin asynchronous read operation from the socket in stream
					// This asynchronous read operation will only read the header part of the message
					uint32_t payloadSizeBytes = 0;
					memcpy(&payloadSizeBytes, recieveBuffer.data() + sizeof(MessageRepType), sizeof(uint32_t));
					this->SetupBuffer(recieveBuffer, sizeof(Header) + payloadSizeBytes, false);

					asio::async_read(this->socket, asio::buffer(recieveBuffer.data() + sizeof(Header), payloadSizeBytes),
						asio::transfer_exactly(payloadSizeBytes), boost::bind(&Implementation::OnRecieveCompletion, this, 
							asio::placeholders::error, asio::placeholders::bytes_transferred, std::ref(recieveBuffer), 
							std::ref(errorCodeRecieve)));
				}
			}

			this->ctx.restart();
			this->ctx.run();

			if (errorCodeTransmit)
				return (ErrorID)errorCodeTransmit.value();
			else if (errorCodeRecieve)
				return (ErrorID)errorCodeRecieve.value();

			return ErrorID::NONE;
		}

		asio::ip::tcp::socket& GetSocketObject()
		{
			return this->socket;
		}

		bool IsSocketOpen() const
		{
			return this->socket.is_open();
		}

		const uint32_t& GetID() const
		{
			return this->id;
		}
	};

	Connection::Connection(Deque<RecievedMessage>& msgsIn) :
		impl(std::make_unique<Implementation>(msgsIn))
	{}

	Connection::~Connection() = default;

	void Connection::PushOutboundMessage(const Message& msg) 
	{
		this->impl->PushOutboundMessage(msg);
	}

	void Connection::CloseSocket() 
	{
		this->impl->CloseSocket();
	}

	ErrorID Connection::FlushSocket()
	{
		return this->impl->FlushSocket();
	}

	asio::ip::tcp::socket& Connection::GetSocketObject() 
	{
		return this->impl->GetSocketObject();
	}

	bool Connection::IsSocketOpen() const 
	{
		return this->impl->IsSocketOpen();
	}

	const uint32_t& Connection::GetID() const
	{
		return this->impl->GetID();
	}

	ConnectionPtr CreateConnection(Deque<RecievedMessage>& msgsIn) { return std::make_shared<Connection>(msgsIn); }
}