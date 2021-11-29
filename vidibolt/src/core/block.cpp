#include <core/block.h>
#include <crypto/sha256.h>

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

	ErrorID Block::GenerateBlockHash()
	{
		// Push the data from all transactions in the block into the string
		std::string combinedTxsData;
		for (const auto& tx : this->impl->txs)
			combinedTxsData += Volt::SerializeTransaction(tx);

		// Combine all block data into one string, then convert string data into byte vector array
		const std::string blockData = std::to_string(this->impl->index) + std::to_string(this->impl->timestamp) +
			std::to_string(this->impl->nonce) + this->impl->previousHash + combinedTxsData;

		const std::vector<uint8_t> rawBlockData = Volt::GetRawString(blockData);

		// Generate the hash
		std::vector<uint8_t> rawHashDigest;
		ErrorID error = Volt::GetSHA256Digest(rawBlockData, rawHashDigest);
		if (error == ErrorID::NONE)
			this->impl->hash = Volt::ConvertByteToHexData(rawHashDigest);

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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}