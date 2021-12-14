#include <core/chain.h>
#include <crypto/sha256.h>
#include <cassert>
#include <sstream>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Chain::Implementation
	{
	public:
		std::vector<Block> blockChain;
	public:
		Implementation() :
			blockChain({ Volt::GetGenesisBlock() })
		{}

		Implementation(const Implementation& impl) :
			blockChain(impl.blockChain)
		{}

		Implementation(const std::vector<Block>& blockChain) :
			blockChain(blockChain)
		{}

		~Implementation() = default;

		void operator=(const Implementation& impl)
		{
			this->blockChain = impl.blockChain;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Chain::Chain() :
		impl(std::make_unique<Implementation>())
	{}

	Chain::Chain(const Chain& chain) :
		impl(std::make_unique<Implementation>(*chain.impl))
	{}

	Chain::Chain(const std::vector<Block>& blockChain) :
		impl(std::make_unique<Implementation>(blockChain))
	{}

	Chain::~Chain() = default;

	void Chain::operator=(const Chain& chain)
	{
		this->impl = std::make_unique<Implementation>(*chain.impl);
	}

	const Block& Chain::GetLatestBlock() const
	{
		return this->impl->blockChain.back();
	}

	const Block& Chain::GetBlockAtIndexHeight(uint32_t blockIndex) const
	{
		return this->impl->blockChain[blockIndex];
	}

	const std::vector<Block>& Chain::GetBlockChain() const
	{
		return this->impl->blockChain;
	}

	double Chain::GetAddressBalance(const ECKeyPair& publicKey) const
	{
		double balance = 0;

		for (uint32_t blockIndex = 0; blockIndex < this->impl->blockChain.size(); blockIndex++)
		{
			const Block& block = this->impl->blockChain[blockIndex];

			for (const Transaction& tx : block.GetTransactions())
			{
				if (publicKey.GetPublicKeyHex() == tx.GetSenderKey())
					balance -= (tx.GetAmount() + tx.GetFee());
				else if (publicKey.GetPublicKeyHex() == tx.GetRecipientKey())
					balance += tx.GetAmount();
			}
		}

		return balance;
	}

	double Chain::GetMiningRewardAmount(uint32_t atBlockIndex) const
	{
		uint32_t blockHeight = (atBlockIndex == UINT32_MAX ? (this->GetLatestBlockHeight() + 1) : (atBlockIndex + 1));

		constexpr double base = 75, decreaseFactor = 1.5;
		constexpr int decreaseRate = 3435000; // Decrease mining reward by the decrease factor every 3,435,000 blocks
		const int epochs = (int)(blockHeight) / decreaseRate;

		return std::max(base / std::pow(decreaseFactor, epochs), 0.3);
	}

	uint32_t Chain::GetLatestBlockHeight() const
	{
		return (uint32_t)(this->impl->blockChain.size() - 1);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Chain CreateExistingChain(const std::vector<Block>& blockChain)
	{
		return Chain(blockChain);
	}

	ErrorCode PushBlock(Chain& chain, const Block& block)
	{
		// The block must be valid for it to be appended to the chain, so we check if it is before appending it to the chain.
		// 
		// For a block to be valid it must satisfy these conditions:
		// - All transactions in the block must be valid.
		// - The previous hash of the block must be equal to the hash of the previous block.
		// - The timestamp of the block must be larger than the timestamp of the previous block.
		// - The index of the block must be equal to the previous block's index + 1.
		// - The hash of the block must be valid.

		ErrorCode error = Volt::VerifyBlock(block, chain);
		if (!error)
			chain.impl->blockChain.emplace_back(block);

		return error;
	}

	ErrorCode VerifyChain(const Chain& chain)
	{
		// There must be a genesis block in the chain
		if (chain.GetLatestBlockHeight() < 1)
			return ErrorID::CHAIN_EMPTY;

		for (uint32_t blockIndex = 0; blockIndex < (uint32_t)chain.impl->blockChain.size(); blockIndex++)
		{
			const Block& block = chain.impl->blockChain[blockIndex];

			// Check that the block is valid
			ErrorCode error = Volt::VerifyBlock(block, chain);
			if (error)
				return error;
		}

		return ErrorID::NONE;
	}

	ErrorCode FindTransaction(const Chain& chain, const std::string& txHash, Transaction& returnedTx)
	{
		// Extract timestamp from the transaction hash string
		const std::string timestampHex = txHash.substr(SHA_256_DIGEST_LENGTH_HEX, txHash.size() - SHA_256_DIGEST_LENGTH_HEX);
		uint64_t timestamp = Volt::ConvertHexToUint(timestampHex);

		// Attempt to find the transaction matching the hash given
		for (uint32_t blockIndex = 0; blockIndex < (uint32_t)chain.impl->blockChain.size(); blockIndex++)
		{
			const Block& block = chain.impl->blockChain[blockIndex];

			// No point searching through blocks that were created before the transaction was.
			// So we skip blocks that existed before the transaction did.
			if (block.GetTimestamp() >= timestamp)
			{
				for (const Transaction& tx : block.GetTransactions())
				{
					if (tx.GetTxHash() == txHash)
					{
						returnedTx = tx;
						return ErrorID::NONE;
					}
				}
			}
		}

		return ErrorID::TRANSACTION_NOT_FOUND;
	}

	std::string SerializeChain(const Chain& chain)
	{
		// Initialize the json serializer
		json::serializer serializer;
		const json::value chainData = json::value_from(chain);
		serializer.reset(&chainData);

		// Stream serialized data into stringstream object
		std::stringstream jsonStream;
		while (!serializer.done())
		{
			char buffer[BOOST_JSON_STACK_BUFFER_SIZE];
			jsonStream << serializer.read(buffer);
		}

		return jsonStream.str();
	}

	std::ostream& operator<<(std::ostream& stream, const Chain& chain)
	{
		stream << Volt::SerializeChain(chain);
		return stream;
	}

	void tag_invoke(json::value_from_tag, json::value& obj, const Chain& chain)
	{
		obj = {
			{ "blocks", chain.GetBlockChain() }
		};
	}

	Chain tag_invoke(json::value_to_tag<Chain>, const json::value& obj)
	{
		return Volt::CreateExistingChain(json::value_to<std::vector<Block>>(obj.at("blocks")));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}