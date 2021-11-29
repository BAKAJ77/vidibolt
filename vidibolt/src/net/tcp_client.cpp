#include <net/tcp_client.h>
#include <net/tcp_connection.h>

#include <boost/asio.hpp>

using namespace boost;

namespace Volt
{
	class TCPClient::Implementation
	{
	private:
		asio::io_context ctx;
		asio::ip::tcp::resolver resolver;
		uint32_t port;

		ConnectionPtr outboundConnection;
		Deque<RecievedMessage> inboundMsgs;
	public:
		Implementation(uint32_t port) :
			port(port), resolver(this->ctx)
		{}

		~Implementation()
		{
			this->ctx.stop();
			this->Disconnect();
		}

		ErrorID Connect(const std::string& address)
		{
			// Setup connection shared pointer object, then resolve endpoints
			ConnectionPtr connection = Volt::CreateConnection(this->inboundMsgs);
			auto endpoints = this->resolver.resolve(address, std::to_string(this->port));

			// Start the synchronous connect operation
			system::error_code ec;
			asio::connect(connection->GetSocketObject(), endpoints, ec);

			// Check for errors
			if (ec)
				return (ErrorID)ec.value();
			else
				this->outboundConnection = std::move(connection);

			return ErrorID::NONE;
		}

		void Disconnect()
		{
			this->outboundConnection.reset();
		}

		void PushOutboundMessage(const Message& msg)
		{
			if (this->outboundConnection)
				this->outboundConnection->PushOutboundMessage(msg);
		}

		ErrorID UpdateState()
		{
			ErrorID error = ErrorID::NONE;
			if (this->outboundConnection)
			{
				error = this->outboundConnection->FlushSocket();
				if (error != ErrorID::NONE)
					this->outboundConnection.reset();
			}
			else
				error = ErrorID::CONNECTION_NO_LONGER_OPEN;

			return error;
		}

		Deque<RecievedMessage>& GetInboundMessages()
		{
			return this->inboundMsgs;
		}

		const uint32_t& GetPort() const
		{
			return this->port;
		}
	};

	TCPClient::TCPClient(uint32_t port) :
		impl(std::make_unique<Implementation>(port))
	{}

	TCPClient::~TCPClient() = default;

	ErrorID TCPClient::Connect(const std::string& address)
	{
		return this->impl->Connect(address);
	}

	void TCPClient::Disconnect() 
	{ 
		this->impl->Disconnect();
	}

	void TCPClient::PushOutboundMessage(const Message& msg)
	{
		this->impl->PushOutboundMessage(msg);
	}

	ErrorID TCPClient::UpdateState() 
	{ 
		return this->impl->UpdateState();
	}

	Deque<RecievedMessage>& TCPClient::GetInboundMessages()
	{ 
		return this->impl->GetInboundMessages(); 
	}

	const uint32_t& TCPClient::GetPort() const 
	{ 
		return this->impl->GetPort(); 
	}
}