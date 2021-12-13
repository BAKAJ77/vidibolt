#include <node/node.h>
#include <string>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Node::Implementation
	{
	public:
		NodeType type;
		uint32_t port;
		uint64_t guid, nodeNetworkID;
		UnorderedMap<uint64_t, Peer> peerList; // <guid : uint64_t, address : std::string>

		TCPClient client;
		TCPServer server;
		MemPool* mempool;
		Chain* chain;
	public:
		Implementation(NodeType type, MemPool* mempool, Chain* chain, uint32_t port, uint64_t nodeNetworkID) :
			type(type), client(port), server(port), port(port), guid(0), nodeNetworkID(nodeNetworkID), mempool(mempool),
			chain(chain)
		{
			this->peerList.Reserve(64);
		}

		Implementation() = default;

		ErrorCode TransmitNodeGUIDData(const RecievedMessage& peerRequestMsg)
		{
			// Construct response message containing the node's GUID data
			Message responseMsg;
			responseMsg.header.id = MessageRepType::NODE_GUID_RESPONSE;
			responseMsg.header.networkID = this->nodeNetworkID;

			responseMsg << this->guid << peerRequestMsg.senderAddress << (int)this->type;

			// Push response message to be transmitted back to the peer who sent the request
			return this->server.PushOutboundResponseMessage(peerRequestMsg, responseMsg);
		}

		ErrorCode PushPeerNodeToList(const RecievedMessage& peerResponseMsg)
		{
			// The peers data in the response message is formatted as so :
			// [ [uint64_t] GUID, [std::string] IP-Address, [int] Node Type ]
			// Extract the data elements from the message data using the message iterator

			MessageIterator iterator = peerResponseMsg.transmittedMsg.GetIterator();

			const NodeType type = (NodeType)iterator.GetNextElementData<int>();
			const std::string address = iterator.GetNextElementData<std::string>();
			const uint64_t guid = iterator.GetNextElementData<uint64_t>();

			// Insert peer node into the peer list
			this->peerList.Insert(guid, { type, address });
			return ErrorID::NONE;
		}

		ErrorCode HandleIncomingData()
		{
			ErrorCode error;

			////////////// Handle data incoming from the client side of the node first //////////////
			while (!this->client.GetInboundMessages().IsEmpty())
			{
				const RecievedMessage& msg = this->client.GetInboundMessages().GetFrontElement();

				// Only process data from other nodes on same peer network
				if (msg.transmittedMsg.header.networkID == this->nodeNetworkID)
				{
					switch (msg.transmittedMsg.header.id)
					{
					case MessageRepType::NODE_GUID_RESPONSE:
						this->PushPeerNodeToList(msg);
						break;
					}

					if (error)
						return error;
				}

				// Pop the recieved message off the queue
				this->client.GetInboundMessages().PopFrontElement();
			}

			////////////// Now, handle data incoming from the server side of the node //////////////
			while (!this->server.GetInboundMessages().IsEmpty())
			{
				const RecievedMessage& msg = this->server.GetInboundMessages().GetFrontElement();

				// Only process data from other nodes on same peer network
				if (msg.transmittedMsg.header.networkID == this->nodeNetworkID)
				{
					switch (msg.transmittedMsg.header.id)
					{
					case MessageRepType::NODE_GUID_REQUEST:
						error = this->TransmitNodeGUIDData(msg);
						break;
					}

					if (error)
						return error;
				}

				// Pop the recieved message off the queue
				this->server.GetInboundMessages().PopFrontElement();
			}

			return ErrorID::NONE;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Node::Node(NodeType type, MemPool* mempool, Chain* chain, uint32_t port, uint64_t nodeNetworkID) :
		impl(std::make_unique<Implementation>(type, mempool, chain, port, nodeNetworkID))
	{
		assert(!(type == NodeType::FULL && !chain)); // Valid chain object must be supplied for FULL nodes
		assert(!(type == NodeType::SOLO_MINER && !mempool)); // Valid mempool object must be supplied for SOLO MINER nodes
	}

	Node::~Node() = default;

	void Node::InitNode(uint64_t guid, const UnorderedMap<uint64_t, Peer>* peerList)
	{
		this->impl->guid = guid;

		if (peerList)
			this->impl->peerList = *peerList;
	}

	ErrorCode Node::FlushNode()
	{
		// Do transmission and reading of outgoing and incoming data
		ErrorCode error = this->impl->client.UpdateState();
		if (error)
			return error;

		this->impl->server.UpdateState();
		error = this->impl->server.GetListenerErrorState();
		if (error)
			return error;

		// Handles processing of the incoming data
		return this->impl->HandleIncomingData();
	}

	TCPClient& Node::GetClient()
	{
		return this->impl->client;
	}

	TCPServer& Node::GetServer()
	{
		return this->impl->server;
	}

	const NodeType& Node::GetNodeType() const
	{
		return this->impl->type;
	}

	const uint32_t& Node::GetPort() const
	{
		return this->impl->port;
	}

	const uint64_t& Node::GetGUID() const
	{ 
		return this->impl->guid; 
	}

	const uint64_t& Node::GetNetworkID() const
	{
		return this->impl->nodeNetworkID;
	}

	const UnorderedMap<uint64_t, Peer>& Node::GetPeerList() const
	{
		return this->impl->peerList;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ErrorCode AddPeerNode(Node& node, const std::string& ipAddress)
	{
		// Next, attempt to connect to the node
		ErrorCode operationError;
		operationError = node.GetClient().Connect(ipAddress);

		if (!operationError) // Connection was successful
		{
			// Construct the GUID request message
			Message guidRequestMsg;
			guidRequestMsg.header.id = MessageRepType::NODE_GUID_REQUEST;
			guidRequestMsg.header.networkID = node.GetNetworkID();

			// Push request message into queue to be transmitted
			node.GetClient().PushOutboundMessage(guidRequestMsg);
		}

		return operationError;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}