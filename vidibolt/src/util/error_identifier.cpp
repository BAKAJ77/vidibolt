#include <util/error_identifier.h>

namespace Volt
{
	ErrorCode::ErrorCode() :
		code(ErrorID::NONE)
	{}

	ErrorCode::ErrorCode(ErrorID code) :
		code(code)
	{}

	ErrorCode::~ErrorCode() = default;

	void ErrorCode::operator=(ErrorCode code)
	{
		this->code = code;
	}

	ErrorCode::operator bool() const
	{
		return this->code != ErrorID::NONE;
	}

	ErrorCode::operator ErrorID() const
	{
		return this->code;
	}
}