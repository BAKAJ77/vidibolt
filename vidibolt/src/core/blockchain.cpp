#include <core/blockchain.h>
#include <crypto/sha256.h>
#include <util/timestamp.h>
#include <util/data_conversion.h>
#include <util/log_handler.h>
#include <cmath>

namespace VOLT
{
	Blockchain::Blockchain(uint32_t difficulty) :
		blockDifficulty((uint32_t)std::round(std::log2(difficulty)))
	{}

	Blockchain::~Blockchain() {}

	TxErrorCode Blockchain::BroadcastTransaction(const ECKeyPair& senderKeyPair, const ECKeyPair& recipientPublicKey, 
		double amount)
	{
		Transaction tx(&senderKeyPair, recipientPublicKey, amount, TransactionType::TRANSFER);

		// We got to make sure the transaction is absolutely valid
		if (amount <= 0)
			return TxErrorCode::AMOUNT_SENT_INVALID;
		else if (this->GetAccountBalance(senderKeyPair) < amount)
			return TxErrorCode::SENDER_BALANCE_INSUFFICIENT;
		else if (tx.VerifySigniture() <= 0)
			return TxErrorCode::SIGNITURE_INVALID;
		else if (!senderKeyPair.HasPrivateKey())
			return TxErrorCode::SENDER_PRIVATE_KEY_MISSING;

		// Add the transaction to the pending list
		this->pendingTxns.emplace_back(tx);
		return TxErrorCode::SUCCESS;
	}

	int Blockchain::MineNextBlock(const ECKeyPair& minerPublicKey)
	{
		// Get the pending transactions at the front of the queue of up to VOLT_MAX_TRANSACTIONS_PER_BLOCK 
		// (defined in volt_api.h) per block
		std::vector<Transaction> blockTxs;
		if (this->pendingTxns.size() < VOLT_MAX_TRANSACTIONS_PER_BLOCK)
		{
			blockTxs = this->pendingTxns;
			this->pendingTxns.clear();
		}
		else
		{
			blockTxs.insert(blockTxs.begin(), this->pendingTxns.begin(), 
				this->pendingTxns.begin() + VOLT_MAX_TRANSACTIONS_PER_BLOCK);
			this->pendingTxns.erase(this->pendingTxns.begin(), this->pendingTxns.begin() + VOLT_MAX_TRANSACTIONS_PER_BLOCK);
		}

		// Insert reward transaction which will be sent to the miner
		double totalFees = 0;
		for (const auto& tx : blockTxs)
			totalFees += tx.GetFee();

		Transaction miningReward(nullptr, minerPublicKey, VOLT_MINING_REWARD + totalFees, TransactionType::MINING_REWARD);
		blockTxs.emplace_back(miningReward);

		// Create new block, pass previous block (if its not genesis block) so it's previous hash can be assigned
		auto previousBlock = this->GetLatestBlock();
		Block newBlock(&previousBlock, blockTxs);
		
		// Do proof of work to find a hash for the block that satisfies the difficulty set.
		// For the hash to be valid, it must be prefixed with an amount of zeros equal to the difficulty.
		std::string blockHash;
		bool hashValid = false;

		while (!hashValid)
		{
			blockHash = newBlock.CalculateHash();
			uint32_t numPrefixZeroDigits = 0;

			for (uint32_t i = 0; i < blockHash.size(); i++) 
			{
				if (blockHash[i] == '0')
					numPrefixZeroDigits++;
				else
					break;
			}

			if (numPrefixZeroDigits >= this->blockDifficulty)
				hashValid = true;
			else
				newBlock.nonse++;

#ifdef _DEBUG
			std::printf("%s\n", blockHash.c_str());
#endif
		}

		// Set the timestamp of the block and generate the final block hash based on the valid hash generated 
		// above and the timestamp retrieved.
		newBlock.timestamp = VOLT::GetCurrentTimestamp();
		auto hashInput = VOLT::GetRawString(blockHash + newBlock.timestamp);
		newBlock.blockHash = VOLT::ConvertByteToHexData(VOLT::GetSHA256Digest(hashInput));

		// Do final block verification check for safe keeping
		if (!newBlock.IsValid())
			return 0;

		// Append the created and verified block to the chain
		this->chain.emplace_back(newBlock);
		return 1;
	}

	double Blockchain::GetAccountBalance(const ECKeyPair& publicKey) const
	{
		double balance = 0;
		for (const auto& block : this->chain)
		{
			for (const auto& tx : block.GetTransactions())
			{
				if (publicKey.GetPublicKeyHex() == tx.GetRecipientPublicKey())
					balance += (tx.GetAmountSent() - tx.GetFee());
				else if (publicKey.GetPublicKeyHex() == tx.GetSenderPublicKey())
					balance -= tx.GetAmountSent();
			}
		}

		return balance;
	}

	bool Blockchain::IsValid() const
	{
		bool valid = true;
		const Block* previousBlock = nullptr;

		for (const auto& block : this->chain)
		{
			// Check if the block's hash is correct
			if (!block.IsValid())
			{
				valid = false;
				break;
			}

			// Check if the hash link between the previous and current block is valid 
			// A.K.A the previous hash of the current block is equal to the block hash of the previous block
			if (previousBlock)
			{
				if (block.previousHash != previousBlock->blockHash)
				{
					valid = false;
					break;
				}
			}

			previousBlock = &block;
		}

		return valid;
	}

	Block Blockchain::GetGenesisBlock() const
	{
		if (!this->chain.empty())
			return this->chain.front();

		return Block();
	}

	Block Blockchain::GetLatestBlock() const
	{
		if (!this->chain.empty())
			return this->chain.back();

		return Block();
	}

	Transaction Blockchain::GetTransaction(const std::string& txHash) const
	{
		for (const auto& block : this->chain)
		{
			for (const auto& tx : block.GetTransactions())
			{
				if (tx.GetHash() == txHash)
					return tx;
			}
		}

		return Transaction();
	}
}