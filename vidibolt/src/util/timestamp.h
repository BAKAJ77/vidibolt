#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <util/volt_api.h>
#include <string>

namespace VOLT
{
	/* 
		Returns a string of the retrieved current date and time data.
	*/
	VOLT_API std::string GetCurrentTimestamp();
}

#endif
