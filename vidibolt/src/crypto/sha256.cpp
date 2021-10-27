#include <crypto/sha256.h>
#include <util/log_handler.h>
#include <openssl/evp.h>

namespace VOLT
{
	VOLT_API std::vector<uint8_t> GetSHA256Digest(const std::vector<uint8_t>& message)
	{
		// Make sure the message data array isn't empty
		if (message.empty())
			LogHandler::GetHandler().PushLog("The message data array passed to SHA256 hash function must not be empty",
				LogSeverity::FATAL);

		EVP_MD_CTX* digestCtx = nullptr;
		EVP_MD* digestImpl = nullptr;

		// Initialize the digest context, fetch the SHA256 implementation
		// then initialize the digest operation
		digestCtx = VOLT_OPENSSL_CHECK(EVP_MD_CTX_new());
		digestImpl = VOLT_OPENSSL_CHECK(EVP_MD_fetch(nullptr, "SHA256", nullptr));
		VOLT_OPENSSL_CHECK(EVP_DigestInit_ex(digestCtx, digestImpl, nullptr));

		// Give the digest operation the message data passed and get the result
		VOLT_OPENSSL_CHECK(EVP_DigestUpdate(digestCtx, message.data(), message.size() * sizeof(uint8_t)));

		std::vector<uint8_t> digest;
		digest.resize(EVP_MD_get_size(digestImpl), '0');

		VOLT_OPENSSL_CHECK(EVP_DigestFinal_ex(digestCtx, digest.data(), nullptr));

		// Free allocated resources
		EVP_MD_CTX_free(digestCtx);
		EVP_MD_free(digestImpl);

		return digest;
	}

	std::vector<uint8_t> GetSignedSHA256Digest(const std::vector<uint8_t>& message, const ECKeyPair& key)
	{
		// Make sure a private key is assigned to the key pair given
		if (!key.HasPrivateKey())
			LogHandler::GetHandler().PushLog("EC key pair object given is missing a private key", LogSeverity::FATAL);

		EVP_MD_CTX* digestSignCtx = nullptr;
		EVP_MD* digestImpl = nullptr;

		// Initialize the signiture digest context, fetch the SHA256 implementation
		// then initialize the signiture digest operation
		digestSignCtx = VOLT_OPENSSL_CHECK(EVP_MD_CTX_new());
		digestImpl = VOLT_OPENSSL_CHECK(EVP_MD_fetch(nullptr, "SHA256", nullptr));
		VOLT_OPENSSL_CHECK(EVP_DigestSignInit(digestSignCtx, nullptr, digestImpl, nullptr, key.keyPair));

		// Give the signiture digest operation themessage data passed and get the length of the signiture
		size_t signitureLength = 0;

		VOLT_OPENSSL_CHECK(EVP_DigestSignUpdate(digestSignCtx, message.data(), message.size() * sizeof(uint8_t)));
		VOLT_OPENSSL_CHECK(EVP_DigestSignFinal(digestSignCtx, nullptr, &signitureLength));

		// Get the resulting signed digest
		std::vector<uint8_t> signedDigest;
		signedDigest.resize(signitureLength, '0');

		VOLT_OPENSSL_CHECK(EVP_DigestSignFinal(digestSignCtx, signedDigest.data(), &signitureLength));
		signedDigest.resize(signitureLength);

		// Free allocated resources
		EVP_MD_CTX_free(digestSignCtx);
		EVP_MD_free(digestImpl);

		return signedDigest;
	}

	int VerifySHA256Digest(const std::vector<uint8_t>& originalMessage, const std::vector<uint8_t>& signiture, 
		const ECKeyPair& key)
	{
		// Make sure a public key is assigned to the key pair given
		if (!key.HasPublicKey())
			LogHandler::GetHandler().PushLog("EC key pair object given is missing a public key", LogSeverity::FATAL);

		EVP_MD_CTX* digestSignVerifyCtx = nullptr;
		EVP_MD* digestImpl = nullptr;

		// Initialize the signiture verification digest context, fetch the SHA256 implementation
		// then initialize the signiture verification digest operation
		digestSignVerifyCtx = VOLT_OPENSSL_CHECK(EVP_MD_CTX_new());
		digestImpl = VOLT_OPENSSL_CHECK(EVP_MD_fetch(nullptr, "SHA256", nullptr));
		VOLT_OPENSSL_CHECK(EVP_DigestVerifyInit(digestSignVerifyCtx, nullptr, digestImpl, nullptr, key.keyPair));

		// Give the signiture verification digest operation themessage data passed and get the length of the signiture
		size_t signitureLength = 0;

		VOLT_OPENSSL_CHECK(EVP_DigestVerifyUpdate(digestSignVerifyCtx, originalMessage.data(), 
			originalMessage.size() * sizeof(uint8_t)));
		int validSigniture = VOLT_OPENSSL_CHECK(EVP_DigestVerifyFinal(digestSignVerifyCtx, signiture.data(), signiture.size()));

		// Free allocated resources
		EVP_MD_CTX_free(digestSignVerifyCtx);
		EVP_MD_free(digestImpl);

		return validSigniture;
	}
}