#include <net/tcp_client.h>
#include <util/error_handler.h>

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

	void TCPClient::Connect(const std::string& address)
	{
		// Setup connection shared pointer object, then resolve endpoints
		ConnectionPtr connection = Volt::CreateConnection(this->inboundMsgs);
		auto endpoints = this->resolver.resolve(address, std::to_string(this->port));

		// Start the synchronous connect operation
		system::error_code ec;
		asio::connect(connection->GetSocketObject(), endpoints, ec);

		// Check for errors
		if (ec)
			ErrorHandler::GetHandler().PushError(ec.message(), ErrorID::CONNECTION_INIT_ERROR, false);
		else
			this->outboundConnection = std::move(connection);
	}

	void TCPClient::Disconnect() { this->outboundConnection.reset(); }

	void TCPClient::PushOutboundMessage(const Message& msg)
	{
		if (this->outboundConnection)
			this->outboundConnection->PushOutboundMessage(msg);
	}

	void TCPClient::UpdateState() { this->outboundConnection->FlushSocket(); }
	Deque<RecievedMessage>& TCPClient::GetInboundMessages() { return this->inboundMsgs; }
	const uint32_t& TCPClient::GetPortNumber() const { return this->port; }
}