#ifndef VIDIBOLT_NODE_REQUEST_H
#define VIDIBOLT_NODE_REQUEST_H

#include <node/node.h>
#include <crypto/ecdsa.h>
#include <util/volt_api.h>
#include <util/error_identifier.h>

namespace Volt
{
	// Send request to the peer node to return the data about it necessary to be added to the peer list.
	// An error code is returned in the event of a failure occurring.
	extern VOLT_API ErrorCode AddPeerNode(Node& node, const std::string& ipAddress);

	// Sends request to a peer FULL node to return the balance tied to the public key given.
	// The returned amount is returned through the last parameter as a future which contains a variant,
	// the expected output is of type DOUBLE.
	extern VOLT_API ErrorCode RequestAddressBalance(Node& node, const ECKeyPair& publicKey, 
		std::future<std::variant<double>>& returnVal);
}

#endif