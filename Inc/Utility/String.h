#pragma once
#include <string>
#include <vector>
#include <typeinfo>

namespace Utility
{
	/// <summary>
	/// ANSI�����񂩂烏�C�h������ɕϊ�
	/// </summary>
	std::wstring	AnsiToWide(std::string_view string);

	std::wstring	UTF8ToWide(std::string_view string);

	std::wstring	UTF8WideToWide(std::wstring_view string);

	/// <summary>
	/// ���C�h�����񂩂�ANSI������ɕϊ�
	/// </summary>
	std::string		WideToAnsi(std::wstring_view string);

	/// <summary>
	/// ANSI�����񂩂�U8������(std::string)�ɕϊ�
	/// </summary>
	std::string		AnsiToUTF8(std::string_view string);

	/// <summary>
	/// ���s���^��񂩂�^�̖��O�����𒊏o����������ɕϊ�
	/// </summary>
	std::string		TypeInfoToString(std::type_info const& type);
	std::string		TypeInfoToString(std::string_view typeString);

	/// <summary>
	/// �������delim�ŕ������Ă����Ԃ�
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
	/// �����񂩂�w�肵���p�^�[���̕�����𒊏o����
	/// </summary>
	/// <param name="input">���͕�����</param>
	/// <param name="pattern">�p�^�[�������� (���K�\��)</param>
	/// <returns>�p�^�[���Ɉ�v���������񂪂���ꍇ�͂��̕�����A�Ȃ���΋󕶎���</returns>
	std::string ExtractSubstring( std::string const& input, std::string const& pattern);

};