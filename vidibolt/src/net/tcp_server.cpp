#include <net/tcp_server.h>
#include <util/error_handler.h>
#include <boost/bind/bind.hpp>

namespace Volt
{
	TCPServer::TCPServer(uint32_t port, bool startListener) :
		port(port), isListening(startListener), acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
		if (startListener)
		{
			this->StartListener();
			this->listeningThread = std::thread([&]() { this->ctx.run(); }); // Run IO context on seperate thread
		}
	}

	TCPServer::~TCPServer() { this->StopListener(); }

	void TCPServer::OnConnectionAccept(const system::error_code& ec, ConnectionPtr connection)
	{
		if (!ec)
			this->inboundConnections.PushBackElement(connection);
		else
			ErrorHandler::GetHandler().PushError(ec.message(), ErrorID::CONNECTION_INIT_ERROR);

		this->StartListener();
	}

	void TCPServer::StartListener()
	{
		// Setup connection shared pointer
		ConnectionPtr connection = Volt::CreateConnection(this->inboundMsgs);

		// Start asynchronous connection accept operation
		this->acceptor.async_accept(connection->GetSocketObject(), boost::bind(&TCPServer::OnConnectionAccept, this,
			asio::placeholders::error, connection));

		if (!this->isListening)
			this->listeningThread = std::thread([&]() { this->ctx.run(); }); // Run IO context on seperate thread
	}

	void TCPServer::StopListener()
	{
		this->ctx.stop();
		if (this->listeningThread.joinable())
			this->listeningThread.join();

		this->isListening = false;
	}

	void TCPServer::PushOutboundResponseMessage(const RecievedMessage& recvMsg, const Message& msgOut)
	{
		recvMsg.connection->PushOutboundMessage(msgOut);
	}

	void TCPServer::BroadcastOutboundMessage(const Message& msg)
	{
		for (size_t i = 0; i < this->inboundConnections.GetSize(); i++)
			this->inboundConnections[i]->PushOutboundMessage(msg);
	}

	void TCPServer::UpdateState()
	{
		// Flush all inbound connection sockets (aka transmit all outbound messages and recieve all inbound messages)
		for (size_t i = 0; i < this->inboundConnections.GetSize(); i++)
		{
			ConnectionPtr& connection = this->inboundConnections[i];
			if (connection && connection->IsSocketOpen())
				connection->FlushSocket();
		}

		// Pop connection if socket has been closed
		if (!this->inboundConnections.IsEmpty())
		{
			const ConnectionPtr& connection = this->inboundConnections.GetFrontElement();
			if (!connection || !connection->IsSocketOpen())
				this->inboundConnections.PopFrontElement();
		}
	}

	Deque<RecievedMessage>& TCPServer::GetInboundMessages() { return this->inboundMsgs; }
	const uint32_t& TCPServer::GetPortNumber() const { return this->port; }
	bool TCPServer::IsListening() const { return this->isListening; }
}