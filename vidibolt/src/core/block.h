#ifndef VIDIBOLT_CORE_BLOCK_H
#define VIDIBOLT_CORE_BLOCK_H

#include <util/volt_api.h>
#include <core/transaction.h>

#include <string>
#include <vector>
#include <memory>

namespace Volt
{
	class Chain;

	// A class that contains data belonging to a block.
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
		
		// Operator overload for assignment operations.
		VOLT_API void operator=(const Block& block);

		// Checks if the block is valid or not.
		// If the block is valid then the value of the error code returned will be 'ErrorID::NONE', else
		// other possible error codes will be returned depending on the type of failure that occurred.
		friend extern VOLT_API ErrorCode VerifyBlock(const Block& block, const Chain& chain);

		// Generates the hash of the block based on its contents. 
		// An error code is returned in the event of a failure occurring.
		VOLT_API ErrorCode GenerateBlockHash(std::string& outputBlockHash) const;

		// Returns the index of the block.
		VOLT_API const uint32_t& GetIndex() const;

		// Returns the timestamp of the block.
		VOLT_API const uint64_t& GetTimestamp() const;

		// Returns the nonce value of the block.
		VOLT_API const uint64_t& GetNonce() const;

		// Returns the block's previous hash.
		VOLT_API const std::string& GetPreviousBlockHash() const;

		// Returns the hash of the block.
		VOLT_API const std::string& GetBlockHash() const;

		// Returns the transactions contained in the block.
		VOLT_API const std::vector<Transaction>& GetTransactions() const;
	};

	// Returns the (hard-coded) genesis block of the entire blockchain.
	extern VOLT_API Block GetGenesisBlock();

	// Operator overload for checking if both the transaction on the left and right hand side are equal.
	extern VOLT_API bool operator==(const Block& lhs, const Block& rhs);

	// Operator overload for checking if both the transaction on the left and right hand side are not equal.
	extern VOLT_API bool operator!=(const Block& lhs, const Block& rhs);
}

#endif
