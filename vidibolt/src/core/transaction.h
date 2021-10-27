#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <util/volt_api.h>
#include <crypto/ecdsa.h>

namespace VOLT
{
	enum class TransactionType
	{
		TRANSFER,
		MINING_REWARD
	};

	class VOLT_API Transaction
	{
		friend class Blockchain;
	private:
		uint64_t id;
		TransactionType type;
		std::string senderPublicKey, recipientPublicKey, timestamp, signiture, hash;
		double amount, fee;
	private:
		Transaction();
	public:
		/*
			The argument 'senderKeyPair' should only ever be nullptr if the transaction type is TransactionType::MINING_REWARD.
		*/
		Transaction(const ECKeyPair* senderKeyPair, const ECKeyPair& recipientPublicKey, double amount, TransactionType type);
		~Transaction();

		/*
			When called, verification checks are executed on the transaction signiture.
			If the signiture is valid, 1 is returned, however the signiture is not valid then 0 is returned.
			NOTE: This function should not be used for MINING_REWARD transactions since they have no sender public key
			associated with them so they will not have a signiture.
		*/
		int VerifySigniture() const;

		/*
			Returns all of the transaction data concatonated into one string.
		*/
		std::string GetTxDataString() const;

		/*
			Returns the ID of the transaction.
		*/
		const uint64_t& GetID() const;

		/*
			Returns the fee paid for the transaction.
		*/
		const double& GetFee() const;

		/*
			Returns the type of the transaction.
		*/
		const TransactionType& GetType() const;

		/*
			Returns the amount of coins sent in the transaction.
		*/
		const double& GetAmountSent() const;

		/*
			Returns the public key of the sender of the transaction.
		*/
		const std::string& GetSenderPublicKey() const;

		/*
			Returns the public key of the recipient of the transaction.
		*/
		const std::string& GetRecipientPublicKey() const;

		/*
			Returns the timestamp of when the transaction was created.
		*/
		const std::string& GetTimestamp() const;

		/*
			Returns the hash of the transaction.
		*/
		const std::string& GetHash() const;
	};

	extern bool operator==(const Transaction& tx1, const Transaction& tx2);
}

#endif