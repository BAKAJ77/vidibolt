#include <util/data_conversion.h>
#include <crypto/sha256.h>
#include <crypto/ecdsa.h>
#include <iostream>

int main(int argc, char** argv)
{
	// Generate the EC key
	VOLT::ECKeyPair keyPair("0368bb1821f006fd796b4765a91ff9cbcc0395927a175f0a493c5e23e488aefc38", 
		"6e3a8c4a4fbca18dfe35d9cc1ad257b50df647aa0334357c3a56d5241ee474d5");

	std::cout << keyPair.GetPublicKeyHex() << std::endl;
	std::cout << keyPair.GetPrivateKeyHex() << std::endl;
	std::cout << keyPair.IsValid() << std::endl;

	std::cin.get();

	for (int i = 0; i < 10; i++)
	{
		std::string x = "Hello", y = "World", z = "Wow!", w =  " What a good day!";

		auto message = VOLT::GetRawString(x + y + z + w);
		auto signiture = VOLT::GetSignedSHA256Digest(message, keyPair);
		auto signitureHex = VOLT::ConvertByteToHexData(signiture);

		std::cout << signitureHex << std::endl;

		signiture = VOLT::ConvertHexToByteData(signitureHex);
		std::cout << VOLT::VerifySHA256Digest(message, signiture, keyPair) << std::endl;
	}

	std::cin.get();
	return 0;
}