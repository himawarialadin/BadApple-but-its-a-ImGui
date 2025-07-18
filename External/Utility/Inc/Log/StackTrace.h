#pragma once
#include "WindowsFwd.h"
#include <vector>
#include <string>

namespace Log
{ 

	//�V���{�����
	//StackTrace�Ŏg�p���܂�
	struct SymbolInfo
	{
		std::string		name;		//�V���{����
		std::string		filename;	//���̃V���{������`����Ă���t�@�C����
		int					line;			//���̃V���{������`����Ă���t�@�C���̍s�ԍ�
	};

	class StackTrace
	{
	public:
		StackTrace();
		~StackTrace();
	
	public:
		static StackTrace Current(ULONG traceSkipCount = 0) { StackTrace s; s.Capture(traceSkipCount); return s; }
	
		//�X�^�b�N�g���[�X�̃L���v�`�����s���܂�
		void													Capture(ULONG traceSkipCount = 0);
	
		//�L���v�`�������g���[�X�����擾���܂�
		//���̒l��GetSymbols�ŕԂ����v�f���Ɠ����ł�
		WORD												GetCapturedTraceCount()	const { return m_frames;	}
	
		//�L���v�`�������V���{�������擾���܂�
		//�v�f�����͌Ă΂ꂽ�����ł�
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
