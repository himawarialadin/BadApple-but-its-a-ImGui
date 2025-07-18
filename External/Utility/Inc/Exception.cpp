#include "Exception.h"
#include "Log/Logger.h"

#include <Windows.h>

inline LPSTR HRTraceA(HRESULT hr)
{
	LPSTR msg = nullptr;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&msg), 0, NULL);
	return msg;
}


namespace Exc
{
	Exception::Exception(std::string_view type, std::string_view msg) : m_msg(std::format("{} : {}", type, msg))
	{
		Log::Logger::LogAssert(false, "[{}] Exception occurred. {}", type, msg);
	}

	Exception::Exception(std::string_view type, std::string_view msg, std::format_args args) : m_msg(std::format("{} : {}", type, msg))
	{
		auto formatMsg = std::vformat(msg, args);
		Log::Logger::LogAssert(false, "[{}] Exception occurred. {}", type, formatMsg);
	}

	HResultErrorException::HResultErrorException(HRESULT result, std::string_view msg)
		: Exception(this, "{}\nHResult‚©‚ç‚Ì—áŠO [{:#X}]:{}", msg, static_cast<unsigned long>(result), HRTraceA(result)) {}
}
