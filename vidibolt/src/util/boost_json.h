#ifndef VIDIBOLT_BOOST_JSON_H
#define VIDIBOLT_BOOST_JSON_H

#ifdef VOLT_PLATFORM_WINDOWS
	#ifdef VOLT_EXPORTS
		#define BOOST_JSON_DECL       __declspec(dllexport)
		#define BOOST_JSON_CLASS_DECL __declspec(dllexport)
	#else
		#define BOOST_JSON_DECL       __declspec(dllimport)
		#define BOOST_JSON_CLASS_DECL __declspec(dllimport)
	#endif
#else
	#define VOLT_API
#endif

#endif