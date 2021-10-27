#include <util/random_generation.h>
#include <random>

namespace VOLT
{
	int GenerateRandomInt(int min, int max)
	{
		std::random_device device;
		std::mt19937 randGen(device());
		std::uniform_int_distribution<int> randDistrib(min, max);

		return randDistrib(randGen);
	}

	uint32_t GenerateRandomUint32(uint32_t min, uint32_t max)
	{
		std::random_device device;
		std::mt19937 randGen(device());
		std::uniform_int_distribution<uint32_t> randDistrib(min, max);

		return randDistrib(randGen);
	}

	uint64_t GenerateRandomUint64(uint64_t min, uint64_t max)
	{
		std::random_device device;
		std::mt19937_64 randGen(device());
		std::uniform_int_distribution<uint64_t> randDistrib(min, max);

		return randDistrib(randGen);
	}
}