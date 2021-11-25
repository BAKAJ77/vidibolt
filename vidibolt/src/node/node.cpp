#include <node/node.h>

namespace Volt
{
	Node::Node(uint32_t port) :
		client(port), server(port)
	{
		this->guid = this->RetrieveNodeGUID();
	}

	Node::~Node() {}

	uint64_t Node::RetrieveNodeGUID()
	{
		return 0; // TEMPORARY
	}

	void Node::RetrieveNodePeerList()
	{
		// We only need 64 nodes to be able to connect to any node in the network
		this->peerList.reserve(64);
	}

	const uint64_t& Node::GetGUID() const { return this->guid; }
}