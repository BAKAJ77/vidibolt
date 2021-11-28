#include <crypto/sha256.h>
#include <openssl/evp.h>

namespace Volt
{
	ErrorID GetSHA256Digest(const std::vector<uint8_t>& message, std::vector<uint8_t>& digestOutput)
	{
		// Make sure the message data array isn't empty
		if (message.empty())
			return ErrorID::MESSAGE_EMPTY;

		EVP_MD_CTX* digestCtx = nullptr;
		EVP_MD* digestImpl = nullptr;

		// Initialize the digest context, fetch the SHA256 implementation
		// then initialize the digest operation
		digestCtx = EVP_MD_CTX_new();
		if (!digestCtx)
			return ErrorID::CONTEXT_GENERATION_FAILURE;

		digestImpl = EVP_MD_fetch(nullptr, "SHA256", nullptr);
		if (!digestImpl)
			return ErrorID::OPERATION_IMPL_FETCH_FAILURE;

		if (EVP_DigestInit_ex(digestCtx, digestImpl, nullptr) <= 0)
			return ErrorID::OPERATION_INIT_FAILURE;

		// Give the digest operation the message data passed and get the result
		if (EVP_DigestUpdate(digestCtx, message.data(), message.size() * sizeof(uint8_t)) <= 0)
			return ErrorID::DIGEST_UPDATE_FAILURE;

		std::vector<uint8_t> digest;
		digest.resize(EVP_MD_get_size(digestImpl), '0');

		if (EVP_DigestFinal_ex(digestCtx, digest.data(), nullptr) <= 0)
			return ErrorID::DIGEST_OPERATION_FAILURE;

		// Free allocated resources
		EVP_MD_CTX_free(digestCtx);
		EVP_MD_free(digestImpl);

		digestOutput = digest;
		return ErrorID::NONE;
	}

	ErrorID GetSignedSHA256Digest(const std::vector<uint8_t>& message, const ECKeyPair& key, 
		std::vector<uint8_t>& signitureDigestOutput)
	{
		// Make sure a private key is assigned to the key pair given
		if (!key.HasPrivateKey())
			return ErrorID::ECDSA_PRIVATE_KEY_REQUIRED;

		// Make sure the message data array isn't empty
		if (message.empty())
			return ErrorID::MESSAGE_EMPTY;

		EVP_MD_CTX* digestSignCtx = nullptr;
		EVP_MD* digestImpl = nullptr;

		// Initialize the signiture digest context, fetch the SHA256 implementation
		// then initialize the signiture digest operation
		digestSignCtx = EVP_MD_CTX_new();
		if (!digestSignCtx)
			return ErrorID::CONTEXT_GENERATION_FAILURE;

		digestImpl = EVP_MD_fetch(nullptr, "SHA256", nullptr);
		if (!digestImpl)
			return ErrorID::OPERATION_IMPL_FETCH_FAILURE;

		if (EVP_DigestSignInit(digestSignCtx, nullptr, digestImpl, nullptr, key.keyPair) <= 0)
			return ErrorID::OPERATION_INIT_FAILURE;

		// Give the signiture digest operation themessage data passed and get the length of the signiture
		size_t signitureLength = 0;

		if (EVP_DigestSignUpdate(digestSignCtx, message.data(), message.size() * sizeof(uint8_t)) <= 0)
			return ErrorID::DIGEST_UPDATE_FAILURE;

		if (EVP_DigestSignFinal(digestSignCtx, nullptr, &signitureLength) <= 0)
			return ErrorID::DIGEST_OPERATION_FAILURE;

		// Get the resulting signed digest
		std::vector<uint8_t> signedDigest;
		signedDigest.resize(signitureLength, '0');

		if (EVP_DigestSignFinal(digestSignCtx, signedDigest.data(), &signitureLength) <= 0)
			return ErrorID::DIGEST_OPERATION_FAILURE;

		signedDigest.resize(signitureLength);

		// Free allocated resources
		EVP_MD_CTX_free(digestSignCtx);
		EVP_MD_free(digestImpl);

		signitureDigestOutput = signedDigest;
		return ErrorID::NONE;
	}

	ErrorID VerifySHA256Digest(const std::vector<uint8_t>& originalMessage, const ECKeyPair& key, 
		const std::vector<uint8_t>& signiture, int& signitureValid)
	{
		// Make sure a public key is assigned to the key pair given
		if (!key.HasPublicKey())
			return ErrorID::ECDSA_PUBLIC_KEY_REQUIRED;

		EVP_MD_CTX* digestSignVerifyCtx = nullptr;
		EVP_MD* digestImpl = nullptr;

		// Initialize the signiture verification digest context, fetch the SHA256 implementation
		// then initialize the signiture verification digest operation
		digestSignVerifyCtx = EVP_MD_CTX_new();
		if (!digestSignVerifyCtx)
			return ErrorID::CONTEXT_GENERATION_FAILURE;

		digestImpl = EVP_MD_fetch(nullptr, "SHA256", nullptr);
		if (!digestImpl)
			return ErrorID::OPERATION_IMPL_FETCH_FAILURE;

		if (EVP_DigestVerifyInit(digestSignVerifyCtx, nullptr, digestImpl, nullptr, key.keyPair) <= 0)
			return ErrorID::OPERATION_INIT_FAILURE;

		// Give the signiture verification digest operation themessage data passed and get the length of the signiture
		if (EVP_DigestVerifyUpdate(digestSignVerifyCtx, originalMessage.data(), originalMessage.size() * sizeof(uint8_t)) <= 0)
			return ErrorID::DIGEST_UPDATE_FAILURE;

		signitureValid = EVP_DigestVerifyFinal(digestSignVerifyCtx, signiture.data(), signiture.size());
		if (signitureValid < 0)
			return ErrorID::DIGEST_OPERATION_FAILURE;

		// Free allocated resources
		EVP_MD_CTX_free(digestSignVerifyCtx);
		EVP_MD_free(digestImpl);

		return ErrorID::NONE;
	}
}