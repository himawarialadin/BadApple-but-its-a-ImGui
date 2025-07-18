#include "String.h"

#include <Windows.h>
#include <string>
#include <vector>
#include <regex>
#include <typeinfo>
#include <codecvt>

namespace Utility
{
	std::wstring CodePageToWide(std::string_view string, uint32_t codePage)
	{
		if (string.empty())
			return L"";
		auto str = static_cast<std::string>(string);

		int		in_length		= (int)str.length();
		int		out_length		= MultiByteToWideChar(codePage, 0, str.data(), in_length, 0, 0);
		std::vector<wchar_t> buffer(out_length);
		if (out_length)
			MultiByteToWideChar(codePage, 0, str.data(), in_length, &buffer[0], out_length);
		std::wstring result(buffer.begin(), buffer.end());
		return result;
	}


	std::string WideToCodepage(std::wstring_view string, uint32_t codePage)
	{
		if (string.empty())
			return "";

		auto str = static_cast<std::wstring>(string);

		int in_length		= (int)string.length();
		int out_length	= WideCharToMultiByte(codePage, 0, str.data(), in_length, 0, 0, 0, 0);
		std::vector<char> buffer(out_length);
		if (out_length)
			WideCharToMultiByte(codePage, 0, str.data(), in_length, &buffer[0], out_length, 0, 0);
		std::string result(buffer.begin(), buffer.end());
		return result;
	}
	
}

namespace Utility
{

	std::wstring AnsiToWide(std::string_view string)
	{
		return CodePageToWide(string, CP_ACP);
	}

	std::wstring UTF8ToWide(std::string_view string)
	{
		return CodePageToWide(string, CP_UTF8);
	}

	std::wstring UTF8WideToWide(std::wstring_view string)
	{
		auto str = WideToCodepage(string, CP_UTF8);
		return CodePageToWide(str, CP_UTF8);
	}

	std::string WideToAnsi(std::wstring_view string)
	{
		return WideToCodepage(string, CP_ACP);
	}

	std::string AnsiToUTF8(std::string_view string)
	{
		if (string.empty())
			return "";

		auto str = static_cast<std::string>(string);

		int wchars_num = MultiByteToWideChar(CP_ACP, 0, str.data(), -1, NULL, 0);
		wchar_t* wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_ACP, 0, str.data(), -1, wstr, wchars_num);
		int chars_num = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* utf8 = new char[chars_num];
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, chars_num, NULL, NULL);
		std::string result(utf8);
		delete[] wstr;
		delete[] utf8;
		return result;
	}

	std::string TypeInfoToString(std::type_info const& type)
	{
		return TypeInfoToString(type.name());
	}

	std::string TypeInfoToString(std::string_view typeString)
	{
		std::string		typeName = static_cast<std::string>(typeString);
		std::regex		pattern(R"((?:class|struct)?\s*(?:\w+::)*([\w\s]+))");
		std::smatch	match;

		if (std::regex_search(typeName, match, pattern))
			return match[1];
		return "";
	}

	std::string ExtractSubstring(std::string const& input, std::string const& pattern)
	{
		std::regex re(pattern);
		std::smatch match;

		if (std::regex_search(input, match, re) && match.size() > 1)
			return match.str(1); // マッチしたグループを返す

		return "";
	}

}
