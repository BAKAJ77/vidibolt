#include <crypto/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <algorithm>

namespace Volt
{
	ECKeyPair::ECKeyPair(ErrorID* error) :
		keyPair(nullptr), keyGenCtx(nullptr)
	{
		// Create the key generation context and initialize it
		this->keyGenCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
		if (!this->keyGenCtx)
		{
			if (error) 
				*error = ErrorID::CONTEXT_GENERATION_FAILURE;
		}
		else
		{
			if (EVP_PKEY_keygen_init(this->keyGenCtx) <= 0)
			{
				if (error) 
					*error = ErrorID::OPERATION_INIT_FAILURE;
			}

			// Set the parameter generation curve and point compresson form enum, then generate the key pair
			if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(this->keyGenCtx, NID_secp256k1) <= 0)
			{
				if (error) 
					*error = ErrorID::EC_PARAMGEN_CURVE_ERROR;
			}

			if (EVP_PKEY_generate(this->keyGenCtx, &this->keyPair) <= 0)
			{
				if (error) 
					*error = ErrorID::ECDSA_KEY_GENERATION_ERROR;
			}
			else
			{
				EC_KEY* ecKeyPair = EVP_PKEY_get1_EC_KEY(this->keyPair);
				EC_KEY_set_conv_form(ecKeyPair, POINT_CONVERSION_COMPRESSED);
			}
		}
	}

	ECKeyPair::ECKeyPair(const std::string& publicKey, const std::string& privateKey, ErrorID* error) :
		keyPair(nullptr), keyGenCtx(nullptr)
	{
		// Make sure the public key given was valid 
		// Note that while a private key must be given, the private key isn't necessary
		if (publicKey.substr(0, 4) != "vpk_")
		{
			if (error) 
				*error = ErrorID::ECDSA_PUBLIC_KEY_INVALID;
		}
		else
		{
			// Transform all the characters in the public key hex string to UPPERCASE
			std::string pubKeyStr = publicKey, privKeyStr = privateKey;

			pubKeyStr.erase(pubKeyStr.begin(), pubKeyStr.begin() + 4); // also remove "vpk_" prefix in public key string
			std::transform(pubKeyStr.begin(), pubKeyStr.end(), pubKeyStr.begin(), ::toupper);

			// Transform all the characters in the private key hex string to UPPERCASE (if a private key was given)
			if (!privateKey.empty())
				std::transform(privKeyStr.begin(), privKeyStr.end(), privKeyStr.begin(), ::toupper);

			// Create and setup the keypair with the given public key (and private key if one was given)
			this->keyPair = EVP_PKEY_new();

			EC_GROUP* group = EC_GROUP_new_by_curve_name_ex(nullptr, nullptr, NID_secp256k1);
			EC_GROUP_set_point_conversion_form(group, POINT_CONVERSION_COMPRESSED);
			EC_POINT* pubKey = EC_POINT_hex2point(group, pubKeyStr.c_str(), nullptr, nullptr);

			EC_KEY* ecKeyPair = EC_KEY_new();
			if (EC_KEY_set_group(ecKeyPair, group) <= 0)
			{
				if (error) 
					*error = ErrorID::EC_KEY_GROUP_ASSIGNMENT_FAILURE;
			}

			if (EC_KEY_set_public_key(ecKeyPair, pubKey) <= 0)
			{
				if (error) 
					*error = ErrorID::EC_KEY_ASSIGNMENT_FAILURE;
			}

			if (!privateKey.empty())
			{
				BIGNUM* privKey = nullptr;

				BN_hex2bn(&privKey, privKeyStr.c_str());
				if (EC_KEY_set_private_key(ecKeyPair, privKey) <= 0)
				{
					if (error) 
						*error = ErrorID::EC_KEY_ASSIGNMENT_FAILURE;
				}
			}

			EVP_PKEY_set1_EC_KEY(this->keyPair, ecKeyPair);
		}
	}

	ECKeyPair::~ECKeyPair()
	{
		EVP_PKEY_CTX_free(this->keyGenCtx);
		EVP_PKEY_free(this->keyPair);
	}

	std::string ECKeyPair::GetPublicKeyHex() const
	{
		// Get the public key (aka EC_POINT*)
		const EC_KEY* EC_KEY_PAIR = EVP_PKEY_get0_EC_KEY(this->keyPair);
		const EC_POINT* PUBLIC_KEY = EC_KEY_get0_public_key(EC_KEY_PAIR);

		// Get data required for conversion to HEX
		const EC_GROUP* GROUP = EC_KEY_get0_group(EC_KEY_PAIR);
		const point_conversion_form_t POINT_CONVERSION = EC_GROUP_get_point_conversion_form(GROUP);

		if (PUBLIC_KEY)
		{
			// Convert the EC_POINT* public key to a hexadecimal format
			std::string publicKeyHex = EC_POINT_point2hex(GROUP, PUBLIC_KEY, POINT_CONVERSION, nullptr);
			std::transform(publicKeyHex.begin(), publicKeyHex.end(), publicKeyHex.begin(), ::tolower);

			return  "vpk_" + publicKeyHex;
		}

		return std::string();
	}

	std::string ECKeyPair::GetPrivateKeyHex() const
	{
		// Get the public key (aka EC_POINT*)
		const EC_KEY* EC_KEY_PAIR = EVP_PKEY_get0_EC_KEY(this->keyPair);
		const BIGNUM* PRIVATE_KEY = EC_KEY_get0_private_key(EC_KEY_PAIR);

		if (PRIVATE_KEY)
		{
			// Convert the BIGNUM* private key to a hexadecimal format
			std::string privateKeyHex = BN_bn2hex(PRIVATE_KEY);
			std::transform(privateKeyHex.begin(), privateKeyHex.end(), privateKeyHex.begin(), ::tolower);

			return privateKeyHex;
		}

		return std::string();
	}

	bool ECKeyPair::HasPublicKey() const
	{
		const EC_KEY* EC_KEY_PAIR = EVP_PKEY_get0_EC_KEY(this->keyPair);
		const EC_POINT* PUBLIC_KEY = EC_KEY_get0_public_key(EC_KEY_PAIR);

		return PUBLIC_KEY ? true : false;
	}

	bool ECKeyPair::HasPrivateKey() const
	{
		const EC_KEY* EC_KEY_PAIR = EVP_PKEY_get0_EC_KEY(this->keyPair);
		const BIGNUM* PRIVATE_KEY = EC_KEY_get0_private_key(EC_KEY_PAIR);

		return PRIVATE_KEY ? true : false;
	}

	bool ECKeyPair::IsValid() const { return EC_KEY_check_key(EVP_PKEY_get0_EC_KEY(this->keyPair)); }
}