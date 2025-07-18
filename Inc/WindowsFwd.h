//WindowsAPIä÷êîÇ≈ïKóvÇ…Ç»ÇÈå^ÇëOï˚êÈåæÇµÇƒÇ®Ç≠èÍèä
#pragma once
#include <sal.h>

#pragma region Macors

#define CALLBACK __stdcall

#pragma endregion

#pragma region Integer

typedef unsigned long			DWORD;
typedef unsigned int			UINT;
typedef __int64					INT_PTR,		*PINT_PTR;
typedef unsigned __int64	UINT_PTR,	*PUINT_PTR;
typedef __int64					LONG_PTR,	*PLONG_PTR;
typedef unsigned __int64	ULONG_PTR, *PULONG_PTR;

typedef unsigned short		WORD;
typedef unsigned long			ULONG;

#pragma endregion

#pragma region Struct
struct tagRECT;
typedef tagRECT RECT, * PRECT, * NPRECT, * LPRECT;

#pragma endregion


#pragma region String

typedef wchar_t					WCHAR;
typedef _Null_terminated_ WCHAR	*LPWSTR;

#pragma endregion

#pragma region Special

typedef UINT_PTR		WPARAM;
typedef LONG_PTR	LPARAM;
typedef LONG_PTR	LRESULT;
typedef _Return_type_success_(return >= 0) long HRESULT;

struct	_IMAGEHLP_SYMBOL64;
typedef _IMAGEHLP_SYMBOL64 IMAGEHLP_SYMBOL64;

#pragma endregion

#pragma region Handles


#define FORWARD_DECLARE_HANDLE(name) struct name##__; typedef struct name##__* name
FORWARD_DECLARE_HANDLE(HWND);
FORWARD_DECLARE_HANDLE(HINSTANCE);
FORWARD_DECLARE_HANDLE(HDC);
FORWARD_DECLARE_HANDLE(HGLRC);
FORWARD_DECLARE_HANDLE(HHOOK);

typedef void*				HANDLE;
typedef HINSTANCE	HMODULE;

#pragma endregion

#pragma region CallBacks

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

#pragma endregion