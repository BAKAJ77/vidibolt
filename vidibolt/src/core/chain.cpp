#include <core/chain.h>
#include <cassert>

namespace Volt
{
	class Chain::Implementation
	{
	public:
		UnorderedMap<uint32_t, Block> blockChain;
	public:
		Implementation()
		{
			const Block genesis = Volt::GetGenesisBlock();
			this->blockChain.Insert(genesis.GetIndex(), genesis);
		};

		Implementation(const Implementation& impl) :
			blockChain(impl.blockChain)
		{}

		Implementation(const UnorderedMap<uint32_t, Block>& blockChain) :
			blockChain(blockChain)
		{
			assert(!blockChain.IsEmpty()); // The blockchain must always have at least a genesis block in it
		}

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

	Chain::Chain(const UnorderedMap<uint32_t, Block>& blockChain) :
		impl(std::make_unique<Implementation>(blockChain))
	{}

	Chain::~Chain() = default;

	void Chain::operator=(const Chain& chain)
	{
		this->impl = std::make_unique<Implementation>(*chain.impl);
	}

	const Block& Chain::GetLatestBlock() const
	{
		return this->impl->blockChain.GetElement(this->GetLatestBlockHeight());
	}

	const Block& Chain::GetBlockAtIndexHeight(uint32_t blockIndex) const
	{
		return this->impl->blockChain.GetElement(blockIndex);
	}

	const UnorderedMap<uint32_t, Block>& Chain::GetBlockChain() const
	{
		return this->impl->blockChain;
	}

	double Chain::GetAddressBalance(const ECKeyPair& publicKey) const
	{
		double balance = 0;
		for (uint32_t blockIndex = 0; blockIndex < this->impl->blockChain.GetSize(); blockIndex++)
		{
			const Block& block = this->impl->blockChain.GetElement(blockIndex);

			for (const auto& tx : block.GetTransactions())
			{
				if (publicKey.GetPublicKeyHex() == tx.GetSenderKey())
					balance -= (tx.GetAmount() + tx.GetFee());
				else if (publicKey.GetPublicKeyHex() == tx.GetRecipientKey())
					balance += tx.GetAmount();
			}
		}

		return balance;
	}

	double Chain::GetMiningRewardAmount() const
	{
		constexpr double base = 75, decreaseFactor = 1.5;
		constexpr int decreaseRate = 3435000; // Decrease mining reward by the decrease factor every 3,435,000 blocks
		const int epochs = (int)(this->GetLatestBlockHeight() + 1) / decreaseRate;

		return std::max(base / std::pow(decreaseFactor, epochs), 0.3);
	}

	uint32_t Chain::GetLatestBlockHeight() const
	{
		return (uint32_t)(this->impl->blockChain.GetSize() - 1);
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
			chain.impl->blockChain.Insert(block.GetIndex(), block);

		return error;
	}

	ErrorCode VerifyChain(const Chain& chain)
	{
		// There must be a genesis block in the chain
		if (chain.GetLatestBlockHeight() < 1)
			return ErrorID::CHAIN_EMPTY;

		for (size_t blockIndex = 0; blockIndex < chain.impl->blockChain.GetSize(); blockIndex++)
		{
			const Block& block = chain.impl->blockChain.GetElement((uint32_t)blockIndex);

			// Check that the block is valid
			ErrorCode error = Volt::VerifyBlock(block, chain);
			if (error)
				return error;
		}

		return ErrorID::NONE;
	}
}