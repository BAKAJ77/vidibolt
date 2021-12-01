#ifndef VIDIBOLT_NODE_H
#define VIDIBOLT_NODE_H

#include <util/volt_api.h>
#include <net/tcp_client.h>
#include <net/tcp_server.h>

#include <memory>

namespace Volt
{
	// Handles VOLT node functionality e.g. broadcasting data to other nodes, recieving data from other nodes etc.
	class Node
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	public:
		VOLT_API Node(uint32_t port = 60000);
		VOLT_API Node(const Node& node) = delete;

		VOLT_API ~Node();

		VOLT_API void operator=(const Node& node) = delete;

		///// TEMPORARY FUNCTIONS /////
		VOLT_API TCPClient& GetClient();
		VOLT_API TCPServer& GetServer();
		///////////////////////////////

		// Returns the port number being used by node.
		VOLT_API const uint32_t& GetPort() const;

		// Returns the GUID of the node.
		VOLT_API const uint64_t& GetGUID() const;
	};
}

#endif
