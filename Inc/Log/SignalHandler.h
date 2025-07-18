#pragma once
#include "Macros.h"
#include <csignal>
#include <cassert>

namespace Log
{
	//シグナル通知を設定するクラス (static class)
	class SignalHandler
	{
		STATIC_CLASS(SignalHandler);
	public:
		static void AddSignalHandler(int signal, _crt_signal_t handler)
		{
			SignalHandlerData& signalHandlerData = g_signalHandlers[GetSignalIndex(signal)];
			if (signalHandlerData.count >= SignalCount)
				return;

			if (signalHandlerData.count == 0)
				std::signal(signal, &SignalHandlerFunction);

			signalHandlerData.signals[signalHandlerData.count] = handler;
			signalHandlerData.count++;
		}

	private:
		static void SignalHandlerFunction(int signal)
		{
			SignalHandlerData& signalHandlerData = g_signalHandlers[GetSignalIndex(signal)];
			for (int i = 0; i < signalHandlerData.count; i++)
			{
				_crt_signal_t handler = signalHandlerData.signals[i];
				if (handler != nullptr)
					handler(signal);
			}
		}

		static int GetSignalIndex(int signal)
		{
			for (int i = 0; i < SignalTypeCount; i++)
			{
				if (Signals[i] == signal)
					return i;
			}

			assert(!"Invalid signal or not supported signal.");
			return -1;
		}

	private:
		inline static constexpr int Signals[] = { SIGINT, SIGILL, SIGFPE, SIGSEGV, SIGTERM,SIGBREAK, SIGABRT };
		inline static constexpr int SignalCount				= 4;
		inline static constexpr int SignalTypeCount		= __crt_countof(Signals);

		struct SignalHandlerData
		{
			_crt_signal_t signals[SignalCount] = {};
			int					count = 0;
		};

		inline static SignalHandlerData g_signalHandlers[SignalTypeCount] = {};
	};


}

