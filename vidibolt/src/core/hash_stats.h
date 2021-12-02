#ifndef VIDIBOLT_CORE_HASH_STATS_H
#define VIDIBOLT_CORE_HASH_STATS_H

#include <util/volt_api.h>

namespace Volt
{
	extern VOLT_API void StartHashRateRecord(const uint64_t& hashCounter);
	extern VOLT_API void EndHashRateRecord();

	extern VOLT_API double GetCurrentHashesPerSecond();
}

#endif
