#ifndef VIDIBOLT_CORE_TRANSACTION_H
#define VIDIBOLT_CORE_TRANSACTION_H

#include <util/volt_api.h>
#include <util/error_identifier.h>
#include <crypto/ecdsa.h>

#include <boost/json.hpp>
#include <string>
#include <memory>

using namespace boost;

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// An enumeration that represents the type of the transaction
	enum class TransactionType : int
	{
		TRANSFER,
		MINING_REWARD
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// A class that contains the data of a transaction.
	class Transaction
	{
	private:
		class Implementation;
		std::unique_ptr<Implementation> impl;
	public:
		VOLT_API Transaction();
		VOLT_API Transaction(const Transaction& tx);
		VOLT_API Transaction(TransactionType type, uint64_t id, double amount, double fee, uint64_t timestamp, 
			const std::string& senderPK, const std::string& recipientPK, ErrorCode* error = nullptr, 
			const std::string& signiture = "", const std::string& txHash = "");

		VOLT_API ~Transaction();

		// Operator overload for assignment operations.
		VOLT_API void operator=(const Transaction& tx);

		// Signs the transaction with the private key given.
		// An error code is returned in the event of a failure occurring.
		friend extern VOLT_API ErrorCode SignTransaction(Transaction& tx, const ECKeyPair& privKey);

		// Checks whether the given transaction is valid.
		// If the transaction is valid then the value of the error code returned will be 'ErrorID::NONE', else
		// other possible error codes will be returned depending on the type of failure that occurred.
		friend extern VOLT_API ErrorCode VerifyTransaction(const Transaction& tx);

		// Returns the type of the transaction.
		VOLT_API const TransactionType& GetType() const;

		// Returns the ID of the transaction.
		VOLT_API const uint64_t& GetID() const;

		// Returns the amount tranferred in the transaction.
		VOLT_API const double& GetAmount() const;

		// Returns the fee amount paid in the transaction.
		VOLT_API const double& GetFee() const;

		// Returns the timestamp of the transaction.
		VOLT_API const uint64_t& GetTimestamp() const;

		// Returns the public key of the sender in the transaction.
		VOLT_API const std::string& GetSenderKey() const;

		// Returns the public key of the recipient in the transaction.
		VOLT_API const std::string& GetRecipientKey() const;

		// Returns the signiture of the transaction.
		VOLT_API const std::string& GetSigniture() const;

		// Returns the transaction's hash.
		VOLT_API const std::string& GetTxHash() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Returns string containing the transaction data that has been serialized into a JSON format.
	extern VOLT_API std::string SerializeTransaction(const Transaction& tx);

	// Operator overload for checking if both the transaction on the left and right hand side are equal.
	extern VOLT_API bool operator==(const Transaction& lhs, const Transaction& rhs);

	// Operator overload for easier outputting of transaction data to the output stream.
	extern VOLT_API std::ostream& operator<<(std::ostream& stream, const Transaction& tx);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Helper functions for conversions to and from boost json value.

	extern VOLT_API void tag_invoke(json::value_from_tag, json::value& obj, const Transaction& tx);
	extern VOLT_API Transaction tag_invoke(json::value_to_tag<Transaction>, const json::value& obj);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif