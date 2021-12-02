#ifndef VIDIBOLT_CORE_MEMPOOL_H
#define VIDIBOLT_CORE_MEMPOOL_H

#include <util/volt_api.h>
#include <util/ts_deque.h>
#include <core/transaction.h>

#include <vector>
#include <memory>

namespace Volt
{
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

		// Pushes given transaction into the mempool.
		// An error code is returned in the event of a failure occurring.
		friend extern VOLT_API ErrorCode PushTransaction(MemPool& pool, const Chain& chain, const Transaction& tx);

		// Pushes given transaction into the mempool.
		// The key pair passed through the parameter 'senderKeyPair' must have both the public and private key of the sender.
		// An error code is returned in the event of a failure occurring.
		friend extern VOLT_API ErrorCode PushTransaction(MemPool& pool, const Chain& chain, TransactionType type,
			double amount, double fee, const ECKeyPair& senderKeyPair, const ECKeyPair& recipientPublicKey);

		// Pops the transaction at the specified index in the queue from the mempool then returns it.
		friend extern VOLT_API Transaction PopTransactionAtIndex(MemPool& pool, size_t index);

		// Pops specified number of transactions from the mempool queue and returns them as a vector of transactions.
		// Note that the specified number of transactions may not be popped if there is less pending transactions
		// in the mempool than that specified to be popped (via numTxs).
		friend extern VOLT_API std::vector<Transaction> PopTransactions(MemPool& pool, uint32_t numTxs);

		// Returns the number of pending transactions in the mempool.
		uint32_t GetPoolSize() const;
	};
}

#endif
