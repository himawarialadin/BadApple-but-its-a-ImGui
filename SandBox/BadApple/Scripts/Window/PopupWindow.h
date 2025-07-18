#pragma once
#include "IWindow.h"
#include <Math/Vector2Int.h>
#include <wrl.h>

struct ID3D11ShaderResourceView;
class PopupWindow : public IWindow
{
public:
	PopupWindow();
	~PopupWindow();
	void Show() override;

public:
	
private:
	using SRV = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>;

	SRV						m_fmodLogoTex			= nullptr;
	bool						m_showed					= false;
	bool						m_isShowingPopup		= false; // �|�b�v�A�b�v���\�������ǂ���
	float						m_dlay						= 0.0f; // �|�b�v�A�b�v�\���̒x������
	Math::Vector2Int	m_fmodLogoSize;
};