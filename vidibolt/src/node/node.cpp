#include <node/node.h>

#include <unordered_map>
#include <string>

namespace Volt
{
	class Node::Implementation
	{
	private:
		uint64_t guid;
		TCPClient client;
		TCPServer server;

		uint32_t port;
		std::unordered_map<uint64_t, std::string> peerList; // <guid : uint64_t, address : std::string>
	private:
		/*
			Retrieves a unoccupied GUID to be assigned to the node.
		*/
		uint64_t RetrieveNodeGUID()
		{
			return 0; // TEMPORARY
		}

		/*
			Retrieves unordered map table of peers for connection routing.
		*/
		void RetrieveNodePeerList()
		{
			// We only need 64 nodes to be able to connect to any node in the network
			this->peerList.reserve(64);
		}
	public:
		Implementation(uint32_t port) :
			client(port), server(port), port(port)
		{
			this->guid = this->RetrieveNodeGUID();
		}

		Implementation() = default;

		TCPClient& GetClient()
		{
			return this->client;
		}

		TCPServer& GetServer()
		{
			return this->server;
		}

		const uint32_t& GetPort() const
		{
			return this->port;
		}

		const uint64_t& GetGUID() const
		{
			return this->guid;
		}
	};

	Node::Node(uint32_t port) :
		impl(new Implementation(port))
	{}

	Node::~Node() 
	{
		delete this->impl;
	}

	TCPClient& Node::GetClient()
	{
		return this->impl->GetClient();
	}

	TCPServer& Node::GetServer()
	{
		return this->impl->GetServer();
	}

	const uint32_t& Node::GetPort() const
	{
		return this->impl->GetPort();
	}

	const uint64_t& Node::GetGUID() const
	{ 
		return this->impl->GetGUID(); 
	}
}