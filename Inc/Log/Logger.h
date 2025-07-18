#pragma once
#include "Macros.h"
#include "StackTrace.h"

#include <format>
#include <string>
#include <deque>
#include <mutex>

#if true
#pragma region �ȈՋL�q�}�N��

//Logger::Log()�̊ȈՋL�q�}�N��
#define DEBUG_LOG(formatString,...)	\
Log::Logger::Log(formatString , __VA_ARGS__)

//Logger::LogError()�̊ȈՋL�q�}�N��
#define DEBUG_LOG_ERROR(formatString,...)\
Log::Logger::LogError(formatString , __VA_ARGS__)

//Logger::LogAssert()�̊ȈՋL�q�}�N��
#define DEBUG_LOG_ASSERT(condition, formatString,...)\
Log::Logger::LogAssert(condition, formatString , __VA_ARGS__)

//Logger::LogWarning()�̊ȈՋL�q�}�N��
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
	/// ���O���o�͂��邽�߂̃N���X
	/// <para>���ӁF�����[�X�r���h���A���̃N���X�͓��삵�܂���B</para>
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
		/// WinAPI��HRESULT�𕶎���ɕϊ����܂��B
		/// </summary>
		static std::string HRTrace(HRESULT hr);

		/// <summary>
		/// <para>���O���o�͂��܂��B</para>
		/// </summary>	
		template<typename ...Args>
		static void Log(std::string_view formatString, Args&&... args)
		{ m_instance->FlowLog(LogType::Log, formatString, std::forward<Args>(args)...); }

		/// <summary>
		/// <para>�G���[���O���o�͂��܂��B</para>
		/// </summary>
		template<typename ...Args>
		static void LogError(std::string_view formatString, Args&&... args)
		{ m_instance->FlowLog(LogType::Error, formatString, std::forward<Args>(args)...); }

		/// <summary>
		/// <para>�x�����O���o�͂��܂��B</para>
		/// </summary>
		template<typename ...Args>
		static void LogWarning(std::string_view formatString, Args&&... args)
		{ m_instance->FlowLog(LogType::Warning, formatString, std::forward<Args>(args)...); }

		/// <summary>
		/// <para>�A�T�[�V�������O���o�͂��܂��B</para>
		/// <para>��1�����̏�����False�̏ꍇ�A���O�y��RuntimeError���o�͂��܂��B</para>
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
		/// <para>�A�T�[�V�������O���o�͂��܂��B</para>
		/// <para>��1������FAILED�}�N�����^�̏ꍇ�A���O�y��RuntimeError���o�͂��܂��B</para>
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
		static constexpr uint32_t SaveArchiveCount = 100; //�ۑ�����ߋ����O�̐� ���̐��𒴂���Ɖߋ����O����Â����̏��ɍ폜����Ă���

		inline static std::unique_ptr<Logger> m_instance;

	private:
		std::deque<Buffer>	m_archivedLogs;
		std::mutex				m_mutex;
	};

}
