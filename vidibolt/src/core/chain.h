#ifndef VIDIBOLT_CORE_CHAIN_H
#define VIDIBOLT_CORE_CHAIN_H

#include <util/volt_api.h>
#include <core/block.h>

#include <vector>
#include <memory>

namespace Volt
{
	class Chain
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	public:
		VOLT_API Chain();
		VOLT_API Chain(const Chain& chain);
		VOLT_API Chain(const std::vector<Block>& blockChain);

		VOLT_API ~Chain();

		/*
			Operator overload for assignment operations.
		*/
		VOLT_API void operator=(const Chain& chain);

		friend extern VOLT_API ErrorCode PushBlock(Chain& chain, const Block& block);
		friend extern VOLT_API ErrorCode VerifyChain(const Chain& chain);

		VOLT_API const Block& GetLatestBlock() const;
		VOLT_API const Block& GetBlockAtIndexHeight(uint32_t blockIndex) const;
		VOLT_API const std::vector<Block>& GetBlockChain() const;

		VOLT_API uint32_t GetLatestBlockHeight() const;
	};
}

#endif
