#include <net/message.h>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Message::Message() = default;

	Message::~Message() = default;

	MessageIterator Message::GetBegin() const
	{
		MessageIterator it(*this);
		it.currentOffset = it.msg.payload.data() + it.msg.header.sizeBytes;
		return it;
	}

	MessageIterator Message::GetEnd() const
	{
		MessageIterator it(*this);
		it.currentOffset = it.msg.payload.data();
		return it;
	}

	std::ostream& operator<<(std::ostream& outStream, const Message& msg)
	{
		outStream << "| Message ID: " << (int)msg.header.id << " | Message Size (Bytes): " << msg.header.sizeBytes << " |";
		return outStream;
	}

	Message& operator<<(Message& message, const std::string& string)
	{
		// Get the offset in payload data buffer (a vector of bytes) to insert new data
		// Also resize the buffer so it's able to store the new data AND the data specifying the length of the string
		const size_t offset = message.payload.size();
		message.payload.resize(offset + string.size() + sizeof(int));

		// Copy the data into the bytes buffer
		const int stringLen = (int)string.size();
		memcpy(message.payload.data() + offset, string.data(), string.size());
		memcpy(message.payload.data() + offset + string.size(), &stringLen, sizeof(int));

		// Update the size (in bytes) counter in the message header
		message.header.sizeBytes = (uint32_t)message.payload.size();
		return message; // We return a reference to the message so the operator can be chained
	}

	Message& operator>>(Message& message, std::string& string)
	{
		// Get the data specifying the length of the string
		const size_t lengthDataOffset = message.payload.size() - sizeof(int);
		int stringLen = 0;
		memcpy(&stringLen, message.payload.data() + lengthDataOffset, sizeof(int));

		// Get the offset in payload data buffer (a vector of bytes), get the string data
		// then shrink the buffer since the data has been copied from the buffer
		const size_t stringDataOffset = lengthDataOffset - stringLen;

		string.resize(stringLen);
		memcpy(string.data(), message.payload.data() + stringDataOffset, stringLen);
		message.payload.resize(stringDataOffset);

		// Update the size (in bytes) counter in the message header
		message.header.sizeBytes = (uint32_t)message.payload.size();
		return message;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	RecievedMessage::RecievedMessage() :
		connectionID(0)
	{}

	RecievedMessage::~RecievedMessage() = default;

	std::ostream& operator<<(std::ostream& outStream, const RecievedMessage& message)
	{
		outStream << message.transmittedMsg;
		return outStream;
	}

	RecievedMessage& operator<<(RecievedMessage& message, const std::string& string)
	{
		message.transmittedMsg << string;
		return message;
	}

	RecievedMessage& operator>>(RecievedMessage& message, std::string& string)
	{
		message.transmittedMsg >> string;
		return message;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	MessageIterator::MessageIterator(const Message& msg) :
		msg(msg), currentOffset(msg.payload.data() + msg.header.sizeBytes)
	{}

	MessageIterator::~MessageIterator() = default;

	bool MessageIterator::HasNextElement() const
	{
		return this->currentOffset > msg.payload.data();
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}