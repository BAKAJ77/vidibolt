#ifndef VOLT_API_H
#define VOLT_API_H

#ifdef VOLT_PLATFORM_WINDOWS
	#ifdef VOLT_EXPORTS
		#define VOLT_API __declspec(dllexport)
	#else
		#define VOLT_API __declspec(dllimport)
	#endif
#else
	#define VOLT_API
#endif

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char uint8_t;

constexpr uint32_t VOLT_MAX_TRANSACTIONS_PER_BLOCK = 20;
constexpr double VOLT_RECOMMENDED_TRANSACTION_FEE = 0.5, VOLT_MINING_REWARD = 250;

#endif