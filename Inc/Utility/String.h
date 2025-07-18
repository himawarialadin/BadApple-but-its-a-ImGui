#pragma once
#include <string>
#include <vector>
#include <typeinfo>

namespace Utility
{
	/// <summary>
	/// ANSI文字列からワイド文字列に変換
	/// </summary>
	std::wstring	AnsiToWide(std::string_view string);

	std::wstring	UTF8ToWide(std::string_view string);

	std::wstring	UTF8WideToWide(std::wstring_view string);

	/// <summary>
	/// ワイド文字列からANSI文字列に変換
	/// </summary>
	std::string		WideToAnsi(std::wstring_view string);

	/// <summary>
	/// ANSI文字列からU8文字列(std::string)に変換
	/// </summary>
	std::string		AnsiToUTF8(std::string_view string);

	/// <summary>
	/// 実行時型情報から型の名前だけを抽出した文字列に変換
	/// </summary>
	std::string		TypeInfoToString(std::type_info const& type);
	std::string		TypeInfoToString(std::string_view typeString);

	/// <summary>
	/// 文字列をdelimで分割してそれを返す
	/// </summary>
	template<typename CharT>
	std::vector<std::basic_string<CharT>> Split(std::basic_string_view<CharT> s, CharT delim) 
	{
		using ElementArray = std::vector<std::basic_string<CharT>>;
		using StringStream = std::basic_stringstream<CharT>;

		ElementArray					elems;
		StringStream						ss(s.data());
		std::basic_string<CharT>	item;

		while (std::getline(ss, item, delim)) 
		{
			if (!item.empty())
				elems.emplace_back(item);
		}
		return elems;
	}

	/// <summary>
	/// 文字列から指定したパターンの文字列を抽出する
	/// </summary>
	/// <param name="input">入力文字列</param>
	/// <param name="pattern">パターン文字列 (正規表現)</param>
	/// <returns>パターンに一致した文字列がある場合はその文字列、なければ空文字列</returns>
	std::string ExtractSubstring( std::string const& input, std::string const& pattern);

};