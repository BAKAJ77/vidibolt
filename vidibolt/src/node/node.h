#ifndef VIDIBOLT_NODE_H
#define VIDIBOLT_NODE_H

#include <net/tcp_client.h>
#include <net/tcp_server.h>
#include <util/volt_api.h>
#include <unordered_map>

namespace Volt
{
	class VOLT_API Node
	{
	private:
		uint64_t guid;
	public:
		TCPClient client;
		TCPServer server;
		std::unordered_map<uint64_t, std::string> peerList; // <guid : uint64_t, address : std::string>
	private:
		/*
			Retrieves a unoccupied GUID to be assigned to the node.
		*/
		uint64_t RetrieveNodeGUID();

		/*
			Retrieves unordered map table of peers for connection routing.
		*/
		void RetrieveNodePeerList();
	public:
		Node(uint32_t port);
		~Node();

		/*
			Returns the GUID of the node.
		*/
		const uint64_t& GetGUID() const;
	};
}

#endif
