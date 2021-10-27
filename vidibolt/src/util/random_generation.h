#ifndef RANDOM_GENERATION_H
#define RANDOM_GENERATION_H

#include <util/volt_api.h>

namespace VOLT
{
	/*
		Returns an random integer between the min and max values given.
	*/
	extern VOLT_API int GenerateRandomInt(int min, int max);

	/*
		Returns an random unsigned integer (32 bit) between the min and max values given.
	*/
	extern VOLT_API uint32_t GenerateRandomUint32(uint32_t min, uint32_t max);

	/*
		Returns an random unsigned integer (64 bit) between the min and max values given.
	*/
	extern VOLT_API uint64_t GenerateRandomUint64(uint64_t min, uint64_t max);
}

#endif