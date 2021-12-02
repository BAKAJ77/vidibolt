#include <core/hash_stats.h>

#include <chrono>
#include <mutex>

namespace Global
{
	const uint64_t* hashCounter;
	double currentHashesPerSecond;

	std::mutex mutex;
	std::chrono::system_clock::duration startTime;
}

namespace Volt
{
	void StartHashRateRecord(const uint64_t& hashCounter)
	{
		std::scoped_lock lock(Global::mutex);

		Global::hashCounter = &hashCounter;
		Global::startTime = std::chrono::system_clock::now().time_since_epoch();
	}

	void EndHashRateRecord()
	{
		if (Global::hashCounter)
		{
			std::scoped_lock lock(Global::mutex);

			// Get the time elapsed since hash rate counter started
			std::chrono::system_clock::duration timeNow = std::chrono::system_clock::now().time_since_epoch();
			double timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(timeNow - Global::startTime).count();

			// Check if a second has passed, if so then calculate h/s
			Global::currentHashesPerSecond = (double)*Global::hashCounter / timeElapsed;
			Global::hashCounter = nullptr;
		}
	}

	double GetCurrentHashesPerSecond()
	{
		std::scoped_lock lock(Global::mutex);
		return Global::currentHashesPerSecond;
	}
}