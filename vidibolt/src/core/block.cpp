#include <core/block.h>
#include <crypto/sha256.h>
#include <core/chain.h>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Block::Implementation
	{
	public:
		uint32_t index;
		uint64_t timestamp, nonce;
		std::string previousHash, hash;
		std::vector<Transaction> txs;
	public:
		Implementation() :
			index(0), timestamp(0), nonce(0)
		{}

		Implementation(uint32_t index, uint64_t timestamp, const std::string& prevHash, const std::vector<Transaction>& txs,
			const std::string& blockHash) :
			index(index), timestamp(timestamp), previousHash(prevHash), txs(txs), hash(blockHash), nonce(0)
		{}

		~Implementation() = default;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Block::Block() :
		impl(std::make_unique<Implementation>())
	{}

	Block::Block(const Block& block) :
		impl(std::make_unique<Implementation>(block.GetIndex(), block.GetTimestamp(), block.GetPreviousBlockHash(),
			block.GetTransactions(), block.GetBlockHash()))
	{}

	Block::Block(uint32_t index, const std::string& prevHash, const std::vector<Transaction>& txs,
		const std::string& blockHash, uint64_t timestamp) :
		impl(std::make_unique<Implementation>(index, timestamp, prevHash, txs, blockHash))
	{}

	Block::~Block() = default;

	void Block::operator=(const Block& block)
	{
		this->impl = std::make_unique<Implementation>(block.GetIndex(), block.GetTimestamp(), block.GetPreviousBlockHash(),
			block.GetTransactions(), block.GetBlockHash());
	}

	ErrorCode Block::GenerateBlockHash(std::string& outputBlockHash) const
	{
		// Combine the data of all transactions into one string of data
		std::string combinedTxsData;
		for (const auto& tx : this->impl->txs)
			combinedTxsData += Volt::SerializeTransaction(tx);

		// Combine all block data into one string, then convert string data into byte vector array
		const std::string blockData = std::to_string(this->impl->index) + std::to_string(this->impl->timestamp) +
			std::to_string(this->impl->nonce) + this->impl->previousHash + combinedTxsData;

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
		ErrorCode error;
		
		// Check that all the transactions contained are valid
		for (const auto& tx : block.GetTransactions())
		{
			error = Volt::VerifyTransaction(tx);
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

			if (block.GetTimestamp() <= prevBlock.GetTimestamp())
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
		error = block.GenerateBlockHash(hash);
		if (!error && (block.GetBlockHash() != hash))
			return ErrorID::BLOCK_HASH_INVALID;

		return error;
	}

	Block GetGenesisBlock()
	{
		return Block(0, "", {}, "238862aa5024ec554942e5d009e84d0ae3586d96f81e1190faa69c67b559486c", 1638318078);
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