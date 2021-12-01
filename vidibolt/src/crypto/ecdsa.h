#ifndef VIDIBOLT_ECDSA_H
#define VIDIBOLT_ECDSA_H

#include <util/volt_api.h>
#include <util/error_identifier.h>
#include <vector>
#include <string>

typedef struct evp_pkey_st EVP_PKEY;
typedef struct evp_pkey_ctx_st EVP_PKEY_CTX;

namespace Volt
{
	class VOLT_API ECKeyPair
	{
		friend VOLT_API ErrorCode GetSignedSHA256Digest(const std::vector<uint8_t>& message, const ECKeyPair& key,
			std::vector<uint8_t>& signitureDigestOutput);
		friend VOLT_API ErrorCode VerifySHA256Digest(const std::vector<uint8_t>& originalMessage, const ECKeyPair& key,
			const std::vector<uint8_t>& signiture);
	private:
		EVP_PKEY_CTX* keyGenCtx;
		EVP_PKEY* keyPair;
	public:
		/*
			Call this constructor if you want a public and private key to be generated automatically.
		*/
		ECKeyPair(ErrorCode* error = nullptr);

		/*
			Call this constructor if you want to set the public and private key manually.
			NOTE: A public key must be given, though you don't need to pass a private key.
		*/
		ECKeyPair(const std::string& publicKey, const std::string& privateKey = "", ErrorCode* error = nullptr);
		
		ECKeyPair(const ECKeyPair&) = delete;
		~ECKeyPair();

		/*
			Returns a string containing the public key in hexadecimal format.
		*/
		std::string GetPublicKeyHex() const;

		/*
			Returns a string containing the private key in hexadecimal format.
		*/
		std::string GetPrivateKeyHex() const;

		/*
			Returns whether the key pair holds a public key.
		*/
		bool HasPublicKey() const;

		/*
			Returns whether the key pair holds a private key.
		*/
		bool HasPrivateKey() const;

		/*
			Returns TRUE if the keypair is valid, else FALSE is returned.
		*/
		bool IsValid() const;
	};
}

#endif