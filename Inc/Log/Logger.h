#pragma once
#include "Macros.h"
#include "StackTrace.h"

#include <format>
#include <string>
#include <deque>
#include <mutex>

#if true
#pragma region 簡易記述マクロ

//Logger::Log()の簡易記述マクロ
#define DEBUG_LOG(formatString,...)	\
Log::Logger::Log(formatString , __VA_ARGS__)

//Logger::LogError()の簡易記述マクロ
#define DEBUG_LOG_ERROR(formatString,...)\
Log::Logger::LogError(formatString , __VA_ARGS__)

//Logger::LogAssert()の簡易記述マクロ
#define DEBUG_LOG_ASSERT(condition, formatString,...)\
Log::Logger::LogAssert(condition, formatString , __VA_ARGS__)

//Logger::LogWarning()の簡易記述マクロ
#define DEBUG_LOG_WARNING(formatString,...)	\
Log::Logger::LogWarning(formatString , __VA_ARGS__)

#define DEBUG_LOG_COM_ASSERT(result, formatString,...)	\
Log::Logger::LogComAssert(result, formatString , __VA_ARGS__)

#define DEBUG_HRTRACE(hr) \
Log::Logger::HRTrace(hr) 



#pragma endregion
#endif // 0

namespace Log
{
	enum class LogType
	{
		Log,
		Warning,
		Error,
		Assert,
	};

	/// <summary>
	/// ログを出力するためのクラス
	/// <para>注意：リリースビルド時、このクラスは動作しません。</para>
	/// </summary>
	class Logger
	{
	public:
		struct Buffer
		{
			LogType							type = (LogType)0;
			std::string							description;
			std::vector<SymbolInfo>	stackTraceSymbols;
		};

	private:
		CREATE_HELPER();
		Logger()	= default;
	public:
		DELETE_COPY_MOVE_COUNSTRACTORS(Logger);
		~Logger() = default;
	public:
		static void Initialize();

	public:
		/// <summary>
		/// WinAPIのHRESULTを文字列に変換します。
		/// </summary>
		static std::string HRTrace(HRESULT hr);

		/// <summary>
		/// <para>ログを出力します。</para>
		/// </summary>	
		template<typename ...Args>
		static void Log(std::string_view formatString, Args&&... args)
		{ m_instance->FlowLog(LogType::Log, formatString, std::forward<Args>(args)...); }

		/// <summary>
		/// <para>エラーログを出力します。</para>
		/// </summary>
		template<typename ...Args>
		static void LogError(std::string_view formatString, Args&&... args)
		{ m_instance->FlowLog(LogType::Error, formatString, std::forward<Args>(args)...); }

		/// <summary>
		/// <para>警告ログを出力します。</para>
		/// </summary>
		template<typename ...Args>
		static void LogWarning(std::string_view formatString, Args&&... args)
		{ m_instance->FlowLog(LogType::Warning, formatString, std::forward<Args>(args)...); }

		/// <summary>
		/// <para>アサーションログを出力します。</para>
		/// <para>第1引数の条件がFalseの場合、ログ及びRuntimeErrorを出力します。</para>
		/// </summary>
		template<typename ...Args>
		static void LogAssert(bool condition, std::string_view formatString, Args&&... args)
		{
			if (!condition)
			{
				m_instance->FlowLog(LogType::Assert,  formatString, std::forward<Args>(args)...);
				__debugbreak();
			}
		}

		/// <summary>
		/// <para>アサーションログを出力します。</para>
		/// <para>第1引数のFAILEDマクロが真の場合、ログ及びRuntimeErrorを出力します。</para>
		/// </summary>
		template<typename ...Args>
		static void LogComAssert(HRESULT result, std::string_view formatString, Args&&... args)
		{
			if (result != 0)
			{
				auto string = std::vformat(formatString, std::make_format_args(std::forward<Args>(args)...));
				auto completeMsg = std::format("{}\nHRESULT : {}", string, HRTrace(result));
				m_instance->FlowLog(LogType::Assert, completeMsg);
				__debugbreak();
			}
		}
		

	private:
		void Output(LogType type, std::string_view msg, std::format_args args);

		template<typename ...Args>
		void FlowLog(LogType type, std::string_view formatString, Args&&... args)
		{ Output(type, formatString, std::make_format_args(args...)); }
	
	private:
		static constexpr uint32_t SaveArchiveCount = 100; //保存する過去ログの数 この数を超えると過去ログから古いもの順に削除されていく

		inline static std::unique_ptr<Logger> m_instance;

	private:
		std::deque<Buffer>	m_archivedLogs;
		std::mutex				m_mutex;
	};

}
