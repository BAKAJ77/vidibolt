#ifndef VIDIBOLT_CORE_CHAIN_H
#define VIDIBOLT_CORE_CHAIN_H

#include <util/volt_api.h>
#include <util/ts_unordered_map.h>
#include <crypto/ecdsa.h>
#include <core/block.h>

#include <memory>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// A class that handles chain related operations and the storing of the blockchain.
	class Chain
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	private:
		VOLT_API Chain(const std::vector<Block>& blockChain);
	public:
		VOLT_API Chain();
		VOLT_API Chain(const Chain& chain);

		VOLT_API ~Chain();

		// Operator overload for assignment operations.
		VOLT_API void operator=(const Chain& chain);

		// Creates new chain object initialized with the given existing vector array of blocks (aka blockchain).
		friend extern VOLT_API Chain CreateExistingChain(const std::vector<Block>& blockChain);

		// Checks if the block is valid, then appends it to the stored chain if it's valid.
		// An error code is returned in the event of a failure occurring.
		friend extern VOLT_API ErrorCode PushBlock(Chain& chain, const Block& block);

		// Checks if the entire stored blockchain is valid.
		// If the chain is valid then the value of the error code returned will be 'ErrorID::NONE', else
		// other possible error codes will be returned depending on the type of failure that occurred.
		friend extern VOLT_API ErrorCode VerifyChain(const Chain& chain);

		// Looks through the block chain for the transaction with the given transaction hash.
		// If a transaction is found, it is returned via the second parameter 'returnedTx'.
		// An error code is returned if something goes wrong e.g. the transaction not being found etc.
		friend extern VOLT_API ErrorCode FindTransaction(const Chain& chain, const std::string& txHash, 
			Transaction& returnedTx);

		// Returns the latest block in the chain.
		VOLT_API const Block& GetLatestBlock() const;

		// Returns the block in the chain matching the index specified.
		VOLT_API const Block& GetBlockAtIndexHeight(uint32_t blockIndex) const;

		// Returns a vector array of the entire stored blockchain.
		VOLT_API const std::vector<Block>& GetBlockChain() const;

		// Returns the amount of coins currently being held by a public key address
		VOLT_API double GetAddressBalance(const ECKeyPair& publicKey) const;

		// Returns the current mining reward amount.
		VOLT_API double GetMiningRewardAmount() const;

		// Returns the height index of the latest block in the chain.
		VOLT_API uint32_t GetLatestBlockHeight() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Returns string containing the chain data that has been serialized into a JSON format.
	extern VOLT_API std::string SerializeChain(const Chain& chain);

	// Operator overload for easier outputting of chain data to the output stream.
	extern VOLT_API std::ostream& operator<<(std::ostream& stream, const Chain& chain);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Helper functions for conversions to and from boost json value.

	extern VOLT_API void tag_invoke(json::value_from_tag, json::value& obj, const Chain& chain);
	extern VOLT_API Chain tag_invoke(json::value_to_tag<Chain>, const json::value& obj);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif
