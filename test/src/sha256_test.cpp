#include <util/data_conversion.h>
#include <crypto/sha256.h>
#include <crypto/ecdsa.h>
#include <iostream>

int main(int argc, char** argv)
{
	// Generate the EC key
	Volt::ECKeyPair keyPair;

	std::cout << keyPair.GetPublicKeyHex() << std::endl;
	std::cout << keyPair.GetPrivateKeyHex() << std::endl;
	std::cout << keyPair.IsValid() << std::endl;

	std::cin.get();

	for (int i = 0; i < 10; i++)
	{
		std::string x = "Hello", y = "World", z = "Wow!", w =  " What a good day!";

		auto message = Volt::GetRawString(x + y + z + w);
		std::vector<uint8_t> signiture;
		Volt::GetSignedSHA256Digest(message, keyPair, signiture);
		auto signitureHex = Volt::ConvertByteToHexData(signiture);

		std::cout << signitureHex << std::endl;

		signiture = Volt::ConvertHexToByteData(signitureHex);
		int valid = 0;

		Volt::ErrorCode verifyError = Volt::VerifySHA256Digest(message, keyPair, signiture);
		std::cout << (verifyError ? "false" : "true") << std::endl;
	}

	std::cin.get();
	return 0;
}