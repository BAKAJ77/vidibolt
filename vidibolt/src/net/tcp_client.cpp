#include <net/tcp_client.h>

namespace Volt
{
	TCPClient::TCPClient(uint32_t port) :
		port(port), resolver(this->ctx)
	{}

	TCPClient::~TCPClient() 
	{ 
		this->ctx.stop();
		this->Disconnect(); 
	}

	ErrorID TCPClient::Connect(const std::string& address)
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

	void TCPClient::Disconnect() { this->outboundConnection.reset(); }

	void TCPClient::PushOutboundMessage(const Message& msg)
	{
		if (this->outboundConnection)
			this->outboundConnection->PushOutboundMessage(msg);
	}

	ErrorID TCPClient::UpdateState() 
	{ 
		ErrorID error = this->outboundConnection->FlushSocket(); 
		if (error != ErrorID::NONE)
			this->outboundConnection.reset();

		return error;
	}

	Deque<RecievedMessage>& TCPClient::GetInboundMessages() { return this->inboundMsgs; }
	const uint32_t& TCPClient::GetPortNumber() const { return this->port; }
}