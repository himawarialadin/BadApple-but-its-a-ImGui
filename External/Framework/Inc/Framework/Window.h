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
	void AddStyle(LONG flags);
	void RemoveStyle(LONG flags);

	LONG GetStyle() const;
	bool IsAvailableAlphaComposing() const { return m_isAvailableAlphaComposing; }

public:
	static Window* GetInstance() { return instance; }

	HWND							GetHWND()				const { return m_hwnd; }
	WNDCLASSEXW const&	GetWindowClass()		const { return m_wc; }
	UINT								GetWindowWidth()		const { return m_windowWidth; }
	UINT								GetWindowHeight()	const { return m_windowHeight; }
	bool 								IsEnableAlphaComposing() const { return m_enableAlphaComposing; }
	void SetCustomWndProc(WNDPROC wndProc) { m_customWndProc = wndProc; }

private:
	bool CheckAlphaComposingSupport(HWND hwnd);
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	inline static Window* instance = nullptr;

	HWND					m_hwnd;
	WNDCLASSEXW	m_wc;

	WNDPROC				m_customWndProc;
	UINT						m_windowWidth				= 1280;
	UINT						m_windowHeight				= 800;
	bool						m_enableAlphaComposing	= false;
	bool						m_isAvailableAlphaComposing = false; // 透明ウィンドウが有効かどうか
};

