#include "PopupWindow.h"
#include "Common.h"
#include "Constant.h"

#include <d3d11.h>
#include <Framework/Timer.h>
#include <Framework/TextureLoader.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui.h>
#include <Math/Converter/ImVec.h>

constexpr float LogoScale	= 0.5f;
constexpr float PopupDelay = 0.1f; // ポップアップ表示の遅延時間

PopupWindow::PopupWindow()
{
	TextureLoader::LoadTextureFromFile(FMOD_LogoFilename.data(),m_fmodLogoTex.ReleaseAndGetAddressOf(), & m_fmodLogoSize);
	if(m_fmodLogoTex == nullptr)
		throw std::runtime_error("Failed to load FMOD logo texture");
}

PopupWindow::~PopupWindow()
{
}

void PopupWindow::Show()
{
	if (m_showed)
		return;
	
	m_dlay += Timer::DeltaTime();
	if (!m_isShowingPopup && m_dlay > PopupDelay)
	{
		ImGui::OpenPopup("License", ImGuiPopupFlags_NoOpenOverExistingPopup);

		if (ImGui::IsPopupOpen("License"))
			m_isShowingPopup = true; // ポップアップが表示中に設定
	}
	
	ImVec2 center		= ImGui::GetMainViewport()->GetCenter();
	ImVec2 logoSize		= Math::Cast<ImVec2>(m_fmodLogoSize) * LogoScale;
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("License", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("Bad Apple!! but it's ImGui");

		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Text("Made using FMOD Studio by Firelight Technologies Pty Ltd.");
		ImGui::NewLine();
		ImGui::Image(m_fmodLogoTex.Get(), logoSize);

		ImGui::NewLine();
		ImGui::Separator();
		if (ImGui::Button("OK", { -1.0f, 0.0f }))
		{
			ImGui::CloseCurrentPopup();
			m_showed = true;
		}

		ImGui::EndPopup();
	}
	

}
