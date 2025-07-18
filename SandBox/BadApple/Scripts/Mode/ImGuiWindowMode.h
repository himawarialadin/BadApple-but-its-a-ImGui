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
	bool					m_isKeepAspectRatio = true; // �A�X�y�N�g����ێ����邩�ǂ���
	bool					m_showTitleBar			= true; // �^�C�g���o�[��\�����邩�ǂ���
	bool					m_hideTinyWindows	= false; //�^�C�g���o�[��菬�����E�B���h�E�̔�\��
	float					m_windowFontSize;
	float					m_windowFontSizeMin;
	Math::Color		m_windowTitleBarColor;
	Math::Color		m_windowTitleColor;

	Math::Vector2	m_displayPos; // �\���ʒu
	Math::Vector2	m_displaySize; // �\���T�C�Y
};

