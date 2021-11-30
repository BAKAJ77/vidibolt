#include <core/mem_pool.h>
#include <util/timestamp.h>

namespace Volt
{
	class MemPool::Implementation
	{
	public:
		Deque<Transaction> pendingTxs;
	public:
		Implementation() = default;
		Implementation(const Deque<Transaction>& txs) :
			pendingTxs(txs)
		{}

		~Implementation() = default;
	};

	MemPool::MemPool() :
		impl(std::make_unique<Implementation>())
	{}

	MemPool::MemPool(const MemPool& other) :
		impl(std::make_unique<Implementation>(other.impl->pendingTxs))
	{}

	MemPool::MemPool(const Deque<Transaction>& pendingTxs) :
		impl(std::make_unique<Implementation>(pendingTxs))
	{}

	MemPool::~MemPool() = default;

	void MemPool::operator=(const MemPool& other)
	{
		this->impl = std::make_unique<Implementation>(other.impl->pendingTxs);
	}

	ErrorID PushTransaction(MemPool& pool, const Transaction& tx)
	{
		// Check if transaction is already in the mem pool
		// If it is then return error
		const Deque<Transaction>& txsPool = pool.impl->pendingTxs;
		for (uint32_t i = 0; i < txsPool.GetSize(); i++)
		{
			if (tx == txsPool[i])
				return ErrorID::TRANSACTION_ALREADY_IN_MEMPOOL;
		}

		// Amount being sent in transaction must be larger than 0
		if (tx.GetAmount() == 0)
			return ErrorID::TRANSACTION_AMOUNT_INVALID;

		// A sender and recipient key must be specified
		if (tx.GetSenderKey().empty() || tx.GetRecipientKey().empty())
			return ErrorID::TRANSACTION_KEY_NOT_SPECIFIED;

		// The transaction timestamp must be within 10 mins of current time
		// or the transaction is written off as expired
		constexpr uint64_t timeOutCutoff = 600;
		if (tx.GetTimestamp() < Volt::GetTimeSinceEpoch() - timeOutCutoff)
			return ErrorID::TRANSACTION_EXPIRED;

		// The signiture of the transaction must be valid
		bool signitureValid;
		Volt::VerifyTransaction(tx, signitureValid);

		if (!signitureValid)
			return ErrorID::TRANSACTION_SIGNITURE_INVALID;

		// [TODO]: THIS FUNCTION WILL TAKE A EXTRA PARAM 'const Chain&' TO CHECK IF THE BALANCE
		// OF SENDER IS SUFFICIENT FOR TRANSACTION.

		// The transaction has been deduced as valid so add it to mempool
		pool.impl->pendingTxs.PushBackElement(tx);
		return ErrorID::NONE;
	}

	std::vector<Transaction> PopTransactions(MemPool& pool, uint32_t numTxs)
	{
		// Pop transactions off the mempool queue and insert them intot the transaction vector
		std::vector<Transaction> poppedTxs;
		const uint32_t poolSize = (uint32_t)pool.impl->pendingTxs.GetSize();

		for (uint32_t i = 0; i < numTxs; i++)
		{
			if (i < poolSize)
			{
				poppedTxs.emplace_back(pool.impl->pendingTxs.GetFrontElement());
				pool.impl->pendingTxs.PopFrontElement();
			}
			else
				break;
		}

		return poppedTxs;
	}
}