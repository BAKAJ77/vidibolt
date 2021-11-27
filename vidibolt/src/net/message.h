#ifndef VIDIBOLT_MESSAGE_H
#define VIDIBOLT_MESSAGE_H

#include <util/volt_api.h>
#include <iostream>
#include <vector>
#include <string>

namespace Volt
{
	///////////////////////////////////////////////////////////////////////////////////////////////

	/*
		An enumeration which holds the purpose of indicating what type of message is being sent/recieved.
	*/
	enum class MessageRepType
	{
		TEST_TYPE
	};

	///////////////////////////////////////////////////////////////////////////////////////////////

	/*
		The message header struct which holds data about the message type and 
		the size of the message payload (the data).
	*/
	struct Header
	{
	public:
		MessageRepType id;
		uint32_t sizeBytes;
	};

	/*
		The entire message struct which holds the message header and the data being transmitted.
	*/
	struct Message
	{
	public:
		Header header;
		std::vector<uint8_t> payload;
	public:
		Message() = default;
		~Message() = default;

		/*
			A operator overload for easier printing of message data to the console.
		*/
		friend std::ostream& operator<<(std::ostream& outStream, const Message& msg)
		{
			outStream << "| Message ID: " << (int)msg.header.id << " | Message Size (Bytes): " << msg.header.sizeBytes << " |";
			return outStream;
		}

		/*
			An operator overload for seamless pushing of data into the message struct container.
			This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
			when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		*/
		template<typename DataType>
		friend Message& operator<<(Message& message, const DataType& data)
		{
			// Make sure the data given is trivially copyable
			static_assert(std::is_standard_layout<DataType>::value, "The data cannot be copied trivially");

			// Get the offset in payload data buffer (a vector of bytes) to insert new data
			// Also resize the buffer so it's able to store the new data
			const size_t OFFSET = message.payloadData.size();
			message.payloadData.resize(OFFSET + sizeof(data));

			// Copy the data into the bytes buffer
			memcpy(message.payloadData.data() + OFFSET, &data, sizeof(data));

			// Update the size (in bytes) counter in the message header
			message.header.sizeBytes = (uint32_t)message.payloadData.size();
			return message; // We return a reference to the message so the operator can be chained
		}

		/*
			An operator overload for seamless extracting of data from the message struct container.
			This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
			when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		*/
		template<typename DataType>
		friend Message& operator>>(Message& message, DataType& data)
		{
			// Make sure the data given is trivially copyable
			static_assert(std::is_standard_layout<DataType>::value, "The data cannot be copied trivially");

			// Get the offset in the payload data buffer where the data will be copied from
			// Copy the data into the reference variable given then shrink the buffer since the data has been pulled
			const size_t OFFSET = message.payloadData.size() - sizeof(data);
			memcpy(&data, message.payloadData.data() + OFFSET, sizeof(data));
			message.payloadData.resize(OFFSET);

			// Update the size (in bytes) counter in the message header
			message.header.sizeBytes = (uint32_t)message.payloadData.size();
			return message;
		}

		/*
			An operator overload for seamless pushing of data into the message struct container.
			This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
			when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		*/
		friend Message& operator<<(Message& message, const std::string& string)
		{
			// Get the offset in payload data buffer (a vector of bytes) to insert new data
			// Also resize the buffer so it's able to store the new data AND the data specifying the length of the string
			const size_t OFFSET = message.payload.size();
			message.payload.resize(OFFSET + string.size() + sizeof(int));

			// Copy the data into the bytes buffer
			const size_t STRING_LEN = string.size();
			memcpy(message.payload.data() + OFFSET, string.data(), string.size());
			memcpy(message.payload.data() + OFFSET + string.size(), &STRING_LEN, sizeof(int));

			// Update the size (in bytes) counter in the message header
			message.header.sizeBytes = (uint32_t)message.payload.size();
			return message; // We return a reference to the message so the operator can be chained
		}

		/*
			An operator overload for seamless extracting of data from the message struct container.
			This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
			when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		*/
		friend Message& operator>>(Message& message, std::string& string)
		{
			// Get the data specifying the length of the string
			const size_t LENGTH_DATA_OFFSET = message.payload.size() - sizeof(int);
			size_t stringLen = 0;
			memcpy(&stringLen, message.payload.data() + LENGTH_DATA_OFFSET, sizeof(int));

			// Get the offset in payload data buffer (a vector of bytes), get the string data
			// then shrink the buffer since the data has been copied from the buffer
			const size_t STRING_DATA_OFFSET = LENGTH_DATA_OFFSET - stringLen;

			string.resize(stringLen);
			memcpy(string.data(), message.payload.data() + STRING_DATA_OFFSET, stringLen);
			message.payload.resize(STRING_DATA_OFFSET);

			// Update the size (in bytes) counter in the message header
			message.header.sizeBytes = (uint32_t)message.payload.size();
			return message;
		}
	};

	/*
		Struct which is basically the same as the Message struct however this one is tied to a specific TCP connection
		with a remote host.
	*/
	struct RecievedMessage
	{
	public:
		uint32_t connectionID;
		Message transmittedMsg;
	public:
		RecievedMessage() :
			connectionID(0)
		{}

		~RecievedMessage() = default;

		/*
			A operator overload for easier printing of message data to the console.
		*/
		friend std::ostream& operator<<(std::ostream& outStream, const RecievedMessage& message)
		{
			outStream << message;
			return outStream;
		}

		/*
			An operator overload for seamless pushing of data into the message struct container.
			This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
			when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		*/
		template<typename DataType>
		friend RecievedMessage& operator<<(RecievedMessage& message, const DataType& data)
		{
			message.transmittedMsg << data;
			return message;
		}

		/*
			An operator overload for seamless extracting of data from the message struct container.
			This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
			when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		*/
		template<typename DataType>
		friend RecievedMessage& operator>>(RecievedMessage& message, DataType& data)
		{
			message.transmittedMsg >> data;
			return message;
		}

		/*
			An operator overload for seamless pushing of data into the message struct container.
			This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
			when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		*/
		friend RecievedMessage& operator<<(RecievedMessage& message, const std::string& string)
		{
			message.transmittedMsg << string;
			return message;
		}

		/*
			An operator overload for seamless extracting of data from the message struct container.
			This struct acts like a stack so data should be pulled in the reverse order that it was pushed for example
			when pushing data it should be "msg << x << y;" so when pulling the data back out it should be "msg >> y >> x;".
		*/
		friend RecievedMessage& operator>>(RecievedMessage& message, std::string& string)
		{
			message.transmittedMsg >> string;
			return message;
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
}

#endif
