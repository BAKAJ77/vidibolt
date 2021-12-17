#include <node/request.h>
#include <util/random_generation.h>

#include <mutex>

namespace
{
	static std::mutex operationMutex;
}

namespace Volt
{
	ErrorCode AddPeerNode(Node& node, const std::string& ipAddress)
	{
		std::scoped_lock lock(operationMutex);

		// Next, attempt to connect to the node
		ErrorCode operationError;
		operationError = node.GetClient().Connect(ipAddress);

		if (!operationError) // Connection was successful
		{
			// Construct the GUID request message
			Message guidRequestMsg;
			guidRequestMsg.header.id = MessageRepType::NODE_GUID_REQUEST;
			guidRequestMsg.header.networkID = node.GetNetworkID();

			// Transmit the request message
			node.GetClient().PushOutboundMessage(guidRequestMsg);
			node.GetClient().TransmitOutboundMessages();
		}

		return operationError;
	}

	ErrorCode RequestAddressBalance(Node& node, const ECKeyPair& publicKey, std::future<std::variant<double>>& returnVal)
	{
		std::scoped_lock lock(operationMutex);

		const uint32_t requestID = node.GenerateRequestID();
		const Peer* fullNodePeer = nullptr;
		ErrorCode operationError;

		// Firstly, check if the node's peer list for a full peer node or solo miner peer node which stores full blockchain data
		for (uint32_t index = 0; index < node.GetPeerList().GetSize(); index++)
		{
			const Peer& peer = node.GetPeerList()[index];
			if (peer.type == NodeType::FULL || (peer.type == NodeType::SOLO_MINER && peer.storesFullBlockchain))
			{
				fullNodePeer = &peer;
				break;
			}
		}

		if (fullNodePeer) // A suitable peer node has been found in the peer list
		{
			// Attempt to connect to the full node peer
			operationError = node.AttemptPeerConnect(fullNodePeer->ipAddress);
		}
		else
			return ErrorID::NO_SUITABLE_NODE_IN_NODE_PEER_LIST;

		if (!operationError)
		{
			// Push pending request return promise
			Node::PendingOperationReturn requestReturnPromise = { requestID, std::promise<std::variant<double>>() };

			returnVal = requestReturnPromise.second.get_future();
			node.GetPendingReturnsFromOperations().EmplaceBackElement(std::move(requestReturnPromise));

			// Construct request message
			Message requestMsg;
			requestMsg.header.id = MessageRepType::ADDRESS_AMOUNT_REQUEST;
			requestMsg.header.networkID = node.GetNetworkID();

			requestMsg << requestID << publicKey.GetPublicKeyHex();

			// Push request message into queue to be transmitted
			node.GetClient().PushOutboundMessage(requestMsg);
			node.GetClient().TransmitOutboundMessages();
		}

		return operationError;
	}
}