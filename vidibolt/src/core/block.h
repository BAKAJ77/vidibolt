#ifndef VIDIBOLT_CORE_BLOCK_H
#define VIDIBOLT_CORE_BLOCK_H

#include <util/volt_api.h>
#include <core/transaction.h>

#include <string>
#include <vector>
#include <memory>

namespace Volt
{
	class Block
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	public:
		VOLT_API Block();
		VOLT_API Block(const Block& block);
		VOLT_API Block(uint32_t index, const std::string& prevHash, const std::vector<Transaction>& txs,
			const std::string& blockHash = "", uint64_t timestamp = 0);

		VOLT_API ~Block();
		
		/*
			Operator overload for assignment operations.
		*/
		VOLT_API void operator=(const Block& block);

		/*
			Generates the hash of the block based on its contents. 
			Returns error ID if an error occurs.
		*/
		VOLT_API ErrorID GenerateBlockHash();

		/*
			Returns the index of the block.
		*/
		VOLT_API const uint32_t& GetIndex() const;

		/*
			Returns the timestamp of the block.
		*/
		VOLT_API const uint64_t& GetTimestamp() const;

		/*
			Returns the nonce value of the block.
		*/
		VOLT_API const uint64_t& GetNonce() const;

		/*
			Returns the block's previous hash.
		*/
		VOLT_API const std::string& GetPreviousBlockHash() const;

		/*
			Returns the hash of the block.
		*/
		VOLT_API const std::string& GetBlockHash() const;

		/*
			Returns the transactions contained in the block.
		*/
		VOLT_API const std::vector<Transaction>& GetTransactions() const;
	};
}

#endif
