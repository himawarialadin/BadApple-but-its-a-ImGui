#include "Logger.h"
#include "Exception.h"
#include "StackTrace.h"
#include "Utility/String.h"
#include "SignalHandler.h"

#include <Windows.h>
#include <comdef.h>

#include <format>
#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <csignal>

namespace Log
{
	static constexpr auto COUT_RED			= "\x1b[31m";
	static constexpr auto COUT_BACK_RED	= "\x1b[41m\x1b[1m";
	static constexpr auto COUT_YELLOW		= "\x1b[33m";
	static constexpr auto COUT_CLEAR			= "\x1b[0m";

	//出力に出す追加文字列
	struct OutputExtensionStrings
	{
		std::string_view		typeName;
		std::string_view		consoleColor;	//コンソールでの色設定
	};

	constexpr OutputExtensionStrings OutputExtensions[] =
	{
		OutputExtensionStrings{.typeName = "[Log]",			.consoleColor = COUT_CLEAR			},  //Log
		OutputExtensionStrings{.typeName = "[Warning]",	.consoleColor = COUT_YELLOW		},  //Warning
		OutputExtensionStrings{.typeName = "[Error]",		.consoleColor = COUT_RED			},  //Error
		OutputExtensionStrings{.typeName = "[Assert]",		.consoleColor = COUT_BACK_RED	},  //Assert
	};

	static std::string GetLocalTimeString()
	{
		// 現在の時刻を取得
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);

		// 時刻をフォーマットに合わせて出力
		std::tm tm_time;
		localtime_s(&tm_time, &time);  // ローカルタイムに変換

		std::ostringstream oss;
		oss << std::put_time(&tm_time, "%Y_%m_%d_%H_%M_%S");
		return oss.str();
	}

}

namespace Log
{
	static void Terminate()
	{
		std::string str;

		try { std::rethrow_exception(std::current_exception()); }
		catch (const std::exception &e)	{ str = e.what(); }
		catch (const Exc::Exception &e)			{ str = e.what(); }
		catch (...)									{ str = "Unknown Exception"; }

		std::cout << "UnHandled Exception : " << str << std::endl;
		Logger::LogAssert(false, "UnHandled Exception : {}", str);
	}

	void Logger::Initialize()
	{
		m_instance = std::make_unique<CreateHelper<Logger>>();
		std::set_terminate(&Terminate);

		SignalHandler::AddSignalHandler(SIGABRT, [](int signal)
			{
				std::cout << "SIGABRT : " << signal << std::endl;
				Logger::LogAssert(false, "Abort Has Called! SIGABRT : {}", signal);
			});

	}

	std::string Logger::HRTrace(HRESULT hr)
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

#ifdef UNICODE
		return Utility::WideToAnsi(errMsg);
#else
		return std::string(errMsg);
#endif
	}

	void Logger::Output(LogType type, std::string_view msg, std::format_args args)
	{
		auto formatMsg = std::vformat(msg, args);
		auto trace			= StackTrace::Current(4);

		Buffer outputBuffer
		{
			.type							= type,
			.stackTraceSymbols	= trace.GetSymbols(),
		};

		if (type == LogType::Assert)
		{
			outputBuffer.description += "\nAssertion failed. \n";
			outputBuffer.description += formatMsg;
		}
		else
		{
			outputBuffer.description += formatMsg;
		}

		//コンソールに表示
		{
			auto& outputExtension = OutputExtensions[static_cast<int>(type)];

			std::string outStr;
			outStr += std::format("{}",		outputExtension.consoleColor);
			outStr += std::format("{} ",		outputExtension.typeName);
			outStr += std::format("{}\n",		outputBuffer.description);
			outStr += COUT_CLEAR;

			//本来はスタックトレースも出すが、コンソールがえらいことになるので出さない
			//output += std::format("#{} {} (at {}:{})\n", i, symbolInfo.name, symbolInfo.filename, symbolInfo.line);

			std::cout << outStr;
			OutputDebugStringA(outStr.c_str());
		}

		{
			std::lock_guard lock(m_mutex);
			m_archivedLogs.emplace_back(outputBuffer);
			if (m_archivedLogs.size() >= SaveArchiveCount)
				m_archivedLogs.pop_front();
		}

		//アサーション失敗の場合、_CrtDbgReportW
		if (type == LogType::Assert)
		{
			std::cout << "CLASH REPORT CREATE...";

			//クラッシュレポートの作成
			{
				if (!std::filesystem::create_directory("Log"))
					std::cout << "DIRECTORY CREATE FAILED!\n";

				auto reportFilePath = std::format("Log/CrashReport_{}.txt", GetLocalTimeString());

				std::ofstream ofs(reportFilePath);
				if (ofs)
				{
					for (auto& arhived : m_archivedLogs)
					{
						auto& outputExtension = OutputExtensions[static_cast<int>(arhived.type)];
						std::string log;

						log += std::format("{}",	outputExtension.typeName);
						log += std::format("{}\n", arhived.description);
						ofs << log;
					}

					std::string log;
					log += std::format("--------------------[EXCEPTION STACKTRACE INFO]--------------------\n");
					for (int i = 0; auto& symbol : m_archivedLogs.back().stackTraceSymbols)
					{
						log += std::format("#{} {} (at {}:{})\n", i, symbol.name, symbol.filename, symbol.line);
						i++;
					}
					log += std::format("-------------------------------------------------------------------------------\n");
					ofs << log;
					ofs.close();

					std::cout << "CREATED\n";
				}
				else
				{
					std::cout << "FAILED!\n";
				}
			}

#ifdef _DEBUG
			auto& stackTraceSymbol = outputBuffer.stackTraceSymbols[0];
			_CrtDbgReportW
			(
				_CRT_ASSERT,
				Utility::AnsiToWide(stackTraceSymbol.filename).c_str(),
				stackTraceSymbol.line,
				Utility::AnsiToWide(stackTraceSymbol.name).c_str(),
				Utility::AnsiToWide(outputBuffer.description).c_str()
			);


#else
			MessageBox(nullptr, Utility::AnsiToWide(outputBuffer.description).c_str(), L"Assertion Failed", MB_OK);
			abort();
#endif

		}
	}

}
