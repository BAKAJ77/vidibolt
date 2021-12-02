#ifndef VIDIBOLT_CORE_HASH_STATS_H
#define VIDIBOLT_CORE_HASH_STATS_H

#include <util/volt_api.h>

namespace Volt
{
	// Starts recording the hash rate via the hash counter provided.
	extern VOLT_API void StartHashRateRecord(const uint64_t& hashCounter);

	// Ends hash rate recording and does small calculations to get the final hash rate.
	extern VOLT_API void EndHashRateRecord();

	// Returns the latest recorded hash rate.
	extern VOLT_API double GetCurrentHashesPerSecond();
}

#endif
