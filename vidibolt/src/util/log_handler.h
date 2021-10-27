#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#include <string>
#include <vector>
#include <util/volt_api.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	Calls passed OpenSSL function and checks for errors from OpenSSL after calling the function.
*/
#define VOLT_OPENSSL_CHECK(opensslFunc) opensslFunc; \
		VOLT::LogHandler::GetHandler().QueryOpenSSLErrors();

/////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace VOLT
{
	/*
		An enumeration representing the severity of a log pushed to the log handler.
	*/
	enum class VOLT_API LogSeverity
	{
		INFO,
		WARNING,
		FATAL
	};

	/*
		Holds data about a created log, it holds the message and severity of the log.
	*/
	struct VOLT_API LogData
	{
		std::string message;
		LogSeverity severity;
	};

	/*
		A singleton class that handles output logs of varying severity.
	*/
	class VOLT_API LogHandler
	{
	private:
		std::vector<LogData> logStack;
		bool throwExceptions;
	private:
		LogHandler();
		~LogHandler();
	public:
		/*
			Sets whether the log hander should throw an exception once a fatal error log has been recieved.
			NOTE: This is enabled by default.
		*/
		void ThrowExceptionOnFatal(bool enable);

		/*
			Inserts output log into the log stack, it can have varying severity: INFO, WARNING or FATAL.
		*/
		void PushLog(const std::string& message, LogSeverity severity);

		/*
			Fetches error(s) thrown by OpenSSL (if any). Any errors found are pushed into log stack.
		*/
		void QueryOpenSSLErrors();

		/*
			Outputs all logs stored in log stack, then proceeding to clear the log stack.
		*/
		void FlushLogStack();

		/*
			Returns the log stack vector containing logs pushed to the log handler. The log stack is also cleared
			once called.
		*/
		std::vector<LogData> GetLogStack();

		/*
			Since this class is a singleton, you will have to use this function to access the methods provided
			by the log handler class.
		*/
		static LogHandler& GetHandler();
	};
}

#endif
