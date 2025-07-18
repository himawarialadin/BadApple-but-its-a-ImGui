#pragma once
#include <Windows.h>
#include <string>
#include <string_view>

class Window
{
public:
	Window(std::wstring_view title = L"", std::wstring_view className = L"");
	~Window() = default;
public:
	void Show();
	bool ProcessMessage();

	void Finalize();
	void SetEnableAlphaComposing(bool enable, HWND hwnd = nullptr);

public:
	static Window* GetInstance() { return instance; }

	HWND							GetHWND()				const { return hwnd; }
	WNDCLASSEXW const&	GetWindowClass()		const { return wc; }
	UINT								GetWindowWidth()		const { return windowWidth; }
	UINT								GetWindowHeight()	const { return windowHeight; }
	bool 								IsEnableAlphaComposing() const { return enableAlphaComposing; }
	void SetCustomWndProc(WNDPROC wndProc) { customWndProc = wndProc; }

private:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	inline static Window* instance = nullptr;

	HWND					hwnd;
	WNDCLASSEXW	wc;

	WNDPROC				customWndProc;
	UINT						windowWidth					= 1280;
	UINT						windowHeight				= 800;
	bool						enableAlphaComposing	= false;
};

