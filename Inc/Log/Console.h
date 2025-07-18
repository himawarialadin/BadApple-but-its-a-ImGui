#pragma once
#include "Macros.h"
#include <Windows.h>
#include <Fcntl.h>
#include <io.h>
#include <cstdio>
#include <cstdlib>

/// <summary>
/// �f�o�b�O�p�R���\�[����ʕ\��
/// </summary>
class Console
{
public:
	STATIC_CLASS(Console);
public:

	static bool Create(bool isUseAnsiEscapeSequence = false)
	{
		if (m_isCreated)
			return false;

		auto handle = GetConsoleWindow();
		if (handle == nullptr)
		{
			m_createConsole = true;
			AllocConsole();

			handle = GetConsoleWindow();
		}

		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONIN$", "r", stdin);


		// https://stackoverflow.com/questions/7414983/how-to-use-the-ansi-escape-code-for-outputting-colored-text-on-console ���
		//ANSI�G�X�P�[�v�V�[�P���X�̗L����(Win10�̂�)
		if (isUseAnsiEscapeSequence)
		{
			HANDLE handleOut = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD consoleMode;
			GetConsoleMode(handleOut, &consoleMode);
			consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			consoleMode |= DISABLE_NEWLINE_AUTO_RETURN;
			SetConsoleMode(handleOut, consoleMode);
		}	

		//�o�c�{�^������
		{
			HWND hwnd		= GetConsoleWindow();
			HMENU hmenu	= GetSystemMenu(hwnd, FALSE);
			EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
		}


		m_isCreated = true;
		std::atexit(Close);

		return true;
	}

	static void Close()
	{
		FreeConsole();
	}

private:
	inline static int		m_hConsole;
	inline static bool		m_isCreated;

	inline static bool m_createConsole = false;
};

