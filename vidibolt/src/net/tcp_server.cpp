#include <net/tcp_server.h>
#include <net/tcp_connection.h>
#include <util/ts_unordered_map.h>

#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <thread>

using namespace boost;

namespace Volt
{
	class TCPServer::Implementation
	{
	private:
		asio::io_context ctx;
		asio::ip::tcp::acceptor acceptor;
		std::thread listeningThread;
		uint32_t port;

		UnorderedMap<uint32_t, ConnectionPtr> inboundConnections;
		Deque<RecievedMessage> inboundMsgs;
		bool isListening;

		ErrorID listenerErrorState;
	private:
		/*
			Callback function which handles error checking and initiation of a connection when one is accepted.
		*/
		void OnConnectionAccept(const system::error_code& ec, ConnectionPtr connection)
		{
			if (!ec)
				this->inboundConnections.Insert(connection->GetID(), connection);
			else
				this->listenerErrorState = (ErrorID)ec.value();

			this->StartListener();
		}
	public:
		Implementation(uint32_t port, bool startListener) : port(port), isListening(startListener), 
			acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), listenerErrorState(ErrorID::NONE)
		{
			if (startListener)
			{
				this->StartListener();
				this->listeningThread = std::thread([&]() { this->ctx.run(); }); // Run IO context on seperate thread
			}
		}

		~Implementation()
		{
			this->StopListener();
		}

		void StartListener()
		{
			// Setup connection shared pointer
			ConnectionPtr connection = Volt::CreateConnection(this->inboundMsgs);

			// Start asynchronous connection accept operation
			this->acceptor.async_accept(connection->GetSocketObject(), boost::bind(&Implementation::OnConnectionAccept, this,
				asio::placeholders::error, connection));

			if (!this->isListening)
				this->listeningThread = std::thread([&]() { this->ctx.run(); }); // Run IO context on seperate thread
		}

		void StopListener()
		{
			this->ctx.stop();
			if (this->listeningThread.joinable())
				this->listeningThread.join();

			this->isListening = false;
		}

		ErrorID PushOutboundResponseMessage(const RecievedMessage& recvMsg, const Message& msgOut)
		{
			if (this->inboundConnections.ElementExists(recvMsg.connectionID))
				this->inboundConnections[recvMsg.connectionID]->PushOutboundMessage(msgOut);
			else
				return ErrorID::CONNECTION_NO_LONGER_OPEN;

			return ErrorID::NONE;
		}

		void BroadcastOutboundMessage(const Message& msg)
		{
			for(size_t i = 0; i < this->inboundConnections.GetSize(); i++)
				this->inboundConnections.GetElementAtIndex(i)->PushOutboundMessage(msg);
		}

		void UpdateState()
		{
			// Flush all inbound connection sockets (aka transmit all outbound messages and recieve all inbound messages)
			for (size_t i = 0; i < this->inboundConnections.GetSize(); i++)
			{
				ErrorID errorID = ErrorID::NONE;
				const ConnectionPtr& connection = this->inboundConnections.GetElementAtIndex(i);
				if (connection && connection->IsSocketOpen())
					errorID = connection->FlushSocket();

				// Check for thrown connection errors
				if (errorID == ErrorID::CONNECTION_RESET_ERROR || errorID == ErrorID::NOT_CONNECTED_ERROR ||
					errorID == ErrorID::EOF_ERROR)
					connection->CloseSocket();
			}

			// Pop connection if socket has been closed or connection pointer isn't valid
			if (!this->inboundConnections.IsEmpty())
			{
				const ConnectionPtr& connection = this->inboundConnections.GetElementAtIndex(0); // Get element at begin of map
				if (!connection || !connection->IsSocketOpen())
					this->inboundConnections.Erase(connection->GetID());
			}
		}

		Deque<RecievedMessage>& GetInboundMessages()
		{
			return this->inboundMsgs;
		}

		const uint32_t& GetPortNumber() const
		{
			return this->port;
		}

		bool IsListening() const
		{
			return this->isListening;
		}

		const ErrorID& GetListenerErrorState() const
		{
			return this->listenerErrorState;
		}
	};

	TCPServer::TCPServer(uint32_t port, bool startListener) :
		impl(std::make_unique<Implementation>(port, startListener))
	{}

	TCPServer::~TCPServer() = default;

	void TCPServer::StartListener()
	{
		this->impl->StartListener();
	}

	void TCPServer::StopListener()
	{
		this->impl->StopListener();
	}

	ErrorID TCPServer::PushOutboundResponseMessage(const RecievedMessage& recvMsg, const Message& msgOut)
	{
		return this->impl->PushOutboundResponseMessage(recvMsg, msgOut);
	}

	void TCPServer::BroadcastOutboundMessage(const Message& msg)
	{
		this->impl->BroadcastOutboundMessage(msg);
	}

	void TCPServer::UpdateState()
	{
		this->impl->UpdateState();
	}

	Deque<RecievedMessage>& TCPServer::GetInboundMessages() 
	{ 
		return this->impl->GetInboundMessages(); 
	}

	const uint32_t& TCPServer::GetPortNumber() const 
	{ 
		return this->impl->GetPortNumber(); 
	}

	bool TCPServer::IsListening() const 
	{ 
		return this->impl->IsListening(); 
	}

	const ErrorID& TCPServer::GetListenerErrorState() const 
	{ 
		return this->impl->GetListenerErrorState(); 
	}
}