#ifndef VIDIBOLT_ERROR_HANDLER_H
#define VIDIBOLT_ERROR_HANDLER_H

#include <string>
#include <vector>
#include <iostream>
#include <util/volt_api.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	Calls given OpenSSL function and checks for errors from OpenSSL after calling the function.
*/
#define VOLT_OPENSSL_CHECK(opensslFunc) opensslFunc; \
		Volt::ErrorHandler::GetHandler().QueryOpenSSLErrors();

/*
	Calls given VOLT function then checks for and outputs errors silently thrown (errors which didn't result in an exception
	being thrown).
*/
#define VOLT_ERROR_CHECK(volt_func) volt_func; \
		if(Volt::ErrorHandler::GetHandler().GetLatestError().id != Volt::ErrorID::NONE) \
			std::cout << Volt::ErrorHandler::GetHandler().GetLatestError().message << std::endl;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Volt
{
	enum class ErrorID
	{
		NONE,
		SHA256_DIGEST_ERROR,
		ECDSA_KEY_PAIR_ERROR,
		OPENSSL_ERROR,
		NETWORK_TRANSMIT_ERROR,
		NETWORK_RECIEVE_ERROR,
		CONNECTION_INIT_ERROR,
		REMOTE_DISCONNECT_ERROR
	};

	struct ErrorData
	{
		ErrorID id;
		std::string message;
	};

	/*
		A singleton class that handles errors that has been thrown.
	*/
	class VOLT_API ErrorHandler
	{
	private:
		std::vector<ErrorData> logStack;
	private:
		ErrorHandler() = default;
		~ErrorHandler() = default;
	public:
		/*
			Pushes the given error into the error stack.
		*/
		void PushError(const std::string& message, ErrorID id, bool throwException = true);

		/*
			Fetches error(s) thrown by OpenSSL (if any). Any errors found are pushed into error stack.
		*/
		void QueryOpenSSLErrors();

		/*
			Outputs all logs stored in log stack, then proceeding to clear the log stack.
		*/
		void FlushStack();

		/*
			Returns the error stack vector containing errors pushed to the error handler. The error stack is also cleared
			once called.
		*/
		std::vector<ErrorData> GetStack();

		/*
			Returns the latest error that has been pushed.
		*/
		ErrorData GetLatestError() const;

		/*
			Since this class is a singleton, you will have to use this function to access the methods provided
			by the error handler class.
		*/
		static ErrorHandler& GetHandler();
	};
}

#endif
