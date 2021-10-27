#include <util/log_handler.h>
#include <openssl/err.h>
#include <iostream>

namespace VOLT
{
	LogHandler::LogHandler() :
		throwExceptions(true)
	{}

	LogHandler::~LogHandler() {}

	void LogHandler::ThrowExceptionOnFatal(bool enable)
	{
		this->throwExceptions = enable;
	}

	void LogHandler::PushLog(const std::string& message, LogSeverity severity)
	{
		this->logStack.push_back({ message, severity });
		if (severity == LogSeverity::FATAL && this->throwExceptions)
		{
			this->FlushLogStack();
			throw std::runtime_error("A fatal error has occured, check console output window for more info");
		}
	}

	void LogHandler::QueryOpenSSLErrors()
	{
		if (ERR_get_error() != 0)
		{
			char buffer[1024];
			ERR_error_string_n(ERR_get_error(), buffer, sizeof(buffer));
			ERR_clear_error();

			this->logStack.push_back({ buffer, LogSeverity::FATAL });
			if (this->throwExceptions)
			{
				this->FlushLogStack();
				throw std::runtime_error("A fatal error has occured, check console output window for more info");
			}
		}
	}

	void LogHandler::FlushLogStack()
	{
		for (const auto& log : this->logStack)
			std::cout << log.message << std::endl;
	}

	std::vector<LogData> LogHandler::GetLogStack()
	{
		const std::vector<LogData> LOG_STACK = this->logStack;
		this->logStack.clear();

		return LOG_STACK;
	}

	LogHandler& LogHandler::GetHandler()
	{
		static LogHandler handler;
		return handler;
	}
}