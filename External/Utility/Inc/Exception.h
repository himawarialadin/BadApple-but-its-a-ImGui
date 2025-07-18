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
	/// <para>ネイティブC++では例外クラスが少ないため、追加・ラップをして例外処理を分かりやすくする。</para>
	/// <para>例外発生時、DEBUG_LOG_ERRORでメッセージが出力される。</para>
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

	//nullオブジェクト参照を逆参照しようとした場合にスローされる例外
	class NullReferenceException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(NullReferenceException) };

	//オブジェクトの現在の状態に対して無効なメソッド呼び出しが行われた場合にスローされる例外
	class InvalidOperationException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(InvalidOperationException) };

	//メソッドに渡された引数のいずれかが無効な場合にスローされる例外
	class InvalidArgumentException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(InvalidArgumentException) };

	//境界外のインデックスを使用して配列またはコレクションの要素にアクセスしようとしたときにスローされる例外
	class IndexOutOfRangeException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(IndexOutOfRangeException) };

	//コレクション内の要素にアクセスするために指定したキーが、コレクション内のいずれのキーとも一致しない場合にスローされる例外
	class KeyNotFoundException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(KeyNotFoundException) };

	//文字列の書式に誤りがあった場合にスローされる例外
	class FormatErrorException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(FormatErrorException) };

	//要求されたメソッドまたは操作が実装されない場合にスローされる例外
	class NotImplementedException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(NotImplementedException) };

	//ディスク上に存在しないファイルにアクセスしようとして失敗したときにスローされる例外
	class FileNotFoundException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(FileNotFoundException) };
	
	//ディスク上に存在しないディレクトリにアクセスしようとして失敗したときにスローされる例外
	class DirectoryNotFoundException : public Exception
	{ __EXCEPTION_CONSTRUCTOR(DirectoryNotFoundException) };

	//ディスク上に存在しないパスにアクセスしようとして失敗したときにスローされる例外
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
