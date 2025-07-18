#pragma once
#include "WindowsFwd.h"
#include <vector>
#include <string>

namespace Log
{ 

	//シンボル情報
	//StackTraceで使用します
	struct SymbolInfo
	{
		std::string		name;		//シンボル名
		std::string		filename;	//このシンボルが定義されているファイル名
		int					line;			//このシンボルが定義されているファイルの行番号
	};

	class StackTrace
	{
	public:
		StackTrace();
		~StackTrace();
	
	public:
		static StackTrace Current(ULONG traceSkipCount = 0) { StackTrace s; s.Capture(traceSkipCount); return s; }
	
		//スタックトレースのキャプチャを行います
		void													Capture(ULONG traceSkipCount = 0);
	
		//キャプチャしたトレース数を取得します
		//この値はGetSymbolsで返される要素数と同じです
		WORD												GetCapturedTraceCount()	const { return m_frames;	}
	
		//キャプチャしたシンボル情報を取得します
		//要素順序は呼ばれた順序です
		std::vector<SymbolInfo> const&		GetSymbols()					const { return m_symbols; }
	
	private:
		inline static constexpr ULONG CaptureTraceCount	= 10;
	
		HANDLE										m_process;
		IMAGEHLP_SYMBOL64*					m_symbolInfo	= nullptr;
		void*												m_stacks[CaptureTraceCount] = {};
		WORD											m_frames = 0;
		std::vector<SymbolInfo>				m_symbols;
	};

}
