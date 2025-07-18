#pragma once
#include "WindowsFwd.h"
#include <string>
#include <format>

#define __EXCEPTION_CONSTRUCTOR(Type)\
public:\
 Type() : Exception(this) {}\
template<typename ...Args> \
 Type(std::string_view msg, Args&&... args) : Exception(this, msg, std::forward<Args>(args)...) {}\

namespace Exc
{
	/// <summary>
	/// <para>�l�C�e�B�uC++�ł͗�O�N���X�����Ȃ����߁A�ǉ��E���b�v�����ė�O�����𕪂���₷������B</para>
	/// <para>��O�������ADEBUG_LOG_ERROR�Ń��b�Z�[�W���o�͂����B</para>
	/// </summary>
	class Exception
	{
	private:
		Exception(std::string_view type, std::string_view msg);
		Exception(std::string_view type, std::string_view msg, std::format_args args);

	public:
		Exception(std::string_view msg) : Exception(typeid(Exception).name(), msg) {}

		template<typename T> requires std::is_class_v<T>
		Exception([[maybe_unused]] T* dummy) : Exception(typeid(T).name(), "") {}

		template<typename T, typename ...Args> requires std::is_class_v<T>
		Exception([[maybe_unused]] T* dummy, std::string_view msg, Args&&... args) 
			: Exception(typeid(T).name(), msg, std::make_format_args(args...)) {}

		std::string const& what() const noexcept { return m_msg; }

	private:
		std::string m_msg;
	};

	//null�I�u�W�F�N�g�Q�Ƃ��t�Q�Ƃ��悤�Ƃ����ꍇ�ɃX���[������O
	class NullReferenceException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(NullReferenceException) };

	//�I�u�W�F�N�g�̌��݂̏�Ԃɑ΂��Ė����ȃ��\�b�h�Ăяo�����s��ꂽ�ꍇ�ɃX���[������O
	class InvalidOperationException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(InvalidOperationException) };

	//���\�b�h�ɓn���ꂽ�����̂����ꂩ�������ȏꍇ�ɃX���[������O
	class InvalidArgumentException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(InvalidArgumentException) };

	//���E�O�̃C���f�b�N�X���g�p���Ĕz��܂��̓R���N�V�����̗v�f�ɃA�N�Z�X���悤�Ƃ����Ƃ��ɃX���[������O
	class IndexOutOfRangeException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(IndexOutOfRangeException) };

	//�R���N�V�������̗v�f�ɃA�N�Z�X���邽�߂Ɏw�肵���L�[���A�R���N�V�������̂�����̃L�[�Ƃ���v���Ȃ��ꍇ�ɃX���[������O
	class KeyNotFoundException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(KeyNotFoundException) };

	//������̏����Ɍ�肪�������ꍇ�ɃX���[������O
	class FormatErrorException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(FormatErrorException) };

	//�v�����ꂽ���\�b�h�܂��͑��삪��������Ȃ��ꍇ�ɃX���[������O
	class NotImplementedException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(NotImplementedException) };

	//�f�B�X�N��ɑ��݂��Ȃ��t�@�C���ɃA�N�Z�X���悤�Ƃ��Ď��s�����Ƃ��ɃX���[������O
	class FileNotFoundException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(FileNotFoundException) };
	
	//�f�B�X�N��ɑ��݂��Ȃ��f�B���N�g���ɃA�N�Z�X���悤�Ƃ��Ď��s�����Ƃ��ɃX���[������O
	class DirectoryNotFoundException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(DirectoryNotFoundException) };

	//�f�B�X�N��ɑ��݂��Ȃ��p�X�ɃA�N�Z�X���悤�Ƃ��Ď��s�����Ƃ��ɃX���[������O
	class PathNotFoundException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(PathNotFoundException) };

	class HResultErrorException : public Exception
	{
	public:
		HResultErrorException(HRESULT result, std::string_view msg);

		template<typename ...Args>
		HResultErrorException(HRESULT result, std::string_view msg, Args&&... args) : HResultErrorException(result, std::vformat(msg, std::make_format_args(std::forward<Args>(args)...)).c_str()) {}
	};

};
