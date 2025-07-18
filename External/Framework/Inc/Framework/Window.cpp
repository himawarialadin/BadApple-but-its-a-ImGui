#include "Window.h"
#include "Graphics.h"
#include <dwmapi.h>
#include <windowsx.h> 

static BOOL _IsWindowsVersionOrGreater(WORD major, WORD minor, WORD)
{
	typedef LONG(WINAPI* PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);
	static PFN_RtlVerifyVersionInfo RtlVerifyVersionInfoFn = nullptr;
	if (RtlVerifyVersionInfoFn == nullptr)
		if (HMODULE ntdllModule = ::GetModuleHandleA("ntdll.dll"))
			RtlVerifyVersionInfoFn = (PFN_RtlVerifyVersionInfo)GetProcAddress(ntdllModule, "RtlVerifyVersionInfo");
	if (RtlVerifyVersionInfoFn == nullptr)
		return FALSE;

	RTL_OSVERSIONINFOEXW versionInfo = { };
	ULONGLONG conditionMask = 0;
	versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	versionInfo.dwMajorVersion = major;
	versionInfo.dwMinorVersion = minor;
	VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	return (RtlVerifyVersionInfoFn(&versionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask) == 0) ? TRUE : FALSE;
}

#define _IsWindows8OrGreater()       _IsWindowsVersionOrGreater(HIBYTE(0x0602), LOBYTE(0x0602), 0) // _WIN32_WINNT_WIN8
#define _IsWindows8Point1OrGreater() _IsWindowsVersionOrGreater(HIBYTE(0x0603), LOBYTE(0x0603), 0) // _WIN32_WINNT_WINBLUE
#define _IsWindows10OrGreater()      _IsWindowsVersionOrGreater(HIBYTE(0x0A00), LOBYTE(0x0A00), 0) // _WIN32_WINNT_WINTHRESHOLD / _WIN32_WINNT_WIN10

Window::Window(std::wstring_view title, std::wstring_view className)
{
	if (className.empty())
		className = L"ImGui SandBox";
	if (title.empty())
		title = L"ImGui SandBox";

	wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, className.data(), nullptr };
	::RegisterClassExW(&wc);

	hwnd = ::CreateWindowW(wc.lpszClassName, title.data(), WS_OVERLAPPEDWINDOW, 100, 100, windowWidth, windowHeight, nullptr, nullptr, wc.hInstance, nullptr);
	instance = this;
}


void Window::Show()
{
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);
}

bool Window::ProcessMessage()
{
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
			return false;
	}
	
	return true;
}

void Window::Finalize()
{
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

void Window::SetEnableAlphaComposing(bool enable, HWND hwnd /*= nullptr*/)
{
	if (hwnd == nullptr)
		hwnd = this->hwnd;

	enableAlphaComposing = enable;

    BOOL composition;
    if (FAILED(::DwmIsCompositionEnabled(&composition)) || !composition)
        return;

    if (enable)
    {
        BOOL opaque;
        DWORD color;
        if (_IsWindows8OrGreater() || (SUCCEEDED(::DwmGetColorizationColor(&color, &opaque)) && !opaque))
        {
            // ぼかし領域を指定（透明ウィンドウ＋背景ぼかし）
            HRGN region = ::CreateRectRgn(0, 0, -1, -1);
            DWM_BLURBEHIND bb = {};
            bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
            bb.hRgnBlur = region;
            bb.fEnable = TRUE;
            ::DwmEnableBlurBehindWindow(hwnd, &bb);
            ::DeleteObject(region);
        }
        else
        {
            // Windows 7など：領域なしでぼかしのみ
            DWM_BLURBEHIND bb = {};
            bb.dwFlags = DWM_BB_ENABLE;
            ::DwmEnableBlurBehindWindow(hwnd, &bb);
        }
    }
    else
    {
        // 無効化
        DWM_BLURBEHIND bb = {};
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = FALSE;
        ::DwmEnableBlurBehindWindow(hwnd, &bb);
    }
}

LRESULT WINAPI Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(instance == nullptr)
		return ::DefWindowProcW(hWnd, msg, wParam, lParam);

	if (instance->customWndProc != nullptr && instance->customWndProc(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
	{
		UINT width = (UINT)LOWORD(lParam);
		UINT height = (UINT)HIWORD(lParam);
		if (width == 0 && height == 0)
			return 0;
		if (instance->windowWidth == width && instance->windowHeight == height)
			return 0;

		if (DX11Graphics::Instance != nullptr)
		{
			instance->windowWidth = width;
			instance->windowHeight = height;
			DX11Graphics::Instance->Resize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
		}

		return 0;
	}
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}