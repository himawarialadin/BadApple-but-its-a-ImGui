#include "StackTrace.h"
#include <windows.h>
#include <imagehlp.h>
#include <iostream>
#include <format>

#pragma comment(lib, "imagehlp.lib")

namespace Log
{

	StackTrace::StackTrace()
	{
		m_process = GetCurrentProcess();
		
		m_symbolInfo = (IMAGEHLP_SYMBOL64*)malloc(MAX_SYM_NAME);
		m_symbolInfo->SizeOfStruct			= sizeof(IMAGEHLP_SYMBOL64);
		m_symbolInfo->MaxNameLength	= MAX_SYM_NAME;

		//初期化
		SymInitialize(m_process, NULL, TRUE);
	
		//SymFromAddrで取得できるシンボル名を修飾する(関数の戻り値や引数の情報を付加する)
		SymSetOptions(SYMOPT_PUBLICS_ONLY);
	}
	
	StackTrace::~StackTrace()
	{
		free(m_symbolInfo);
	}
	
	void StackTrace::Capture(ULONG traceSkipCount)
	{
		m_symbols.clear();
	
		//スタックトレースのキャプチャ開始
		m_frames = CaptureStackBackTrace(traceSkipCount, CaptureTraceCount, m_stacks, NULL);
	
		//スタックトレースの情報を出力
		char szUndName[MAX_SYM_NAME];
		for (DWORD i = 0; i < m_frames; i++)
		{
			SymbolInfo info;

			//シンボル名を取得
			DWORD64 displacement = 0;
			if (SymGetSymFromAddr64(m_process, (DWORD64)(m_stacks[i]), &displacement, m_symbolInfo))
			{
				//シンボルが定義されているファイル名と行番号を取得
				{
					IMAGEHLP_LINE64 line;
					DWORD displacement2 = 0;
					line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
					if (SymGetLineFromAddr64(m_process, (DWORD64)(m_stacks[i]), &displacement2, &line))
					{
						info.filename	= line.FileName;
						info.line			= line.LineNumber;//行番号(ただし、関数の最終行の位置を表す)
					}
				}

				//修飾済みシンボル名を解除
				DWORD flags = 0;
				flags |= UNDNAME_NO_MS_KEYWORDS;			//MSの独自キーワードの展開無効
				flags |= UNDNAME_NO_SPECIAL_SYMS;				//特殊名をデコードしない
				flags |= UNDNAME_NAME_ONLY;						//名前空間の展開をしない
				flags |= UNDNAME_NO_ACCESS_SPECIFIERS;	//
				if (UnDecorateSymbolName(m_symbolInfo->Name, szUndName, sizeof(szUndName), flags))
				{
					int MaxNameLength = 255;
					info.name = szUndName;
					if(info.name.length() > MaxNameLength)
						info.name = info.name.substr(0, MaxNameLength) + "...";
				}
	
			}
	
			m_symbols.emplace_back(info);
		}
	}

}