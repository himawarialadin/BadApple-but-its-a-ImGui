#include "StatusPopupWindow.h"
#include <filesystem>
#include "Constant.h"
#include <ImGui/imgui.h>
#include <Framework/Timer.h>
#include <Framework/Framework.h>

StatusPopupWindow::StatusPopupWindow()
{
	namespace fs = std::filesystem;
	m_exisingVideo = fs::exists(fs::path(VideoFileName));
	m_exisingAudio = fs::exists(fs::path(AudioFileName));
}

StatusPopupWindow::~StatusPopupWindow()
{
}

void StatusPopupWindow::Show()
{
	if (m_exisingAudio && m_exisingVideo)
		return;

	m_dlay += Timer::DeltaTime();
	if (!m_showingPopup && m_dlay > 0.1f)
	{
		ImGui::OpenPopup("Status Error", ImGuiPopupFlags_NoOpenOverExistingPopup);

		if (ImGui::IsPopupOpen("Status Error"))
			m_showingPopup = true; // ポップアップが表示中に設定
		
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Status Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("Please check the file paths and ensure they exist :");

		if (!m_exisingVideo)
			ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, "Video file not found: %s", VideoFileName.data());
		if (!m_exisingAudio)
			ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "Audio file not found: %s", AudioFileName.data());
		ImGui::NewLine();

		ImGui::Separator();
		ImGui::Text("For details, please refer to Assets/README.md.");
		ImGui::Text("Please place the file in the specified location and restart the application.");
		if (ImGui::Button("Exit (Close the Application)", { -1.0f, 0.0f }))
		{
			ImGui::CloseCurrentPopup();
			m_showingPopup = false;

			Framework::Exit(); // アプリケーションを終了
		}
		ImGui::EndPopup();
	}

}
