#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <util/volt_api.h>
#include <core/transaction.h>
#include <core/block.h>
#include <vector>

namespace VOLT
{
	enum class TxErrorCode : int
	{
		SUCCESS = 1,
		SIGNITURE_INVALID = 0,
		AMOUNT_SENT_INVALID = -1,
		SENDER_BALANCE_INSUFFICIENT = -2,
		SENDER_PRIVATE_KEY_MISSING = -3
	};

	class VOLT_API Blockchain
	{
	private:
		std::vector<Block> chain;
		std::vector<Transaction> pendingTxns;
		uint32_t blockDifficulty;
	public:
		Blockchain(uint32_t difficulty);
		~Blockchain();

		/*
			Broadcast transaction to network, the transaction is added to the pending txs list.
			Returns a enum indicating whether the transaction was successfully added to pending txns or if an error
			occurred, the error is given as an enum value.
		*/
		TxErrorCode BroadcastTransaction(const ECKeyPair& senderKeyPair, const ECKeyPair& recipientPublicKey,
			double amount);

		/*
			Generates next block and runs proof-of-work to validate it, once done it gets added to the chain.
			Returns 1 if the block was appended to the chain successfuly, else 0 is returned.  
		*/
		int MineNextBlock(const ECKeyPair& minerPublicKey);

		/*
			Returns the balance of the account tied to the public key given.
		*/
		double GetAccountBalance(const ECKeyPair& publicKey) const;

		/*
			Returns TRUE if the chain is valid, else if its not then returned FALSE.
		*/
		bool IsValid() const;

		/*
			Returns the genesis block in the chain.
		*/
		Block GetGenesisBlock() const;

		/*
			Returns the latest block appended to the chain.
		*/
		Block GetLatestBlock() const;

		/*
			Returns the confirmed transaction matching the transaction hash given.
		*/
		Transaction GetTransaction(const std::string& txHash) const;
	};
}

#endif
