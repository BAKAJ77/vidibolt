#include <core/transaction.h>
#include <util/timestamp.h>
#include <crypto/sha256.h>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Transaction::Implementation
	{
	public:
		TransactionType type;
		uint64_t id, timestamp;
		double amount;

		std::string senderPK, recipientPK, signiture;
	public:
		Implementation() :
			type(TransactionType::TRANSFER), id(0), amount(0), timestamp(0)
		{}

		Implementation(TransactionType type, uint64_t id, double amount, uint64_t timestamp, const std::string& senderPK,
			const std::string& recipientPK, const std::string& signiture) :
			type(type), id(id), amount(amount), timestamp(timestamp), senderPK(senderPK), recipientPK(recipientPK), 
			signiture(signiture)
		{}

		~Implementation() = default;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Transaction::Transaction() :
		impl(std::make_unique<Implementation>())
	{}

	Transaction::Transaction(const Transaction& tx) :
		impl(std::make_unique<Implementation>(tx.GetType(), tx.GetID(), tx.GetAmount(), tx.GetTimestamp(), tx.GetSenderKey(),
			tx.GetRecipientKey(), tx.GetSigniture()))
	{}

	Transaction::Transaction(TransactionType type, uint64_t id, double amount, uint64_t timestamp, 
		const std::string& senderPK, const std::string& recipientPK, const std::string& signiture) :
		impl(std::make_unique<Implementation>(type, id, amount, timestamp, senderPK, recipientPK, signiture))
	{}

	Transaction::~Transaction() = default;

	void Transaction::operator=(const Transaction& tx)
	{
		this->impl = std::make_unique<Implementation>(tx.GetType(), tx.GetID(), tx.GetAmount(), tx.GetTimestamp(), 
			tx.GetSenderKey(), tx.GetRecipientKey(), tx.GetSigniture());
	}

	const TransactionType& Transaction::GetType() const
	{
		return this->impl->type;
	}

	const uint64_t& Transaction::GetID() const
	{
		return this->impl->id;
	}

	const double& Transaction::GetAmount() const
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

	ErrorCode SignTransaction(Transaction& tx, const ECKeyPair& privKey)
	{
		// Combine all transaction data into one string of data
		const std::string txData = std::to_string(tx.GetID()) + std::to_string(tx.GetAmount()) + 
			std::to_string(tx.GetTimestamp()) + tx.GetSenderKey() + tx.GetRecipientKey();

		// Execute the transaction signing operation
		std::vector<uint8_t> msgBytes = Volt::GetRawString(txData);
		std::vector<uint8_t> outputSigniture;

		ErrorCode error = Volt::GetSignedSHA256Digest(msgBytes, privKey, outputSigniture);
		if (!error) // If no error occurred, then store the generated signiture
			tx.impl->signiture = Volt::ConvertByteToHexData(outputSigniture);

		return error;
	}

	ErrorCode VerifyTransaction(const Transaction& tx)
	{
		ErrorCode error;

		if (tx.GetType() != TransactionType::MINING_REWARD)
		{
			// Combine all transaction data into one string of data
			const std::string txData = std::to_string(tx.GetID()) + std::to_string(tx.GetAmount()) +
				std::to_string(tx.GetTimestamp()) + tx.GetSenderKey() + tx.GetRecipientKey();

			// Do transaction signiture verification process
			std::vector<uint8_t> msgBytes = Volt::GetRawString(txData);
			std::vector<uint8_t> signitureBytes = Volt::ConvertHexToByteData(tx.impl->signiture);

			ECKeyPair pubKey(tx.GetSenderKey(), std::string(), &error);
			if (!error) // If no error occurred when creating key pair object, then do signiture verification
				error = Volt::VerifySHA256Digest(msgBytes, pubKey, signitureBytes);
		}

		return error;
	}

	std::string SerializeTransaction(const Transaction& tx)
	{
		return json::serialize(json::value_from(tx));
	}

	void tag_invoke(json::value_from_tag, json::value& obj, const Transaction& tx)
	{
		obj = {
			{ "type", (uint32_t)tx.impl->type },
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
			(TransactionType)json::value_to<uint32_t>(obj.at("type")),
			json::value_to<uint64_t>(obj.at("id")),
			json::value_to<double>(obj.at("amount")),
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

	bool operator==(const Transaction& lhs, const Transaction& rhs)
	{
		return lhs.GetType() == rhs.GetType() &&
			lhs.GetID() == rhs.GetID() &&
			lhs.GetAmount() == rhs.GetAmount() &&
			lhs.GetTimestamp() == rhs.GetTimestamp() &&
			lhs.GetSenderKey() == rhs.GetSenderKey() &&
			lhs.GetRecipientKey() == rhs.GetRecipientKey() &&
			lhs.GetSigniture() == rhs.GetSigniture();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}