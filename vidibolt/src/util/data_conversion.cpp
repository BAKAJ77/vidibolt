#include <util/data_conversion.h>

#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Volt
{
	std::vector<uint8_t> ConvertHexToByteData(const std::string& hex)
	{
		std::vector<uint8_t> resultByteData;
		resultByteData.reserve(hex.size() / 2);
	
		// Convert the hexadecimal string into bytes
		for (size_t i = 0; i < hex.size(); i += 2)
		{
			int leftShiftAmt = 4;
			unsigned char deneryChar = 0;
			
			for (size_t j = 0; j < 2; j++)
			{
				if (hex[i + j] >= '0' && hex[i + j] <= '9')
					deneryChar += (unsigned char)((hex[i + j] - '0') << leftShiftAmt);
				else if (hex[i + j] >= 'A' && hex[i + j] <= 'F')
					deneryChar += (unsigned char)((10 + (hex[i + j] - 'A')) << leftShiftAmt);

				leftShiftAmt -= 4;
			}

			resultByteData.emplace_back(deneryChar);
		}

		return resultByteData;
	}

	std::string ConvertByteToHexData(const std::vector<uint8_t>& bytes)
	{
		std::stringstream ss;
		for (uint32_t i = 0; i < bytes.size(); i++)
			ss << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i];

		// Convert hex to upper case format
		std::string hex = ss.str();
		std::transform(hex.begin(), hex.end(), hex.begin(), ::toupper);

		return hex;
	}

	std::string ConvertUintToHex(const uint64_t& num)
	{
		std::stringstream hexStream;
		hexStream << std::hex << std::setw(2) << std::setfill('0') << num;

		return hexStream.str();
	}

	uint64_t ConvertHexToUint(const std::string& hex)
	{
		return std::stoull(hex, nullptr, 16);
	}

	std::vector<uint8_t> GetRawString(const std::string& str)
	{
		std::vector<uint8_t> rawStringData;
		rawStringData.reserve(str.size());

		for (const char& c : str)
			rawStringData.emplace_back(static_cast<uint8_t>(c));

		return rawStringData;
	}
}