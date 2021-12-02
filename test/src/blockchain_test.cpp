#include <crypto/ecdsa.h>
#include <core/block.h>
#include <core/chain.h>
#include <core/mem_pool.h>
#include <core/hash_stats.h>
#include <util/timestamp.h>
#include <crypto/sha256.h>

#include <iostream>

int main(int argc, char** argv)
{
	std::vector<double> recordedhashRates;
	Volt::ErrorCode keyGenError, miningError, chainAppendError, txError, txMempoolError, chainError;
	
	// Init 2 keypairs, manually create a keypair and generate a new keypair
	Volt::ECKeyPair keyPair("VPK_022102EEFF84CBD0D70BA47E778E451D7A38F2E6AA2E885692DCEB731377F6F18F", 
		"0B42DACF5D7B8B825FBB4543298601359FB27A8D85147F3235F0DAA140553DD6", &keyGenError);
	Volt::ECKeyPair keyPair2(&keyGenError);

	// Blockchain related stuff
	Volt::Chain chain;
	Volt::MemPool memPool;
	Volt::Block block;

	// Mine 2 blocks and append them to the chain
	miningError = Volt::MineNextBlock(memPool, block, chain, 2, keyPair);
	chainAppendError = Volt::PushBlock(chain, block);

	recordedhashRates.emplace_back(Volt::GetCurrentHashesPerSecond());

	miningError = Volt::MineNextBlock(memPool, block, chain, 2, keyPair);
	chainAppendError = Volt::PushBlock(chain, block);

	recordedhashRates.emplace_back(Volt::GetCurrentHashesPerSecond());

	// Create a transaction and push the transaction to the mempool
	txError = Volt::PushTransaction(memPool, chain, 100, 20, keyPair, keyPair2);
	
	// Mine 3rd block
	miningError = Volt::MineNextBlock(memPool, block, chain, 2, keyPair2, [](const Volt::Transaction& tx) { return true; });
	chainAppendError = Volt::PushBlock(chain, block);

	recordedhashRates.emplace_back(Volt::GetCurrentHashesPerSecond());

	// Print the average of the hash rates recorded
	double total = 0;
	for (const auto& hashRate : recordedhashRates)
		total += hashRate;

	std::cout << "Average Hash Rate: " << (total / recordedhashRates.size()) << " H/s\n\n";

	// Print out the balance of public key addresses
	std::cout << "| " << keyPair.GetPublicKeyHex() << " | Balance: " << chain.GetAddressBalance(keyPair) << std::endl;
	std::cout << "| " << keyPair2.GetPublicKeyHex() << " | Balance: " << chain.GetAddressBalance(keyPair2) << std::endl;

	// Print the current block height of the chain
	std::cout << "Current Block Height: " << chain.GetLatestBlockHeight() << std::endl;

	// Verify the chain
	chainError = Volt::VerifyChain(chain);
	std::cout << "Is Chain Valid: " << (!chainError ? "Yes" : "No") << std::endl << std::endl;

	std::system("pause");
	return 0;
}