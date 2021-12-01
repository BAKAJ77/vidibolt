#include <crypto/ecdsa.h>
#include <core/mem_pool.h>
#include <core/block.h>
#include <core/chain.h>
#include <util/timestamp.h>

#include <iostream>

int main(int argc, char** argv)
{
	// Generate the EC key
	Volt::ErrorCode keyGenError;

	Volt::ECKeyPair keyPair("VPK_022102EEFF84CBD0D70BA47E778E451D7A38F2E6AA2E885692DCEB731377F6F18F", 
		"0B42DACF5D7B8B825FBB4543298601359FB27A8D85147F3235F0DAA140553DD6", &keyGenError);
	Volt::ECKeyPair keyPair2(&keyGenError);

	// Transaction and Mempool related stuff
	Volt::Chain chain;
	Volt::MemPool memPool;

	Volt::Transaction tx = { 4000, 800, Volt::GetTimeSinceEpoch(), keyPair.GetPublicKeyHex(), keyPair2.GetPublicKeyHex() };
	Volt::Transaction tx2 = { 4283, 500, Volt::GetTimeSinceEpoch(), keyPair2.GetPublicKeyHex(), keyPair.GetPublicKeyHex() };
	Volt::SignTransaction(tx, keyPair);
	Volt::SignTransaction(tx2, keyPair2);

	Volt::PushTransaction(memPool, tx, chain);
	Volt::PushTransaction(memPool, tx2, chain);

	auto& txs = Volt::PopTransactions(memPool, VOLT_MAX_TRANSACTIONS_PER_BLOCK);
	Volt::ErrorCode txError = Volt::VerifyTransaction(tx);

	std::cout << (!txError ? "true" : "false") << std::endl;
	std::cout << tx << std::endl;
	std::cin.get();


	// Block and Chain related stuff
	Volt::Block block(1, chain.GetLatestBlock().GetBlockHash(), {}, 
		"E466A10B4F6B01E738087CED3250814C309981B0F3B104A0D138987D7F8B525A", 1638318978);

	Volt::ErrorCode blockError = Volt::PushBlock(chain, block);
	Volt::ErrorCode chainError = Volt::VerifyChain(chain);

	std::cout << (!chainError ? "true" : "false") << std::endl;

	std::system("pause");
	return 0;
}