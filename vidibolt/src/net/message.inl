#include <net/message.h>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename Ty>
	Message& operator<<(Message& message, const Ty& data)
	{
		// Make sure the data given is trivially copyable
		static_assert(std::is_standard_layout<Ty>::value, "The data cannot be copied trivially");

		// Get the offset in payload data buffer (a vector of bytes) to insert new data
		// Also resize the buffer so it's able to store the new data
		const size_t offset = message.payload.size();
		message.payload.resize(offset + sizeof(data));

		// Copy the data into the bytes buffer
		memcpy(message.payload.data() + offset, &data, sizeof(data));

		// Update the size (in bytes) counter in the message header
		message.header.sizeBytes = (uint32_t)message.payload.size();
		return message; // We return a reference to the message so the operator can be chained
	}

	template<typename Ty>
	Message& operator>>(Message& message, Ty& data)
	{
		// Make sure the data given is trivially copyable
		static_assert(std::is_standard_layout<Ty>::value, "The data cannot be copied trivially");

		// Get the offset in the payload data buffer where the data will be copied from
		// Copy the data into the reference variable given then shrink the buffer since the data has been pulled
		const size_t offset = message.payload.size() - sizeof(data);
		memcpy(&data, message.payload.data() + offset, sizeof(data));
		message.payload.resize(offset);

		// Update the size (in bytes) counter in the message header
		message.header.sizeBytes = (uint32_t)message.payload.size();
		return message;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename Ty>
	RecievedMessage& operator<<(RecievedMessage& message, const Ty& data)
	{
		message.transmittedMsg << data;
		return message;
	}

	template<typename Ty>
	RecievedMessage& operator>>(RecievedMessage& message, Ty& data)
	{
		message.transmittedMsg >> data;
		return message;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	template<typename Ty>
	Ty MessageIterator::GetNextElementData()
	{
		// Make sure the data given is trivially copyable
		static_assert(std::is_standard_layout<Ty>::value, "The data cannot be copied trivially");

		// Get pointer to the data of type 'Ty' at the current offset
		// Update iterator data offset
		const Ty* data = (Ty*)(this->currentOffset - sizeof(Ty));
		this->currentOffset -= sizeof(Ty);

		return *data;
	}

	template<>
	std::string MessageIterator::GetNextElementData<std::string>()
	{
		// Get the length of the string
		int stringLen = 0;
		memcpy(&stringLen, this->currentOffset - sizeof(int), sizeof(int));

		// Copy the string data into the constructed string
		std::string data;
		data.resize(stringLen);
		memcpy(data.data(), this->currentOffset - sizeof(int) - stringLen, stringLen);

		// Update iterator data offset
		this->currentOffset -= (sizeof(int) + stringLen);
		
		return data;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}