#ifndef VIDIBOLT_SHA256_H
#define VIDIBOLT_SHA256_H

#include <util/data_conversion.h>
#include <util/volt_api.h>
#include <crypto/ecdsa.h>

namespace Volt
{
	/*
		Returns resulting hash byte data from the SHA256 digest operation.
	*/
	extern VOLT_API std::vector<uint8_t> GetSHA256Digest(const std::vector<uint8_t>& message);

	/*
		Returns resulting signiture byte data from the SHA256 digest signing operation.
	*/
	extern VOLT_API std::vector<uint8_t> GetSignedSHA256Digest(const std::vector<uint8_t>& message, const ECKeyPair& key);

	/*
		Returns 1 if the signiture is valid, else if it't not then 0 is returned.
	*/
	extern VOLT_API int VerifySHA256Digest(const std::vector<uint8_t>& originalMessage, const std::vector<uint8_t>& signiture, 
		const ECKeyPair& key);
}

#endif
