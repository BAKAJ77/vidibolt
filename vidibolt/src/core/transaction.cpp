#include <core/transaction.h>
#include <util/random_generation.h>
#include <util/data_conversion.h>
#include <util/log_handler.h>
#include <util/timestamp.h>
#include <crypto/sha256.h>

namespace VOLT
{
	Transaction::Transaction() :
		id(0), amount(0), fee(0), type(TransactionType::TRANSFER)
	{}

	Transaction::Transaction(const ECKeyPair* senderKeyPair, const ECKeyPair& recipientPublicKey, double amount,
		TransactionType type) :
		id(VOLT::GenerateRandomUint64(0, UINT64_MAX)), amount(amount), timestamp(VOLT::GetCurrentTimestamp()), type(type),
		senderPublicKey(senderKeyPair ? senderKeyPair->GetPublicKeyHex() : std::string()), 
		recipientPublicKey(recipientPublicKey.GetPublicKeyHex())
	{
		this->fee = this->type == TransactionType::TRANSFER ? VOLT_RECOMMENDED_TRANSACTION_FEE : 0;

		// Make sure sender key pair is given if transaction type is TransactionType::TRANSFER
		if (!senderKeyPair && type == TransactionType::TRANSFER)
			LogHandler::GetHandler().PushLog("The sender key pair pointer passed is not valid", LogSeverity::FATAL);

		// Concatonate all transaction data into one string then convert the string to raw string data
		const auto TX_DATA = VOLT::GetRawString(std::to_string(this->id) + this->senderPublicKey + 
			std::to_string(this->amount) + std::to_string(this->fee) + this->recipientPublicKey + this->timestamp + 
			std::to_string((int)this->type));

		// Generate transaction hash from transaction data
		const auto TX_HASH = VOLT::GetSHA256Digest(TX_DATA);
		this->hash = VOLT::ConvertByteToHexData(TX_HASH);

		// Generate transaction signiture from transaction data
		if (senderKeyPair && type == TransactionType::TRANSFER)
		{
			const auto SIGNITURE_BYTES = VOLT::GetSignedSHA256Digest(TX_DATA, *senderKeyPair);
			this->signiture = VOLT::ConvertByteToHexData(SIGNITURE_BYTES);
		}
	}

	Transaction::~Transaction() {}

	int Transaction::VerifySigniture() const
	{
		if (this->type == TransactionType::TRANSFER)
		{
			// Concatonate all transaction data into one string then convert the string to raw string data
			const auto TX_DATA = VOLT::GetRawString(std::to_string(this->id) + this->senderPublicKey +
				std::to_string(this->amount) + std::to_string(this->fee) + this->recipientPublicKey + this->timestamp + 
				std::to_string((int)this->type));

			// Convert transaction signiture from hex format to byte format
			const auto SIGNITURE_BYTES = VOLT::ConvertHexToByteData(this->signiture);
			const ECKeyPair PUBLIC_KEY(this->senderPublicKey);
			return VOLT::VerifySHA256Digest(TX_DATA, SIGNITURE_BYTES, PUBLIC_KEY);
		}

		return -10;
	}

	std::string Transaction::GetTxDataString() const
	{
		return std::to_string(this->id) + this->senderPublicKey + std::to_string(this->amount) + std::to_string(this->fee) +
			this->recipientPublicKey + this->timestamp + std::to_string((int)this->type) + this->signiture + this->hash;
	}

	const uint64_t& Transaction::GetID() const
	{
		return this->id;
	}

	const double& Transaction::GetFee() const
	{
		return this->fee;
	}

	const TransactionType& Transaction::GetType() const
	{
		return this->type;
	}

	const double& Transaction::GetAmountSent() const
	{
		return this->amount;
	}

	const std::string& Transaction::GetSenderPublicKey() const
	{
		return this->senderPublicKey;
	}

	const std::string& Transaction::GetRecipientPublicKey() const
	{
		return this->recipientPublicKey;
	}

	const std::string& Transaction::GetTimestamp() const
	{
		return this->timestamp;
	}

	const std::string& Transaction::GetHash() const
	{
		return this->hash;
	}

	bool operator==(const Transaction& tx1, const Transaction& tx2)
	{
		if (tx1.GetHash() == tx2.GetHash())
			return true;

		return false;
	}
}