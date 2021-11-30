#ifndef VIDIBOLT_CORE_MEMPOOL_H
#define VIDIBOLT_CORE_MEMPOOL_H

#include <util/volt_api.h>
#include <util/ts_deque.h>
#include <core/transaction.h>

#include <vector>
#include <memory>

namespace Volt
{
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

		/*
			Operator overload for assignment operations.
		*/
		VOLT_API void operator=(const MemPool& pool);

		/*
			Pushes given transaction into the mempool.
			Returns an error ID if an error occurs.
		*/
		friend extern VOLT_API ErrorID PushTransaction(MemPool& pool, const Transaction& tx);

		/*
			Pops specified number of transactions from the mempool queue and returns them as a vector of transactions.
			
			Note that the specified number of transactions may not be popped if there is less pending transactions 
			in the mempool than that specified to be popped (via numTxs).
		*/
		friend extern VOLT_API std::vector<Transaction> PopTransactions(MemPool& pool, uint32_t numTxs);
	};
}

#endif
