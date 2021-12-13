#ifndef VIDIBOLT_CORE_MEMPOOL_H
#define VIDIBOLT_CORE_MEMPOOL_H

#include <util/volt_api.h>
#include <util/ts_deque.h>
#include <core/transaction.h>

#include <vector>
#include <memory>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Chain;

	// A class for containing pending transactions.
	class MemPool
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	public:
		VOLT_API MemPool();
		VOLT_API MemPool(const MemPool& pool);
		VOLT_API MemPool(const Deque<Transaction>& pendingTxs);

		VOLT_API ~MemPool();

		// Operator overload for assignment operations.
		VOLT_API void operator=(const MemPool& pool);

		// Creates and initializes new regular transfer transaction, the created and initialized transaction object is returned.
		// Potential errors are returned via the last parameter 'error'.
		friend extern VOLT_API Transaction CreateNewTransaction(double amount, double fee, const ECKeyPair& senderKeyPair,
			const ECKeyPair& recipientPublicKey, ErrorCode *error = nullptr);

		// Pushes given transaction into the mempool, also note that the transaction given must be signed and valid.
		// An error code is returned in the event of a failure occurring.
		friend extern VOLT_API ErrorCode PushTransaction(MemPool& pool, const Chain& chain, const Transaction& tx);

		// Pops the transaction at the specified index in the queue from the mempool then returns it.
		friend extern VOLT_API Transaction PopTransactionAtIndex(MemPool& pool, size_t index);

		// Pops specified number of transactions from the mempool queue and returns them as a vector array of transactions.
		// Note that the specified number of transactions may not be popped if there is less pending transactions
		// in the mempool than that specified to be popped (via numTxs).
		friend extern VOLT_API std::vector<Transaction> PopTransactions(MemPool& pool, uint32_t numTxs);

		// Looks through the mempool for the pending transaction matching the given transaction hash.
		// If the transaction is found, it is returned via the second parameter 'returnedTx'.
		// An error code is returned if something goes wrong e.g. the transaction not being found etc.
		friend extern VOLT_API ErrorCode FindTransaction(const MemPool& pool, const std::string& txHash,
			Transaction& returnedTx);

		// Returns the number of pending transactions in the mempool.
		uint32_t GetPoolSize() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif
