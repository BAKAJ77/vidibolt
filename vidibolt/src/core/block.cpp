#include <core/block.h>
#include <core/mem_pool.h>
#include <core/chain.h>
#include <crypto/sha256.h>
#include <util/random_generation.h>
#include <util/timestamp.h>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Block::Implementation
	{
	public:
		uint32_t index;
		uint64_t timestamp, difficulty, nonce;
		std::string previousHash, hash;
		std::vector<Transaction> txs;
	public:
		Implementation() :
			index(0), timestamp(0), nonce(0), difficulty(0)
		{}

		Implementation(uint32_t index, uint64_t timestamp, const std::string& prevHash, const std::vector<Transaction>& txs,
			const std::string& blockHash, uint64_t difficulty, uint64_t nonce) :
			index(index), timestamp(timestamp), previousHash(prevHash), txs(txs), hash(blockHash), nonce(nonce), 
			difficulty(difficulty)
		{}

		~Implementation() = default;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Block::Block() :
		impl(std::make_unique<Implementation>())
	{}

	Block::Block(const Block& block) :
		impl(std::make_unique<Implementation>(block.GetIndex(), block.GetTimestamp(), block.GetPreviousBlockHash(),
			block.GetTransactions(), block.GetBlockHash(), block.GetDifficulty(), block.GetNonce()))
	{}

	Block::Block(uint32_t index, const std::string& prevHash, const std::vector<Transaction>& txs, uint64_t difficulty,
		const std::string& blockHash, uint64_t timestamp, uint64_t nonce) :
		impl(std::make_unique<Implementation>(index, timestamp, prevHash, txs, blockHash, difficulty, nonce))
	{}

	Block::~Block() = default;

	void Block::operator=(const Block& block)
	{
		this->impl = std::make_unique<Implementation>(block.GetIndex(), block.GetTimestamp(), block.GetPreviousBlockHash(),
			block.GetTransactions(), block.GetBlockHash(), block.GetDifficulty(), block.GetNonce());
	}

	ErrorCode Block::GenerateBlockHash(std::string& outputBlockHash) const
	{
		// Combine the data of all transactions into one string of data
		std::string combinedTxsData;
		for (const auto& tx : this->impl->txs)
			combinedTxsData += Volt::SerializeTransaction(tx);

		// Combine all block data into one string, then convert string data into byte vector array
		const std::string blockData = std::to_string(this->impl->index) + std::to_string(this->impl->nonce) + 
			this->impl->previousHash + combinedTxsData;

		const std::vector<uint8_t> rawBlockData = Volt::GetRawString(blockData);

		// Generate the hash
		std::vector<uint8_t> rawHashDigest;
		ErrorCode error = Volt::GetSHA256Digest(rawBlockData, rawHashDigest);
		if (!error)
			outputBlockHash = Volt::ConvertByteToHexData(rawHashDigest); // The block hash was successfully generated

		return error;
	}

	const uint32_t& Block::GetIndex() const
	{
		return this->impl->index;
	}

	const uint64_t& Block::GetTimestamp() const
	{
		return this->impl->timestamp;
	}

	const uint64_t& Block::GetDifficulty() const
	{
		return this->impl->difficulty;
	}

	const uint64_t& Block::GetNonce() const
	{
		return this->impl->nonce;
	}

	const std::string& Block::GetPreviousBlockHash() const
	{
		return this->impl->previousHash;
	}

	const std::string& Block::GetBlockHash() const
	{
		return this->impl->hash;
	}

	const std::vector<Transaction>& Block::GetTransactions() const
	{
		return this->impl->txs;
	}

	ErrorCode VerifyBlock(const Block& block, const Chain& chain)
	{
		// Check that all the transactions contained are valid
		for (const auto& tx : block.GetTransactions())
		{
			ErrorCode error = Volt::VerifyTransaction(tx);
			if (error)
				return error;
		}

		// Check that the previous hash stored in the block is correct and the timestamp make sense 
		// (the timestamp of the this block must be larger than the previous block's timestamp).
		// 
		// Also, the index of the block must be equal to the index of the previous block + 1
		if (block.GetIndex() != 0)
		{
			const Block& prevBlock = chain.GetBlockAtIndexHeight(block.GetIndex() - 1);
			if (block.GetPreviousBlockHash() != prevBlock.GetBlockHash())
				return ErrorID::BLOCK_PREVIOUS_HASH_INVALID;

			if (block.GetTimestamp() < prevBlock.GetTimestamp())
				return ErrorID::BLOCK_TIMESTAMP_INVALID;

			if (block.GetIndex() != prevBlock.GetIndex() + 1)
				return ErrorID::BLOCK_INDEX_INVALID;
		}
		else
		{
			const Block& genesisBlock = chain.GetBlockChain().front();
			if (genesisBlock != Volt::GetGenesisBlock())
				return ErrorID::GENESIS_BLOCK_INVALID;
		}

		// Check that the hash of the block is valid [TODO: SUBJECT TO CHANGE]
		std::string hash;
		ErrorCode error = block.GenerateBlockHash(hash);
		if (error)
			return error;

		const std::string hashInput = hash + std::to_string(block.GetTimestamp());
		std::vector<uint8_t> buffer = Volt::GetRawString(hashInput), finalHashBuffer;

		error = Volt::GetSHA256Digest(buffer, finalHashBuffer);
		if (error)
			return error;

		if (block.GetBlockHash() != Volt::ConvertByteToHexData(finalHashBuffer))
			return ErrorID::BLOCK_HASH_INVALID;

		return ErrorID::NONE;
	}

	ErrorCode MineNextBlock(MemPool& pool, Block& minedBlock, const Chain& chain, uint64_t difficulty, 
		const ECKeyPair& minerPublicKey, std::function<bool(const Transaction&)> txHandler)
	{
		// Get the latest block in the chain and get transactions to be included into the block
		const Block& latestBlock = chain.GetLatestBlock();
		std::vector<Transaction> txs;

		if (txHandler) // Use the given custom transaction handler function
		{
			for (size_t i = 0; i < pool.GetPoolSize(); i++)
			{
				if (i < VOLT_MAX_TRANSACTIONS_PER_BLOCK)
				{
					const Transaction tx = Volt::PopTransactionAtIndex(pool, i);
					if (txHandler(std::ref(tx)))
						txs.emplace_back(tx);
				}
				else
					break;
			}
		}
		else // No custom handler function was given, so just get the transactions at the front of queue in the mempool
			txs = Volt::PopTransactions(pool, VOLT_MAX_TRANSACTIONS_PER_BLOCK);

		// Add mining reward transaction for the miner to the block
		Transaction tx(TransactionType::MINING_REWARD, Volt::GenerateRandomUint64(0, UINT64_MAX), VOLT_MINING_REWARD, 0,
			Volt::GetTimeSinceEpoch(), "", minerPublicKey.GetPublicKeyHex());

		txs.emplace_back(tx);

		// Initialize the block
		minedBlock = { latestBlock.GetIndex() + 1, latestBlock.GetBlockHash(), txs, difficulty };

		// Start doing proof-of-work (find a hash that satisfies the block difficulty)
		std::string generatedHash;
		bool hashValid = false;

		while (!hashValid)
		{
			// Generate a new hash
			ErrorCode error = minedBlock.GenerateBlockHash(generatedHash);
			if (error)
				return error;

			// Check if the hash is valid
			hashValid = true;
			for (size_t i = 0; i < difficulty; i++)
			{
				if (generatedHash[i] != '0')
				{
					hashValid = false;
					break;
				}
			}

			// Increment the nonce value if the generated hash doesn't satisfy the block difficulty
			if (!hashValid)
				minedBlock.impl->nonce++;
		}

		// Assign the current timestamp to the block
		minedBlock.impl->timestamp = Volt::GetTimeSinceEpoch();

		// Get the hash of the generated hash combined with the timestamp
		const std::string hashInput = generatedHash + std::to_string(minedBlock.GetTimestamp());
		std::vector<uint8_t> buffer = Volt::GetRawString(hashInput), finalHashBuffer;

		ErrorCode error = Volt::GetSHA256Digest(buffer, finalHashBuffer);
		if (error)
			return error;
		
		// Finally, assign the final generated hash to the block
		minedBlock.impl->hash = Volt::ConvertByteToHexData(finalHashBuffer);
		return ErrorID::NONE;
	}

	Block GetGenesisBlock()
	{
		return Block(0, "", {}, 0, "AC7FDA5E0E2BF8B6600D4AFAC9C6095E89E9C14B30BC4A114FAB090BCAFADC79", 1638318078);
	}

	bool operator==(const Block& lhs, const Block& rhs)
	{
		return lhs.GetIndex() == rhs.GetIndex() &&
			lhs.GetTimestamp() == rhs.GetTimestamp() &&
			lhs.GetNonce() == rhs.GetNonce() &&
			lhs.GetPreviousBlockHash() == rhs.GetPreviousBlockHash() &&
			lhs.GetBlockHash() == rhs.GetBlockHash();
	}

	bool operator!=(const Block& lhs, const Block& rhs)
	{
		return lhs.GetIndex() != rhs.GetIndex() ||
			lhs.GetTimestamp() != rhs.GetTimestamp() ||
			lhs.GetNonce() != rhs.GetNonce() ||
			lhs.GetPreviousBlockHash() != rhs.GetPreviousBlockHash() ||
			lhs.GetBlockHash() != rhs.GetBlockHash();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}