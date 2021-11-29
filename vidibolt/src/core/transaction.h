#ifndef VIDIBOLT_TRANSACTION_H
#define VIDIBOLT_TRANSACTION_H

#include <util/volt_api.h>
#include <util/error_identifier.h>
#include <crypto/ecdsa.h>

#include <boost/json.hpp>
#include <string>
#include <memory>

using namespace boost;

namespace Volt
{
	/*
		A class that contains the data of a transaction.
	*/
	class Transaction
	{
	private:
		class Implementation;
		std::shared_ptr<Implementation> impl;
	public:
		VOLT_API Transaction();
		VOLT_API Transaction(uint64_t id, uint64_t amount, uint64_t timestamp, const std::string& senderPK,
			const std::string& recipientPK, const std::string& signiture = "");

		VOLT_API ~Transaction() = default;

		/*
			Signs the transaction with the private key given.
			An ErrorID is returned if an error occurs.
		*/
		friend extern VOLT_API ErrorID SignTransaction(Transaction& tx, const ECKeyPair& privKey);

		/*
			Checks whether the transaction's signiture is valid. The result is returned via the 'txValid' reference parameter.
			'txValid' is set to TRUE if the signiture is valid, else it's set to FALSE.

			Also an ErrorID is returned if an error occurs.
		*/
		friend extern VOLT_API ErrorID VerifyTransaction(const Transaction& tx, bool& txValid);

		/*
			Returns string containing the transaction data that has been serialized into a JSON format.
		*/
		friend extern VOLT_API std::string SerializeTransaction(const Transaction& tx);

		/*
			Operator overload for easier printing of the transaction data to the output stream.
		*/
		friend extern VOLT_API std::ostream& operator<<(std::ostream& stream, const Transaction& tx);
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Helper functions for conversions to and from boost json value.

		friend extern VOLT_API void tag_invoke(json::value_from_tag, json::value& obj, const Transaction& tx);
		friend extern VOLT_API Transaction tag_invoke(json::value_to_tag<Transaction>, const json::value& obj);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/*
			Returns the ID of the transaction.
		*/
		VOLT_API const uint64_t& GetID() const;

		/*
			Returns the amount tranferred in the transaction.
		*/
		VOLT_API const uint64_t& GetAmount() const;

		/*
			Returns the timestamp of the transaction.
		*/
		VOLT_API const uint64_t& GetTimestamp() const;

		/*
			Returns the public key of the sender in the transaction.
		*/
		VOLT_API const std::string& GetSenderKey() const;

		/*
			Returns the public key of the recipient in the transaction.
		*/
		VOLT_API const std::string& GetRecipientKey() const;

		/*
			Returns the signiture of the transaction.
		*/
		VOLT_API const std::string& GetSigniture() const;
	};
}

#endif