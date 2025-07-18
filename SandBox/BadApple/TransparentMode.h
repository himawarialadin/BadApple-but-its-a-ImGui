#pragma once
#include "IMode.h"
#include <Math/Vector2.h>

typedef unsigned int ImU32;

//
class TransparentMode : public IMode
{
public:
	void Show() override;
	void ShowSettings() override;

private:
	void DrawOutside();
	void Draw();

private:
	ImU32				m_backgroundColor = 0x00000000; // Transparent background color
	Math::Vector2	m_windowMin;
	Math::Vector2	m_windowMax;
	Math::Vector2	m_displayPos;		
	Math::Vector2	m_displaySize;

	bool					m_isKeepAspectRatio = true; // ����̃A�X�y�N�g����ێ����邩�ǂ���
	bool					m_isFillOutside			= true; // �\���̈�ȊO��h��Ԃ����ǂ���
};