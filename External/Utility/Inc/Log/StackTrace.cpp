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

		//������
		SymInitialize(m_process, NULL, TRUE);
	
		//SymFromAddr�Ŏ擾�ł���V���{�������C������(�֐��̖߂�l������̏���t������)
		SymSetOptions(SYMOPT_PUBLICS_ONLY);
	}
	
	StackTrace::~StackTrace()
	{
		free(m_symbolInfo);
	}
	
	void StackTrace::Capture(ULONG traceSkipCount)
	{
		m_symbols.clear();
	
		//�X�^�b�N�g���[�X�̃L���v�`���J�n
		m_frames = CaptureStackBackTrace(traceSkipCount, CaptureTraceCount, m_stacks, NULL);
	
		//�X�^�b�N�g���[�X�̏����o��
		char szUndName[MAX_SYM_NAME];
		for (DWORD i = 0; i < m_frames; i++)
		{
			SymbolInfo info;

			//�V���{�������擾
			DWORD64 displacement = 0;
			if (SymGetSymFromAddr64(m_process, (DWORD64)(m_stacks[i]), &displacement, m_symbolInfo))
			{
				//�V���{������`����Ă���t�@�C�����ƍs�ԍ����擾
				{
					IMAGEHLP_LINE64 line;
					DWORD displacement2 = 0;
					line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
					if (SymGetLineFromAddr64(m_process, (DWORD64)(m_stacks[i]), &displacement2, &line))
					{
						info.filename	= line.FileName;
						info.line			= line.LineNumber;//�s�ԍ�(�������A�֐��̍ŏI�s�̈ʒu��\��)
					}
				}

				//�C���ς݃V���{����������
				DWORD flags = 0;
				flags |= UNDNAME_NO_MS_KEYWORDS;			//MS�̓Ǝ��L�[���[�h�̓W�J����
				flags |= UNDNAME_NO_SPECIAL_SYMS;				//���ꖼ���f�R�[�h���Ȃ�
				flags |= UNDNAME_NAME_ONLY;						//���O��Ԃ̓W�J�����Ȃ�
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