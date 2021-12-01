#ifndef VIDIBOLT_SHA256_H
#define VIDIBOLT_SHA256_H

#include <util/data_conversion.h>
#include <util/error_identifier.h>
#include <util/volt_api.h>
#include <crypto/ecdsa.h>
#include <vector>

namespace Volt
{
	/*
		Returns resulting hash byte data from the SHA256 digest operation.
	*/
	extern VOLT_API ErrorCode GetSHA256Digest(const std::vector<uint8_t>& message, std::vector<uint8_t>& digestOutput);

	/*
		Returns resulting signiture byte data from the SHA256 digest signing operation.
	*/
	extern VOLT_API ErrorCode GetSignedSHA256Digest(const std::vector<uint8_t>& message, const ECKeyPair& key,
		std::vector<uint8_t>& signitureDigestOutput);

	/*
		Returns an error code with a value of 'ErrorID::NONE' if the signiture was valid, else other
		possible error codes are returned on failure.
	*/
	extern VOLT_API ErrorCode VerifySHA256Digest(const std::vector<uint8_t>& originalMessage, const ECKeyPair& key,
		const std::vector<uint8_t>& signiture);
}

#endif
