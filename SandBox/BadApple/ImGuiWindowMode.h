#pragma once
#include "IMode.h"
#include <Math/Vector2.h>
#include <Math/Color.h>

class ImGuiWindowMode : public IMode
{
public:
	ImGuiWindowMode();
	~ImGuiWindowMode() = default;

	void Show() override;
	void ShowSettings() override;

private:
	void Draw();
	void ShowWindow(uint32_t index, Math::Vector2 const& windowPos, Math::Vector2 const& windowSize);

private:
	bool					m_isKeepAspectRatio = true; // アスペクト比を維持するかどうか
	bool					m_showTitleBar			= true; // タイトルバーを表示するかどうか
	bool					m_hideTinyWindows	= false; //タイトルバーより小さいウィンドウの非表示
	float					m_windowFontSize;
	float					m_windowFontSizeMin;
	Math::Color		m_windowTitleBarColor;
	Math::Color		m_windowTitleColor;

	Math::Vector2	m_displayPos; // 表示位置
	Math::Vector2	m_displaySize; // 表示サイズ
};

