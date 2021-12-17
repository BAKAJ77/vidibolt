#ifndef VIDIBOLT_MESSAGE_H
#define VIDIBOLT_MESSAGE_H

#include <util/volt_api.h>
#include <iostream>
#include <vector>
#include <string>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct Message;

	// A class that handles iteration through data in message object.
	// Note that it iterates through data elements in message object from back to from since data in message objects
	// are stored in a stack (aka F-I-L-O) format.
	class MessageIterator
	{
		friend struct Message;
	private:
		const Message& msg;
		const uint8_t* currentOffset;
	public:
		VOLT_API MessageIterator(const Message& msg);
		VOLT_API ~MessageIterator();

		// Returns the next data element in message object, the type of the data is assumed to be 'Ty'.
		template<typename Ty>
		VOLT_EXPORT Ty GetNextElementData();

		// Returns the next data element in message object, the type of the data is assumed to be a string.
		template<>
		VOLT_EXPORT std::string GetNextElementData<std::string>();

		// Returns TRUE if all data in message object hasn't been iterated through, else FALSE is returned.
		VOLT_API bool HasNextElement() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// An enumeration which holds the purpose of indicating what type of message is being sent/recieved.
	enum class MessageRepType : uint32_t
	{
		// Message IDs related to adding other peer nodes to your node's peer list.
		NODE_GUID_REQUEST,
		NODE_GUID_RESPONSE,

		// Message IDs related to requesting the balance held by a specified address from other peer nodes.
		ADDRESS_AMOUNT_REQUEST,
		ADDRESS_AMOUNT_RESPONSE,
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// The message header struct which holds data about the message type and the size of the message payload(the data).
	struct Header
	{
	public:
		MessageRepType id;
		uint32_t sizeBytes = 0;
		uint64_t networkID = 0;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// The entire message struct which holds the message header and the data being transmitted.
	struct Message
	{
	public:
		Header header;
		std::vector<uint8_t> payload;
	public:
		VOLT_API Message();
		VOLT_API ~Message();

		// A operator overload for easier printing of message data to the console.
		friend extern VOLT_API std::ostream& operator<<(std::ostream& outStream, const Message& msg);

		// An operator overload for seamless pushing of data into the message struct container.
		// This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
		// when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		template<typename Ty> friend extern VOLT_EXPORT Message& operator<<(Message& message, const Ty& data);

		// An operator overload for seamless extracting of data from the message struct container.
		// This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
		// when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		template<typename Ty> friend extern VOLT_EXPORT Message& operator>>(Message& message, Ty& data);

		// An operator overload for seamless pushing of data into the message struct container.
		// This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
		// when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		friend extern VOLT_API Message& operator<<(Message& message, const std::string& string);

		// An operator overload for seamless extracting of data from the message struct container.
		// This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
		// when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		friend extern VOLT_API Message& operator>>(Message& message, std::string& string);

		// Returns iterator pointing to the start of the message payload data array. 
		VOLT_API MessageIterator GetBegin() const;

		// Returns iterator pointing to the end of the message payload data array.
		VOLT_API MessageIterator GetEnd() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Struct which is basically the same as the Message struct however this one is tied to a specific TCP connection
	// with a remote host.
	struct RecievedMessage
	{
	public:
		uint32_t connectionID;
		std::string senderAddress; // The IPv4 address of the sender

		Message transmittedMsg;
	public:
		VOLT_API RecievedMessage();
		VOLT_API ~RecievedMessage();

		// A operator overload for easier printing of message data to the console.
		friend extern VOLT_API std::ostream& operator<<(std::ostream& outStream, const RecievedMessage& message);

		// An operator overload for seamless pushing of data into the message struct container.
		// This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
		// when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		template<typename Ty> friend extern VOLT_EXPORT RecievedMessage& operator<<(RecievedMessage& message, const Ty& data);

		// An operator overload for seamless extracting of data from the message struct container.
		// This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
		// when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		template<typename Ty> friend extern VOLT_EXPORT RecievedMessage& operator>>(RecievedMessage& message, Ty& data);

		// An operator overload for seamless pushing of data into the message struct container.
		// This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
		// when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		friend extern VOLT_API RecievedMessage& operator<<(RecievedMessage& message, const std::string& string);

		// An operator overload for seamless extracting of data from the message struct container.
		// This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
		// when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		friend extern VOLT_API RecievedMessage& operator>>(RecievedMessage& message, std::string& string);
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#include <net/message.inl>

#endif
