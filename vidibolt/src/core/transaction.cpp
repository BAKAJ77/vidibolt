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
		double amount, fee;

		std::string senderPK, recipientPK, signiture, txHash;
	public:
		Implementation() :
			type(TransactionType::TRANSFER), id(0), amount(0), timestamp(0), fee(0)
		{}

		Implementation(TransactionType type, uint64_t id, double amount, double fee, uint64_t timestamp, 
			const std::string& senderPK, const std::string& recipientPK, const std::string& signiture, 
			const std::string& txHash) :
			type(type), id(id), amount(amount), fee(fee), timestamp(timestamp), senderPK(senderPK), recipientPK(recipientPK), 
			signiture(signiture), txHash(txHash)
		{}

		~Implementation() = default;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string Transaction::GetTransactionDataStr() const
	{
		return std::to_string(this->GetID()) + std::to_string(this->GetAmount()) + std::to_string(this->GetTimestamp()) + 
			this->GetSenderKey() + this->GetRecipientKey();
	}

	ErrorCode Transaction::GenerateTxHash()
	{
		// Combine all transaction data into one string of data
		const std::string txData = this->GetTransactionDataStr();
		const std::vector<uint8_t> txDataBytes = Volt::GetRawString(txData);

		// Get the hash from the transaction data then get the hash from the generated
		// So basically we are doing the hash operation twice
		std::vector<uint8_t> outputHash;
		ErrorCode error = Volt::GetSHA256Digest(txDataBytes, outputHash);
		if (!error)
		{
			error = Volt::GetSHA256Digest(outputHash, outputHash); // Get hash from previous generated hash
			if (!error)
			{
				// Convert timestamp string to hex
				std::string timestampHex = Volt::ConvertUintToHex(this->GetTimestamp());
				std::transform(timestampHex.begin(), timestampHex.end(), timestampHex.begin(), ::toupper);

				// Append the converted timestamp hex string to the hash string 
				this->impl->txHash = Volt::ConvertByteToHexData(outputHash) + timestampHex;
			}
		}

		return error;
	}

	Transaction::Transaction() :
		impl(std::make_unique<Implementation>())
	{}

	Transaction::Transaction(const Transaction& tx) :
		impl(std::make_unique<Implementation>(tx.GetType(), tx.GetID(), tx.GetAmount(), tx.GetFee(), tx.GetTimestamp(), 
			tx.GetSenderKey(), tx.GetRecipientKey(), tx.GetSigniture(), tx.GetTxHash()))
	{}

	Transaction::Transaction(TransactionType type, uint64_t id, double amount, double fee, uint64_t timestamp, 
		const std::string& senderPK, const std::string& recipientPK, ErrorCode* error, const std::string& signiture,
		const std::string& txHash) :
		impl(std::make_unique<Implementation>(type, id, amount, fee, timestamp, senderPK, recipientPK, signiture, txHash))
	{
		if (txHash.empty())
			error ? *error = this->GenerateTxHash() : this->GenerateTxHash();
	}

	Transaction::~Transaction() = default;

	void Transaction::operator=(const Transaction& tx)
	{
		this->impl = std::make_unique<Implementation>(tx.GetType(), tx.GetID(), tx.GetAmount(), tx.GetFee(), tx.GetTimestamp(), 
			tx.GetSenderKey(), tx.GetRecipientKey(), tx.GetSigniture(), tx.GetTxHash());
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

	const double& Transaction::GetFee() const
	{
		return this->impl->fee;
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

	const std::string& Transaction::GetTxHash() const
	{
		return this->impl->txHash;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ErrorCode SignTransaction(Transaction& tx, const ECKeyPair& privKey)
	{
		// Do the transaction signing operation
		std::vector<uint8_t> txHashBytes = Volt::ConvertHexToByteData(tx.GetTxHash().substr(0, SHA_256_DIGEST_LENGTH_HEX));
		std::vector<uint8_t> outputSigniture;

		ErrorCode error = Volt::GetSignedSHA256Digest(txHashBytes, privKey, outputSigniture);
		if (!error)
			tx.impl->signiture = Volt::ConvertByteToHexData(outputSigniture);

		return error;
	}

	ErrorCode VerifyTransaction(const Transaction& tx)
	{
		ErrorCode error;
		if (tx.GetType() != TransactionType::MINING_REWARD)
		{
			// Do transaction signiture verification process
			std::vector<uint8_t> txHashBytes = Volt::ConvertHexToByteData(tx.GetTxHash().substr(0, SHA_256_DIGEST_LENGTH_HEX));
			std::vector<uint8_t> signitureBytes = Volt::ConvertHexToByteData(tx.impl->signiture);

			ECKeyPair pubKey(tx.GetSenderKey(), std::string(), &error);
			if (!error) // If no error occurred when creating key pair object, then do signiture verification
				error = Volt::VerifySHA256Digest(txHashBytes, pubKey, signitureBytes);
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
			{ "fee", tx.impl->fee },
			{ "timestamp", tx.impl->timestamp },
			{ "signiture", tx.impl->signiture },
			{ "hash", tx.impl->txHash }
		};
	}

	Transaction tag_invoke(json::value_to_tag<Transaction>, const json::value& obj)
	{
		const json::object& object = obj.as_object();

		return Transaction {
			(TransactionType)json::value_to<uint32_t>(obj.at("type")),
			json::value_to<uint64_t>(obj.at("id")),
			json::value_to<double>(obj.at("amount")),
			json::value_to<double>(obj.at("fee")),
			json::value_to<uint64_t>(obj.at("timestamp")),
			json::value_to<std::string>(obj.at("sender")),
			json::value_to<std::string>(obj.at("recipient")),
			nullptr,
			json::value_to<std::string>(obj.at("signiture")),
			json::value_to<std::string>(obj.at("hash"))
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
			lhs.GetFee() == rhs.GetFee() &&
			lhs.GetTimestamp() == rhs.GetTimestamp() &&
			lhs.GetSenderKey() == rhs.GetSenderKey() &&
			lhs.GetRecipientKey() == rhs.GetRecipientKey() &&
			lhs.GetSigniture() == rhs.GetSigniture() &&
			lhs.GetTxHash() == rhs.GetTxHash();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}