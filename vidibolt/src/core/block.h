#ifndef VIDIBOLT_CORE_BLOCK_H
#define VIDIBOLT_CORE_BLOCK_H

#include <util/volt_api.h>
#include <core/transaction.h>

#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace Volt
{
	class Chain;
	class MemPool;

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
			uint64_t difficulty, const std::string& blockHash = "", uint64_t timestamp = 0, uint64_t nonce = 0);

		VOLT_API ~Block();
		
		// Operator overload for assignment operations.
		VOLT_API void operator=(const Block& block);

		// Checks if the block is valid or not.
		// If the block is valid then the value of the error code returned will be 'ErrorID::NONE', else
		// other possible error codes will be returned depending on the type of failure that occurred.
		friend extern VOLT_API ErrorCode VerifyBlock(const Block& block, const Chain& chain);

		// Creates a new block and fills it with transactions fetched from the mempool, the created block is then returned.
		// Note that this function does NOT perform any proof-of-work on the block, it only creates and initializes it with
		// data.
		// 
		// Also, note that the mining reward will only be inserted into the block if the miner public key is provided, 
		// if it's not provided then no mining reward transaction will be included in the block.
		friend extern VOLT_API Block CreateBlock(MemPool& pool, const Chain& chain, uint64_t difficulty,
			const ECKeyPair* minerPublicKey = nullptr, std::function<bool(const Transaction&)> txHandler = nullptr);

		// Fills the block with transactions from the mempool then does proof-of-work to validate the block.
		// The successfully mined block is returned via the second parameter 'minedBlock'.
		// 
		// Transactions picked can be customised by passing a function via the last parameter 'txHandler',
		// when the passed function returns TRUE then the transaction is added to the block.
		// 
		// An error code is returned in the event of a failure occurring.
		friend extern VOLT_API ErrorCode MineNextBlock(MemPool& pool, Block& minedBlock, const Chain& chain,
			uint64_t difficulty, const ECKeyPair& minerPublicKey, std::function<bool(const Transaction&)> txHandler = nullptr);

		// Only does proof-of-work on the block assuming that the block data has already been initialized (this should be
		// done via the CreateBlock() method).
		// 
		// The fifth and sixth parameter ('nonceStart' and 'nonceEnd' respectively) are for specifying at which nonce the
		// mining of the block should start and where it should end e.g. if we set 'nonceStart' to 50 and 'nonceEnd' to 100,
		// generation of hashes for the block will start at an initial nonce value of 50 then the generation of hashes will
		// end when either a hash satisfying the difficulty is found or when the nonce value reaches a value of 100.
		// 
		// The block passed via 'block' will be modified with the block hash and timestamp being assigned if 
		// a valid hash is successfully found.
		// 
		// An error code is returned in the event of a failure occurring.
		friend extern VOLT_API ErrorCode MineNextBlock(Block& block, const Chain& chain, uint64_t difficulty, 
			uint64_t nonceStart = 0, uint64_t nonceEnd = UINT64_MAX);

		// Generates the hash of the block based on its contents. 
		// An error code is returned in the event of a failure occurring.
		VOLT_API ErrorCode GenerateBlockHash(std::string& outputBlockHash) const;

		// Returns the index of the block.
		VOLT_API const uint32_t& GetIndex() const;

		// Returns the timestamp of the block.
		VOLT_API const uint64_t& GetTimestamp() const;

		// Returns the mining difficulty of the block
		VOLT_API const uint64_t& GetDifficulty() const;

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
