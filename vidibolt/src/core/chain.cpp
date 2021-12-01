#include <core/chain.h>

namespace Volt
{
	class Chain::Implementation
	{
	public:
		std::vector<Block> blockChain;
	public:
		Implementation() :
			blockChain({ Volt::GetGenesisBlock() })
		{};

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

	uint64_t Chain::GetAddressBalance(const std::string& publicKey) const
	{
		uint64_t balance = 0;
		for (const auto& block : this->impl->blockChain)
		{
			for (const auto& tx : block.GetTransactions())
			{
				if (publicKey == tx.GetSenderKey())
					balance -= tx.GetAmount();
				else if (publicKey == tx.GetRecipientKey())
					balance += tx.GetAmount();
			}
		}

		return balance;
	}

	uint32_t Chain::GetLatestBlockHeight() const
	{
		// Make sure value returned is never less than 0
		return (uint32_t)std::max(this->impl->blockChain.size() - 1, (size_t)0);
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
		for (size_t i = 0; i < chain.impl->blockChain.size(); i++)
		{
			const Block& block = chain.impl->blockChain[i];

			// Check that the block is valid
			ErrorCode error = Volt::VerifyBlock(block, chain);
			if (error)
				return error;
		}

		return ErrorID::NONE;
	}
}