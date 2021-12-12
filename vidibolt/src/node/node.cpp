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
		UnorderedMap<uint64_t, std::string> peerList; // <guid : uint64_t, address : std::string>

		TCPClient client;
		TCPServer server;
	public:
		Implementation(NodeType type, uint32_t port, uint64_t nodeNetworkID) :
			type(type), client(port), server(port), port(port), guid(0), nodeNetworkID(nodeNetworkID)
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

			responseMsg << this->guid << peerRequestMsg.senderAddress;

			// Push response message to be transmitted back to the peer who sent the request
			return this->server.PushOutboundResponseMessage(peerRequestMsg, responseMsg);
		}

		void PushPeerNodeToList(const RecievedMessage& peerResponseMsg)
		{
			// The peers data in the response message is formatted as so: 
			// [ [uint64_t] GUID, [std::string] IP-Address, [int] String Length ]
			// All transmitted messages are constructed like stack containers and therefore store payload data contiguosly
			// Therefore we can easily copy the payload data we recieved into their respective data type containers
			uint64_t peerGUID;
			std::string peerAddress;
			peerAddress.resize(peerResponseMsg.transmittedMsg.header.sizeBytes - sizeof(uint64_t) - sizeof(int));

			memcpy(&peerGUID, peerResponseMsg.transmittedMsg.payload.data(), sizeof(uint64_t));
			memcpy(peerAddress.data(), peerResponseMsg.transmittedMsg.payload.data() + sizeof(uint64_t),
				peerAddress.size());

			this->peerList.Insert(peerGUID, peerAddress);
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
				}

				// Pop the recieved message off the queue
				this->server.GetInboundMessages().PopFrontElement();
			}

			return error;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Node::Node(NodeType type, uint32_t port, uint64_t nodeNetworkID) :
		impl(std::make_unique<Implementation>(type, port, nodeNetworkID))
	{}

	Node::~Node() = default;

	void Node::InitNode(uint64_t guid, const UnorderedMap<uint64_t, std::string>* peerList)
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

	const UnorderedMap<uint64_t, std::string>& Node::GetPeerList() const
	{
		return this->impl->peerList;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ErrorCode AddPeerNode(Node& node, const std::string& ipAddress)
	{
		// Firstly, make sure the peer node isn't already in the node's peer list
		for (uint32_t index = 0; index < node.GetPeerList().GetSize(); index++)
		{
			if (node.GetPeerList().GetElementAtIndex(index) == ipAddress)
				return ErrorID::PEER_NODE_ALREADY_IN_PEER_LIST;
		}

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