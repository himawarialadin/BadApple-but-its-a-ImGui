#pragma once
#include "IWindow.h"

class StatusPopupWindow : public IWindow
{
public:
	StatusPopupWindow();
	~StatusPopupWindow();
	void Show() override;

public:

private:
	bool m_exisingVideo = false; // �r�f�I�����݂��邩�ǂ���
	bool m_exisingAudio = false; // �I�[�f�B�I�����݂��邩�ǂ���

	bool m_showingPopup = false; // �|�b�v�A�b�v���\�������ǂ���
	float m_dlay = 0.0f; // �|�b�v�A�b�v�\���̒x������
};