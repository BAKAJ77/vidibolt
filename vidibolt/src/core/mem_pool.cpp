#include <core/mem_pool.h>
#include <core/chain.h>
#include <util/timestamp.h>
#include <util/random_generation.h>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	MemPool::MemPool() :
		impl(std::make_unique<Implementation>())
	{}

	MemPool::MemPool(const MemPool& pool) :
		impl(std::make_unique<Implementation>(pool.impl->pendingTxs))
	{}

	MemPool::MemPool(const Deque<Transaction>& pendingTxs) :
		impl(std::make_unique<Implementation>(pendingTxs))
	{}

	MemPool::~MemPool() = default;

	void MemPool::operator=(const MemPool& pool)
	{
		this->impl = std::make_unique<Implementation>(pool.impl->pendingTxs);
	}

	uint32_t MemPool::GetPoolSize() const
	{
		return (uint32_t)this->impl->pendingTxs.GetSize();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Transaction CreateNewTransaction(double amount, double fee, const ECKeyPair& senderKeyPair, 
		const ECKeyPair& recipientPublicKey, ErrorCode* error)
	{
		Transaction tx(TransactionType::TRANSFER, Volt::GenerateRandomUint64(0, UINT64_MAX), amount, fee,
			Volt::GetTimeSinceEpoch(), senderKeyPair.GetPublicKeyHex(), recipientPublicKey.GetPublicKeyHex(), error);

		error ? *error = Volt::SignTransaction(tx, senderKeyPair) : Volt::SignTransaction(tx, senderKeyPair);
		return tx;
	}

	ErrorCode PushTransaction(MemPool& pool, const Chain& chain, const Transaction& tx)
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

		// The sender must have a sufficient balance to execute transaction
		ErrorCode keyPairError;
		ECKeyPair publicKey(tx.GetSenderKey(), std::string(), &keyPairError);
		if (keyPairError)
			return keyPairError;

		if (chain.GetAddressBalance(publicKey) < (tx.GetAmount() + tx.GetFee()))
			return ErrorID::TRANSACTION_SENDER_BALANCE_INSUFFICIENT;

		// The transaction timestamp must be within 10 mins of current time
		// or the transaction is written off as expired
		constexpr uint64_t timeOutCutoff = 600;
		if (tx.GetTimestamp() < Volt::GetTimeSinceEpoch() - timeOutCutoff)
			return ErrorID::TRANSACTION_EXPIRED;

		// The signiture of the transaction must be valid
		ErrorCode error = Volt::VerifyTransaction(tx);
		if (error)
			return error;

		// The transaction has been deduced as valid so add it to mempool
		pool.impl->pendingTxs.PushBackElement(tx);
		return ErrorID::NONE;
	}

	Transaction PopTransactionAtIndex(MemPool& pool, size_t index)
	{
		const Transaction tx = pool.impl->pendingTxs[index];
		pool.impl->pendingTxs.PopElementAtIndex(index);
		return tx;
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
				const Transaction& tx = pool.impl->pendingTxs.GetFrontElement();

				poppedTxs.emplace_back(tx);
				pool.impl->pendingTxs.PopFrontElement();
			}
			else
				break;
		}

		return poppedTxs;
	}

	ErrorCode FindTransaction(const MemPool& pool, const std::string& txHash, Transaction& returnedTx)
	{
		for (uint32_t index = 0; index < pool.GetPoolSize(); index++)
		{
			const Transaction& tx = pool.impl->pendingTxs[index];
			if (tx.GetTxHash() == txHash)
			{
				returnedTx = tx;
				return ErrorID::NONE;
			}
		}

		return ErrorID::TRANSACTION_NOT_FOUND;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}