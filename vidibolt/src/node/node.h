#ifndef VIDIBOLT_NODE_H
#define VIDIBOLT_NODE_H

#include <util/volt_api.h>
#include <net/tcp_client.h>
#include <net/tcp_server.h>

namespace Volt
{
	/*
		Handles VOLT node functionality e.g. broadcasting data to other nodes, recieving data from other nodes etc.
	*/
	class VOLT_API Node
	{
	private:
		class Implementation;
		Implementation* impl;
	public:
		Node(uint32_t port = 60000);
		~Node();

		/* TEMPORARY FUNCTIONS */
		TCPClient& GetClient();
		TCPServer& GetServer();
		/***********************/

		/*
			Returns the port number being used by node.
		*/
		const uint32_t& GetPort() const;

		/*
			Returns the GUID of the node.
		*/
		const uint64_t& GetGUID() const;
	};
}

#endif
