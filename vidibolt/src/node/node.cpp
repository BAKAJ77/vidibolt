#include <node/node.h>
#include <util/random_generation.h>

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
		Vector<Peer> peerList; // <guid : uint64_t, address : std::string>
		Vector<PendingOperationReturn> pendingRequestReturns;
		
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

		// Attemps to connect to peer node. Specified number of attempts are made on failures to connect before giving up and 
		// returning an error.
		ErrorCode AttemptPeerConnect(const std::string& ipAddress, size_t maxAttempts = 3)
		{
			ErrorCode operationError;
			size_t attempts = 0;

			while (attempts < maxAttempts)
			{
				operationError = this->client.Connect(ipAddress);
				if (!operationError)
					break;
				else
					attempts++;
			}

			return operationError;
		}

		ErrorCode ProcessAddressBalanceResponse(const RecievedMessage& peerResponseMsg)
		{
			// The recieved response message should be formatted as so: [ [uint32_t] Request ID, [double] Balance ]
			// Extract the elements from the response message
			MessageIterator iterator = peerResponseMsg.transmittedMsg.GetBegin();

			const double returnedBalance = iterator.GetNextElementData<double>();
			const uint32_t requestID = iterator.GetNextElementData<uint32_t>();

			if (returnedBalance == -1) // If the returned balance value is -1 then an error occurred on the peer node's side
				return ErrorID::BALANCE_REQUEST_PEER_SIDE_ERROR;

			// No errors occurred so assign the returned balance to the correct PendingRequestReturn promise object
			for (uint32_t index = 0; index < this->pendingRequestReturns.GetSize(); index++)
			{
				Node::PendingOperationReturn& returnPromise = this->pendingRequestReturns[index];
				if (requestID == returnPromise.first)
				{
					returnPromise.second.set_value(returnedBalance);
					this->pendingRequestReturns.EraseElementAtIndex(index);
				}
			}

			return ErrorID::NONE;
		}

		ErrorCode TransmitAddressBalance(const RecievedMessage& peerRequestMsg)
		{
			ErrorCode operationError;
			
			// The recieved request message should be formatted as so: [ [uint32_t] Request ID, [std::string] Public Key Address ]
			// Extract the elements from the request message
			MessageIterator iterator = peerRequestMsg.transmittedMsg.GetBegin();

			const std::string publicKeyAddress = iterator.GetNextElementData<std::string>();
			const uint32_t requestID = iterator.GetNextElementData<uint32_t>();

			// Get the balance tied to the public key addresss
			const ECKeyPair publicKey(publicKeyAddress, std::string(), &operationError);
			double retrievedBalance = -1;

			if (!operationError)
				retrievedBalance = this->chain->GetAddressBalance(publicKey);

			// Initialize the outbound message to be transmitted
			Message outboundMsg;
			outboundMsg.header.id = MessageRepType::ADDRESS_AMOUNT_RESPONSE;
			outboundMsg.header.networkID = this->nodeNetworkID;

			outboundMsg << requestID << retrievedBalance;

			// Send the response to the originating node of the request	
			operationError = this->server.PushOutboundResponseMessage(peerRequestMsg, outboundMsg);

			return operationError;
		}

		ErrorCode PushPeerNodeToList(const RecievedMessage& peerResponseMsg)
		{
			// The peers data in the response message is formatted as so :
			// [ [bool] Stores Full Blockchain, [uint64_t] GUID, [int] Node Type ]
			// Extract the data elements from the message data using the message iterator

			MessageIterator iterator = peerResponseMsg.transmittedMsg.GetBegin();

			const NodeType type = (NodeType)iterator.GetNextElementData<int>();
			const uint64_t guid = iterator.GetNextElementData<uint64_t>();
			const bool storesFullBlockchain = (bool)iterator.GetNextElementData<int>();

			// Insert peer node into the peer list
			this->peerList.PushBackElement({ type, guid, peerResponseMsg.senderAddress, storesFullBlockchain });

			// Close the connection with the peer node
			this->client.Disconnect();

			return ErrorID::NONE;
		}

		ErrorCode TransmitNodeGUIDData(const RecievedMessage& peerRequestMsg)
		{
			// Construct response message containing the node's GUID data
			Message responseMsg;
			responseMsg.header.id = MessageRepType::NODE_GUID_RESPONSE;
			responseMsg.header.networkID = this->nodeNetworkID;

			responseMsg << (int)(this->chain != nullptr) << this->guid << (int)this->type;

			return this->server.PushOutboundResponseMessage(peerRequestMsg, responseMsg);
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Vector<Node::PendingOperationReturn>& Node::GetPendingReturnsFromOperations()
	{
		return this->impl->pendingRequestReturns;
	}

	Node::Node(NodeType type, MemPool* mempool, Chain* chain, uint32_t port, uint64_t nodeNetworkID) :
		impl(std::make_unique<Implementation>(type, mempool, chain, port, nodeNetworkID))
	{
		assert(!(type == NodeType::FULL && !chain)); // Valid chain object must be supplied for FULL nodes
		assert(!(type == NodeType::SOLO_MINER && !mempool)); // Valid mempool object must be supplied for SOLO MINER nodes
	}

	Node::~Node() = default;

	void Node::InitNode(uint64_t guid, const Vector<Peer>* peerList)
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
		return this->HandleIOData();
	}

	ErrorCode Node::AttemptPeerConnect(const std::string& ipAddress, size_t maxAttempts)
	{
		return this->impl->AttemptPeerConnect(ipAddress, maxAttempts);
	}

	uint32_t Node::GenerateRequestID() const
	{
		uint32_t generatedID = 0;
		while (true)
		{
			// Generate a new random number ID
			generatedID = Volt::GenerateRandomUint32(0, UINT32_MAX);
			
			// Check if the generated random number ID is not taken yet
			bool generatedIDUnoccupied = true;
			for (uint32_t index = 0; index < this->impl->pendingRequestReturns.GetSize(); index++)
			{
				if (generatedID == this->impl->pendingRequestReturns[index].first)
				{
					generatedIDUnoccupied = false;
					break;
				}
			}

			// If the generated ID is not taken then break out of ID generation while loop
			if (generatedIDUnoccupied)
				break;
		}

		return generatedID;
	}

	ErrorCode Node::HandleIOData()
	{
		ErrorCode error;

		////////////// Handle data incoming from the client side of the node first //////////////
		while (!this->impl->client.GetInboundMessages().IsEmpty())
		{
			const RecievedMessage& msg = this->impl->client.GetInboundMessages().GetFrontElement();

			// Only process data from other nodes on same peer network
			if (msg.transmittedMsg.header.networkID == this->impl->nodeNetworkID)
			{
				switch (msg.transmittedMsg.header.id)
				{
				case MessageRepType::NODE_GUID_RESPONSE:
					error = this->impl->PushPeerNodeToList(msg);
					break;
				case MessageRepType::ADDRESS_AMOUNT_RESPONSE:
					error = this->impl->ProcessAddressBalanceResponse(msg);
					break;
				}

				if (error)
					return error;
			}

			// Pop the recieved message off the queue
			this->impl->client.GetInboundMessages().PopFrontElement();
		}

		////////////// Now, handle data incoming from the server side of the node //////////////
		while (!this->impl->server.GetInboundMessages().IsEmpty())
		{
			const RecievedMessage& msg = this->impl->server.GetInboundMessages().GetFrontElement();

			// Only process data from other nodes on same peer network
			if (msg.transmittedMsg.header.networkID == this->impl->nodeNetworkID)
			{
				switch (msg.transmittedMsg.header.id)
				{
				case MessageRepType::NODE_GUID_REQUEST:
					error = this->impl->TransmitNodeGUIDData(msg);
					break;
				case MessageRepType::ADDRESS_AMOUNT_REQUEST:
					error = this->impl->TransmitAddressBalance(msg);
					break;
				}

				if (error)
					return error;
			}

			// Pop the recieved message off the queue
			this->impl->server.GetInboundMessages().PopFrontElement();
		}

		return ErrorID::NONE;
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

	const Vector<Peer>& Node::GetPeerList() const
	{
		return this->impl->peerList;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}