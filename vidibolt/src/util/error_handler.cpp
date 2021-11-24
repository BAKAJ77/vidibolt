#include <util/error_handler.h>
#include <openssl/err.h>

namespace Volt
{
	void ErrorHandler::PushError(const std::string& message, ErrorID id, bool throwException)
	{
		this->logStack.push_back({ id, message });
		if (throwException)
		{
			const auto& errorLog = this->GetLatestError();
			std::cout << errorLog.message << std::endl;
			throw std::runtime_error(errorLog.message);
		}
	}

	void ErrorHandler::QueryOpenSSLErrors()
	{
		if (ERR_get_error() != 0)
		{
			char buffer[1024];
			ERR_error_string_n(ERR_get_error(), buffer, sizeof(buffer));
			ERR_clear_error();

			this->logStack.push_back({ ErrorID::OPENSSL_ERROR, buffer });
			
			const auto& errorLog = this->GetLatestError();
			std::cout << errorLog.message << std::endl;
			throw std::runtime_error(errorLog.message);
		}
	}

	void ErrorHandler::FlushStack()
	{
		for (const auto& log : this->logStack)
			std::cout << log.message << std::endl;
	}

	ErrorData ErrorHandler::GetLatestError() const
	{
		if (this->logStack.empty())
			return { ErrorID::NONE, "null" };

		return this->logStack.back();
	}

	std::vector<ErrorData> ErrorHandler::GetStack()
	{
		const std::vector<ErrorData> LOG_STACK = this->logStack;
		this->logStack.clear();
		return LOG_STACK;
	}

	ErrorHandler& ErrorHandler::GetHandler()
	{
		static ErrorHandler handler;
		return handler;
	}
}