#ifndef VIDIBOLT_NODE_H
#define VIDIBOLT_NODE_H

#include <util/volt_api.h>
#include <util/ts_unordered_map.h>
#include <net/tcp_client.h>
#include <net/tcp_server.h>
#include <core/transaction.h>
#include <core/mem_pool.h>

#include <memory>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// An enumeration that represents the type of a node
	enum class NodeType
	{
		// Will store the entire blockchain data so therefore having no limited features.
		FULL,

		// Can either store blockchain data generated forward of wallet creation, or can point to a full node for 
		// broadcasting transactions therefore avoiding having to store any blockchain data at all.
		WALLET,

		// Can either store the entire blockchain data or point to a full node on the same local area network therefore
		// avoiding having to store any blockchain data. Also will hold mempool data.
		SOLO_MINER
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// A struct which holds data about a peer node in a node's peer list
	struct Peer
	{
		NodeType type;
		std::string ipAddress;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Handles VOLT node functionality e.g. broadcasting data to other nodes, recieving data from other nodes etc.
	class Node
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	private:
		// Returns the client side TCP handler object.
		VOLT_API TCPClient& GetClient();
		
		// Returns the server side TCP handler object.
		VOLT_API TCPServer& GetServer();
	public:
		VOLT_API Node(NodeType type, MemPool* mempool, Chain* chain, uint32_t port = 60000, 
			uint64_t nodeNetworkID = VOLT_MAINNET_NETWORK_ID);
		VOLT_API Node(const Node& node) = delete;

		VOLT_API ~Node();

		VOLT_API void operator=(const Node& node) = delete;

		// Send request to the peer node to return the data about it necessary to be added to the peer list
		friend extern VOLT_API ErrorCode AddPeerNode(Node& node, const std::string& ipAddress);
		
		// Assigns the node with the GUID given and a list of direct peers.
		VOLT_API void InitNode(uint64_t guid, const UnorderedMap<uint64_t, Peer>* peerList = nullptr);

		// Should be called every now and then so all pending outbound messages are transmitted and all pending inbound
		// messages are recieved.
		VOLT_API ErrorCode FlushNode();

		// Returns the node type
		VOLT_API const NodeType& GetNodeType() const;

		// Returns the port number being used by node.
		VOLT_API const uint32_t& GetPort() const;

		// Returns the GUID of the node.
		VOLT_API const uint64_t& GetGUID() const;

		// Returns the peer network ID the node is connected to.
		VOLT_API const uint64_t& GetNetworkID() const;

		// Returns the peer list held by the node.
		VOLT_API const UnorderedMap<uint64_t, Peer>& GetPeerList() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif
