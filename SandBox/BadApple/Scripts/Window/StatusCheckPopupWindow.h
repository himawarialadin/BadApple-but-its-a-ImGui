#pragma once
#include "IWindow.h"

class StatusCheckPopupWindow : public IWindow
{
public:
	StatusCheckPopupWindow();
	~StatusCheckPopupWindow();
	void Show() override;

public:

private:
	bool m_exisingVideo		= false; // ビデオが存在するかどうか
	bool m_exisingAudio		= false; // オーディオが存在するかどうか
	bool m_isShowingPopup = false; // ポップアップが表示中かどうか

	float m_dlay					= 0.0f; // ポップアップ表示の遅延時間
};