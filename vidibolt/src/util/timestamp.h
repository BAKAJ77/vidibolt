#ifndef VIDIBOLT_TIMESTAMP_H
#define VIDIBOLT_TIMESTAMP_H

#include <util/volt_api.h>

#include <string>
#include <chrono>

namespace Volt
{
	// Returns a string of the retrieved current date and time data.
	extern VOLT_API std::string GetTimeStampStr();

	// Returns time that has passed (in seconds) since epoch (1st Jan 1970).
	extern VOLT_API uint64_t GetTimeSinceEpoch();
}

#endif
