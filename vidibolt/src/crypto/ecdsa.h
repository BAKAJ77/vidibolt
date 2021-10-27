#ifndef ECDSA_H
#define ECDSA_H

#include <util/volt_api.h>
#include <vector>
#include <string>

typedef struct evp_pkey_st EVP_PKEY;
typedef struct evp_pkey_ctx_st EVP_PKEY_CTX;

namespace VOLT
{
	class VOLT_API ECKeyPair
	{
		friend VOLT_API std::vector<uint8_t> GetSignedSHA256Digest(const std::vector<uint8_t>& message, const ECKeyPair& key);
		friend VOLT_API int VerifySHA256Digest(const std::vector<uint8_t>& originalMessage, 
			const std::vector<uint8_t>& signiture, const ECKeyPair& key);
	private:
		EVP_PKEY_CTX* keyGenCtx;
		EVP_PKEY* keyPair;
	public:
		/*
			Call this constructor if you want a public and private key to be generated automatically.
		*/
		ECKeyPair();

		/*
			Call this constructor if you want to set the public and private key manually.
			NOTE: A public key must be given, though you don't need to pass a private key.
		*/
		ECKeyPair(const std::string& publicKey, const std::string& privateKey = "");
		
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