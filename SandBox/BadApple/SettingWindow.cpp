#include "SettingWindow.h"
#include <ImGui/imgui.h>
#include "IMode.h"
#include "VideoWindow.h"
#include "AnimationPlayer.h"
#include "VideoProcessingJob.h"
#include "Constant.h"
#include <Math/Converter/ImVec.h>
#include <ImGui/imgui_internal.h>
#include <Framework/Window.h>

SettingWindow::SettingWindow()
{
}

SettingWindow::~SettingWindow()
{
}

void SettingWindow::SetAnimationPlayer(AnimationPlayer* player)
{
	m_player = player;
	if (m_player)
		m_quadTreeDepth = static_cast<int32_t>(m_player->GetVideoJob()->GetDesc().quadTreeDepth);
	
}

void SettingWindow::Show()
{
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	if(m_mode == nullptr)
		m_mode = std::unique_ptr<IMode>(m_modeBuilders->at(m_modeIndex).first());

	if (ImGui::Begin("Settings"))
	{
		//Header
		if (ImGui::BeginChild("Footer", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::SeparatorText("Animation settings");
			ShowAnimationSettings();

			ImGui::NewLine();
			ImGui::SeparatorText("Video processing settings");
			ShowProcessSettings();
		}
		
		ImGui::EndChild();

		//Footer
		{

		}

	}
	ImGui::End();
}

void SettingWindow::ShowAnimationSettings()
{
	auto& [_, currentModeName] = m_modeBuilders->at(m_modeIndex);
	if (ImGui::BeginCombo("Mode", currentModeName.data()))
	{
		for (uint32_t i = 0; i < m_modeBuilders->size(); i++)
		{
			auto& [_, modeName] = m_modeBuilders->at(i);
			if (ImGui::Selectable(modeName.data()))
			{
				m_modeIndex = i;
				m_mode = std::unique_ptr<IMode>(m_modeBuilders->at(i).first());
			}
		}

		ImGui::EndCombo();
	}

	if(ImGui::TreeNode("Mode settings"))
	{
		m_mode->ShowSettings();
		ImGui::TreePop();
	}

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Checkbox("Show trees", &m_mode->m_data.showTree);

	bool videoShowing = m_videoWindow->IsShowing();
	if (ImGui::Checkbox("Show video (OpenCV)", &videoShowing))
		m_videoWindow->SetShowing(videoShowing);

	bool isAlphaComposing = Window::GetInstance()->IsEnableAlphaComposing();
	if(ImGui::Checkbox("Transparent window", &isAlphaComposing))
		Window::GetInstance()->SetEnableAlphaComposing(isAlphaComposing);
}

void SettingWindow::ShowProcessSettings()
{
	ImGui::TextColored(Math::Cast<ImVec4>(Math::Colors::Red), "High load warning on change !!");
	ImGui::Spacing();

	auto& job = m_player->GetVideoJob();
	const auto oldDesc = job->GetDesc();
	const auto videoInfo = m_player->GetVideoInfo();

	ImGui::SliderInt("Quadtree Depth [!]",		reinterpret_cast<int*>(&m_quadTreeDepth), 1, QuadTreeMaxDepth);
	if (ImGui::IsItemDeactivatedAfterEdit())
		job->SetQuadTreeDepth(m_quadTreeDepth);

	ImGui::DragInt("Frame/Segment",			reinterpret_cast<int*>(&m_framePerSegment), 1, videoInfo.frameCount);
	if (ImGui::IsItemDeactivatedAfterEdit())
		job->SetFramePerSegment(m_framePerSegment);
	
	ImGui::DragInt("Keep load segment",		reinterpret_cast<int*>(&m_loadSegmentCount), 1, videoInfo.frameCount / oldDesc.framePerSegment);
	if (ImGui::IsItemDeactivatedAfterEdit())
		job->SetKeepLoadSegmentCount(m_loadSegmentCount);

}
