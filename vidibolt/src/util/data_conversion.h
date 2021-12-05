#ifndef VIDIBOLT_DATA_CONVERSIONS_H
#define VIDIBOLT_DATA_CONVERSIONS_H

#include <util/volt_api.h>
#include <string>
#include <vector>

namespace Volt
{
	// Returns vector array of bytes representing the resulting data from converting from a hex format to a byte format.
	extern VOLT_API std::vector<uint8_t> ConvertHexToByteData(const std::string& hex);

	// Returns the resulting hex data string from the conversion of the given byte data to hexadecimal format.
	extern VOLT_API std::string ConvertByteToHexData(const std::vector<uint8_t>& bytes);

	// Returns the resulting hex data string from the conversion of the given unsigned integer to hexadecimal format.
	extern VOLT_API std::string ConvertUintToHex(const uint64_t& num);

	// Returns the resulting unsigned integer retrieved from the hex to uint conversion
	extern VOLT_API uint64_t ConvertHexToUint(const std::string& hex);

	// Returns vector of characters extracted from string object.
    // NOTE: The unsigned char vector doesn't include the null terminator.
	extern VOLT_API std::vector<uint8_t> GetRawString(const std::string& str);
}

#endif
