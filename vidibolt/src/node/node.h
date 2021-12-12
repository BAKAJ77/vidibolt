#ifndef VIDIBOLT_NODE_H
#define VIDIBOLT_NODE_H

#include <util/volt_api.h>
#include <util/ts_unordered_map.h>
#include <net/tcp_client.h>
#include <net/tcp_server.h>

#include <memory>

namespace Volt
{
	// An enumeration that represents the type of a node
	enum class NodeType
	{
		REGULAR,
		STREAM_STORAGE,
		MINER
	};

	// Handles VOLT node functionality e.g. broadcasting data to other nodes, recieving data from other nodes etc.
	class Node
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	private:

	private:
		// Returns the client side TCP handler object.
		VOLT_API TCPClient& GetClient();
		
		// Returns the server side TCP handler object.
		VOLT_API TCPServer& GetServer();
	public:
		VOLT_API Node(NodeType type, uint32_t port = 60000, uint64_t nodeNetworkID = VOLT_MAINNET_NETWORK_ID);
		VOLT_API Node(const Node& node) = delete;

		VOLT_API ~Node();

		VOLT_API void operator=(const Node& node) = delete;

		// Send request to the peer node to return the data about it necessary to be added to the peer list
		friend extern VOLT_API ErrorCode AddPeerNode(Node& node, const std::string& ipAddress);
		
		// Assigns the node with the GUID given and a list of direct peers.
		VOLT_API void InitNode(uint64_t guid, const UnorderedMap<uint64_t, std::string>* peerList = nullptr);

		// Should be called every now and then so all pending outbound messages are transmitted and all pending inbound
		// messages are recieved.
		VOLT_API ErrorCode FlushNode();

		///// TEMPORARY FUNCTIONS /////
		VOLT_API TCPClient& GetClient();
		VOLT_API TCPServer& GetServer();
		///////////////////////////////

		// Returns the node type
		VOLT_API const NodeType& GetNodeType() const;

		// Returns the port number being used by node.
		VOLT_API const uint32_t& GetPort() const;

		// Returns the GUID of the node.
		VOLT_API const uint64_t& GetGUID() const;

		// Returns the peer network ID the node is connected to.
		VOLT_API const uint64_t& GetNetworkID() const;

		// Returns the peer list held by the node.
		VOLT_API const UnorderedMap<uint64_t, std::string>& GetPeerList() const;
	};
}

#endif
