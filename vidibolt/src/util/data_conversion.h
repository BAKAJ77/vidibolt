#ifndef DATA_CONVERSIONS_H
#define DATA_CONVERSIONS_H

#include <util/volt_api.h>
#include <string>
#include <vector>

namespace VOLT
{
	/*
		Returns vector array of bytes representing the resulting data from converting from a hex format to a byte format.
	*/
	extern VOLT_API std::vector<uint8_t> ConvertHexToByteData(const std::string& hex);

	/*
		Returns resulting hex data string from the conversion of the given byte data to a hexadecimal format.
	*/
	extern VOLT_API std::string ConvertByteToHexData(const std::vector<uint8_t>& bytes);

	/*
		Returns vector of characters extracted from string object.
		NOTE: The unsigned char vector doesn't include the null terminator.
	*/
	extern VOLT_API std::vector<uint8_t> GetRawString(const std::string& str);
}

#endif
