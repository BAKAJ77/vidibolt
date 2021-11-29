#include <core/transaction.h>
#include <util/timestamp.h>
#include <crypto/sha256.h>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Transaction::Implementation
	{
	public:
		uint64_t id, amount, timestamp;
		std::string senderPK, recipientPK, signiture;
	public:
		Implementation() :
			id(0), amount(0), timestamp(0)
		{}

		Implementation(uint64_t id, uint64_t amount, uint64_t timestamp, const std::string& senderPK,
			const std::string& recipientPK, const std::string& signiture) :
			id(id), amount(amount), timestamp(timestamp), senderPK(senderPK), recipientPK(recipientPK), signiture(signiture)
		{}

		~Implementation() = default;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Transaction::Transaction() :
		impl(std::make_shared<Implementation>())
	{}

	Transaction::Transaction(uint64_t id, uint64_t amount, uint64_t timestamp, const std::string& senderPK, 
		const std::string& recipientPK, const std::string& signiture) :
		impl(std::make_shared<Implementation>(id, amount, timestamp, senderPK, recipientPK, signiture))
	{}

	const uint64_t& Transaction::GetID() const
	{
		return this->impl->id;
	}

	const uint64_t& Transaction::GetAmount() const
	{
		return this->impl->amount;
	}

	const uint64_t& Transaction::GetTimestamp() const
	{
		return this->impl->timestamp;
	}

	const std::string& Transaction::GetSenderKey() const
	{
		return this->impl->senderPK;
	}

	const std::string& Transaction::GetRecipientKey() const
	{
		return this->impl->recipientPK;
	}

	const std::string& Transaction::GetSigniture() const
	{
		return this->impl->signiture;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ErrorID SignTransaction(Transaction& tx, const ECKeyPair& privKey)
	{
		const std::string txData = std::to_string(tx.GetID()) + std::to_string(tx.GetAmount()) + 
			std::to_string(tx.GetTimestamp()) + tx.GetSenderKey() + tx.GetRecipientKey();

		std::vector<uint8_t> msgBytes = Volt::GetRawString(txData);
		std::vector<uint8_t> outputSigniture;

		ErrorID errorID = Volt::GetSignedSHA256Digest(msgBytes, privKey, outputSigniture);
		if(errorID == ErrorID::NONE)
			tx.impl->signiture = Volt::ConvertByteToHexData(outputSigniture);

		return errorID;
	}

	ErrorID VerifyTransaction(const Transaction& tx, bool& txValid)
	{
		const std::string txData = std::to_string(tx.GetID()) + std::to_string(tx.GetAmount()) +
			std::to_string(tx.GetTimestamp()) + tx.GetSenderKey() + tx.GetRecipientKey();

		std::vector<uint8_t> msgBytes = Volt::GetRawString(txData);
		std::vector<uint8_t> signitureBytes = Volt::ConvertHexToByteData(tx.impl->signiture);
		ErrorID errorID = ErrorID::NONE;
		int isValid = 0;

		ECKeyPair pubKey(tx.GetSenderKey(), std::string(), &errorID);
		auto x = pubKey.GetPublicKeyHex();
		if (errorID == ErrorID::NONE)
			errorID = Volt::VerifySHA256Digest(msgBytes, pubKey, signitureBytes, isValid);

		txValid = (bool)std::max(isValid, 0);
		return errorID;
	}

	std::string SerializeTransaction(const Transaction& tx)
	{
		return json::serialize(json::value_from(tx));
	}

	void tag_invoke(json::value_from_tag, json::value& obj, const Transaction& tx)
	{
		obj = {
			{ "id", tx.impl->id },
			{ "sender", tx.impl->senderPK },
			{ "recipient", tx.impl->recipientPK },
			{ "amount", tx.impl->amount },
			{ "timestamp", tx.impl->timestamp },
			{ "signiture", tx.impl->signiture }
		};
	}

	Transaction tag_invoke(json::value_to_tag<Transaction>, const json::value& obj)
	{
		const json::object& object = obj.as_object();

		return Transaction {
			json::value_to<uint64_t>(obj.at("id")),
			json::value_to<uint64_t>(obj.at("amount")),
			json::value_to<uint64_t>(obj.at("timestamp")),
			json::value_to<std::string>(obj.at("sender")),
			json::value_to<std::string>(obj.at("recipient")),
			json::value_to<std::string>(obj.at("signiture"))
		};
	}

	std::ostream& operator<<(std::ostream& stream, const Transaction& tx)
	{
		stream << json::serialize(json::value_from(tx));
		return stream;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}