#include <core/block.h>
#include <util/log_handler.h>
#include <crypto/sha256.h>

namespace VOLT
{
	Block::Block() :
		nonse(0), index(0)
	{}
	
	Block::Block(const Block* previousBlock, const std::vector<Transaction>& txs) :
		nonse(0), txs(txs)
	{
		if (previousBlock)
		{
			this->index = previousBlock->index + 1;
			this->previousHash = previousBlock->blockHash;
		}
		else
			this->index = 0;
	}

	Block::Block(const Block& other) :
		index(other.index), previousHash(other.previousHash), blockHash(other.blockHash),
		timestamp(other.timestamp), nonse(other.nonse), txs(other.txs)
	{}

	Block::~Block() {}
	
	std::string Block::CalculateHash() const
	{
		// Construct one string consisting of all transactional data in the block
		std::string txData;
		for (const auto& tx : this->txs)
			txData += tx.GetTxDataString();

		// Calculate the hash of the block
		std::vector<uint8_t> blockData, digest;
		blockData = VOLT::GetRawString(std::to_string(index) + this->previousHash + std::to_string(this->nonse) + txData);
		const std::string BLOCK_HASH = VOLT::ConvertByteToHexData(VOLT::GetSHA256Digest(blockData));

		return BLOCK_HASH;
	}

	bool Block::IsValid() const
	{
		// Make sure the transactions in the block are valid
		for (uint32_t i = 0; i < this->txs.size(); i++)
		{
			for (uint32_t j = 0; j < this->txs.size(); j++)
			{
				// Check for identical/duplicate transactions in the block
				if (i != j && this->txs[i] == this->txs[j])
					return false;
			}

			// Check for transaction validity
			if (this->txs[i].GetType() == TransactionType::TRANSFER && this->txs[i].VerifySigniture() <= 0)
				return false;
		}

		// Construct one string consisting of all transactional data in the block
		std::string txData;
		for (const auto& tx : this->txs)
			txData += tx.GetTxDataString();

		// Calculate the hash of the block
		std::vector<uint8_t> blockData, digest;
		blockData = VOLT::GetRawString(std::to_string(index) + this->previousHash + std::to_string(this->nonse) + txData);
		std::string generatedBlockHash = VOLT::ConvertByteToHexData(VOLT::GetSHA256Digest(blockData));

		// Generate the final block hash based on the valid hash generated above and the timestamp retrieved.
		auto hashInput = VOLT::GetRawString(generatedBlockHash + this->timestamp);
		generatedBlockHash = VOLT::ConvertByteToHexData(VOLT::GetSHA256Digest(hashInput));

		if (this->blockHash == generatedBlockHash)
			return true;

		return false;
	}

	const std::string& Block::GetPreviousHash() const
	{
		return this->previousHash;
	}

	const std::string& Block::GetBlockHash() const
	{
		return this->blockHash;
	}

	const std::string& Block::GetTimestamp() const
	{
		return this->timestamp;
	}

	const uint32_t& Block::GetBlockHeight() const
	{
		return this->index;
	}

	const uint32_t& Block::GetNonse() const
	{
		return this->nonse;
	}

	const std::vector<Transaction>& Block::GetTransactions() const
	{
		return this->txs;
	}
}