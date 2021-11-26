#ifndef VIDIBOLT_VOLT_API_H
#define VIDIBOLT_VOLT_API_H

#ifdef VOLT_PLATFORM_WINDOWS
	#ifdef VOLT_EXPORTS
		#define VOLT_API __declspec(dllexport)
		#define VOLT_EXPORT VOLT_API
	#else
		#define VOLT_API __declspec(dllimport)
		#define VOLT_EXPORT __declspec(dllexport)
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