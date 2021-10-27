#include <core/blockchain.h>
#include <crypto/ecdsa.h>
#include <core/transaction.h>
#include <iostream>

int main(int argc, char** argv)
{
	// Generate the EC key
	VOLT::ECKeyPair keyPair("vpk_0368bb1821f006fd796b4765a91ff9cbcc0395927a175f0a493c5e23e488aefc38",
		"6e3a8c4a4fbca18dfe35d9cc1ad257b50df647aa0334357c3a56d5241ee474d5");
	VOLT::ECKeyPair keyPair2, keyPair3;

	VOLT::Blockchain voltBlockchain(8);

	// Mine block 1
	voltBlockchain.MineNextBlock(keyPair);
	auto block = voltBlockchain.GetGenesisBlock();
	auto genesisTxs = block.GetTransactions();

	std::cout << std::endl << voltBlockchain.GetAccountBalance(keyPair) << std::endl;
	std::cin.get();

	auto txStatus = voltBlockchain.BroadcastTransaction(keyPair, keyPair2, 250);

	// Mine block 2
	voltBlockchain.MineNextBlock(keyPair);
	auto latest = voltBlockchain.GetLatestBlock();
	auto latestBlockTxs = latest.GetTransactions();
	auto tx = voltBlockchain.GetTransaction(latestBlockTxs[0].GetHash());
	
	std::cout << std::endl << voltBlockchain.GetAccountBalance(keyPair) << std::endl;
	std::cin.get();

	auto txStatus2 = voltBlockchain.BroadcastTransaction(keyPair2, keyPair3, 50);
	auto txStatus3 = voltBlockchain.BroadcastTransaction(keyPair2, keyPair3, 200);
	auto txStatus4 = voltBlockchain.BroadcastTransaction(keyPair2, keyPair3, 70);

	voltBlockchain.MineNextBlock(keyPair);
	auto latest2 = voltBlockchain.GetLatestBlock();
	auto latestBlockTxs2 = latest.GetTransactions();
	auto tx2 = voltBlockchain.GetTransaction(latestBlockTxs[0].GetHash());

	std::cout << std::endl << voltBlockchain.GetAccountBalance(keyPair) << std::endl;
	std::cout << voltBlockchain.IsValid() << std::endl;
	std::cin.get();

	return 0;
}