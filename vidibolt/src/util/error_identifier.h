#ifndef VIDIBOLT_ERROR_IDENTIFIER_H
#define VIDIBOLT_ERROR_IDENTIFIER_H

#include <util/volt_api.h>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Enumeration that represents the type of error that occured within the execution of a function.
	enum class ErrorID : uint32_t
	{
		NONE = 0,

		// VOLT related errors
		CONNECTION_NO_LONGER_OPEN = 20000,
		TRANSACTION_ALREADY_IN_MEMPOOL = 20001,
		TRANSACTION_AMOUNT_INVALID = 20002,
		TRANSACTION_KEY_NOT_SPECIFIED = 20003,
		TRANSACTION_EXPIRED = 20004,
		BLOCK_PREVIOUS_HASH_INVALID = 20005,
		BLOCK_TIMESTAMP_INVALID = 20006,
		BLOCK_INDEX_INVALID = 20007,
		GENESIS_BLOCK_INVALID = 20008,
		BLOCK_HASH_INVALID = 20009,
		TRANSACTION_SENDER_BALANCE_INSUFFICIENT = 20010,
		BLOCK_HASH_DIFFICULTY_INSUFFICIENT = 20011,
		CHAIN_EMPTY = 20012,
		NO_HASH_SOLUTION_FOUND_IN_NONCE_RANGE = 20013,
		NONCE_MIN_LARGER_THAN_NONCE_MAX = 20014,
		TRANSACTION_NOT_FOUND = 20015,
		TRANSACTION_HASH_INVALID = 20016,
		RECIEVED_TRANSMITTED_DATA_INVALID = 20017,
		PEER_NODE_ALREADY_IN_PEER_LIST = 20018,
		RECIEVED_DATA_INCOMPLETE = 20019,
		BLOCK_MINING_REWARD_INVALID = 20020,
		NODE_PEER_LIST_EMPTY = 20021,
		NO_SUITABLE_NODE_IN_NODE_PEER_LIST = 20022,
		CLIENT_CONNECTION_OCCUPIED = 20023,
		BALANCE_REQUEST_PEER_SIDE_ERROR = 20024,

		// OpenSSL related error codes
		MESSAGE_EMPTY = 40000,
		DIGEST_UPDATE_FAILURE = 40001,
		DIGEST_OPERATION_FAILURE = 40002,
		CONTEXT_GENERATION_FAILURE = 40003,
		OPERATION_IMPL_FETCH_FAILURE = 40004,
		OPERATION_INIT_FAILURE = 40005,
		ECDSA_PRIVATE_KEY_REQUIRED = 40006,
		ECDSA_PUBLIC_KEY_REQUIRED = 40007,
		ECDSA_PUBLIC_KEY_INVALID = 40008,
		EC_KEY_GROUP_ASSIGNMENT_FAILURE = 40009,
		EC_KEY_ASSIGNMENT_FAILURE = 40010,
		ECDSA_KEY_PAIR_ERROR = 40011,
		ECDSA_KEY_GENERATION_ERROR = 40012,
		EC_PARAMGEN_CURVE_ERROR = 40013,
		SIGNITURE_INVALID = 40014,

		// ASIO error codes
		EOF_ERROR = 2,
		ACCESS_DENIED_ERROR = 10013,
		ADDRESS_FAMILY_NOT_SUPPORTED_ERROR = 10047,
		ADDRESS_IN_USE_ERROR = 10048,
		ALREADY_CONNECTED_ERROR = 10056,
		ALREADY_STARTED_ERROR = 10037,
		BROKEN_PIPE_ERROR = 109,
		CONNECTION_ABORTED_ERROR = 10053,
		CONNECTION_REFUSED_ERROR = 10061,
		CONNECTION_RESET_ERROR = 10054,
		BAD_DESCRIPTOR_ERROR = 10009,
		FAULT_ERROR = 10014,
		HOST_UNREACHABLE_ERROR = 10065,
		IN_PROGRESS_ERROR = 10036,
		INTERRUPTED_ERROR = 10004,
		INVALID_ARGUMENT_ERROR = 10022,
		MESSAGE_SIZE_ERROR = 10040,
		NAME_TOO_LONG_ERROR = 10063,
		NETWORK_DOWN_ERROR = 10050,
		NETWORK_RESET_ERROR = 10052,
		NETWORK_UNREACHABLE_ERROR = 10051,
		NO_DESCRIPTORS_ERROR = 10024,
		NO_BUFFER_SPACE_ERROR = 10055,
		NO_MEMORY_ERROR = 14,
		NO_PERMISSION_ERROR = 5,
		NO_PROTOCOL_OPTION_ERROR = 10042,
		NOT_CONNECTED_ERROR = 10057,
		NOT_SOCKET_ERROR = 10038,
		OPERATION_ABORTED_ERROR = 995,
		OPERATION_NOT_SUPPORTED_ERROR = 10045,
		SHUT_DOWN_ERROR = 10058,
		TIMED_OUT_ERROR = 10060,
		TRY_AGAIN_ERROR = 1237,
		WOULD_BLOCK_ERROR = 10035
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// A class that wraps the ErrorID enumeration for easier error handling.
	class ErrorCode
	{
	private:
		ErrorID code;
	public:
		VOLT_API ErrorCode();
		VOLT_API ErrorCode(ErrorID code);

		VOLT_API ~ErrorCode();

		// Operator overload for assignment of an error code value.
		VOLT_API void operator=(ErrorCode code);

		// Typecast overload for conversions from ErrorCode to a boolean value.
		VOLT_API operator bool() const;

		// Typecast overload for conversions from ErrorCode to a ErrorID value.
		VOLT_API operator ErrorID() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif
