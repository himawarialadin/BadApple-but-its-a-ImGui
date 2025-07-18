#include "RectangleMode.h"
#include "Common.h"
#include "Constant.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui.h>


#include <Math/Math.h>
#include <Math/Vector2.h>
#include <Math/Vector2Int.h>
#include <Math/Converter/ImVec.h>

void RectangleMode::ShowSettings()
{
	ImGui::Checkbox("Keep Aspect Ratio", &m_isKeepAspectRatio);
	ImGui::Checkbox("Fill Outside", &m_isFillOutside);
}

void RectangleMode::Show()
{
	auto& style = ImGui::GetStyle();
	m_backgroundColor = ImGui::GetColorU32(ImGuiCol_WindowBg);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
	if(ImGui::Begin("Bad Apple!!"))
	{
		m_windowMin = Math::Cast<Math::Vector2>(ImGui::GetCursorScreenPos()) - Math::Cast<Math::Vector2>(style.WindowPadding);
		m_windowMax = Math::Cast<Math::Vector2>(ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail()) + Math::Cast<Math::Vector2>(style.WindowPadding);

		Math::Vector2 windowSize = m_windowMax - m_windowMin;
		if (m_isKeepAspectRatio)
		{
			m_displaySize = CalculateDisplaySize(windowSize.x, windowSize.y, m_data.size.x, m_data.size.y);
			m_displayPos = m_windowMin + (windowSize - m_displaySize) * 0.5f;
		}
		else
		{
			m_displaySize	= windowSize;
			m_displayPos		= m_windowMin;
		}


		//表示領域以外は塗りつぶしておく
		if(m_isKeepAspectRatio && m_isFillOutside)
			DrawOutside();

		Draw();
	}
	ImGui::End();

	ImGui::PopStyleColor(1);
}


void RectangleMode::DrawOutside()
{
	auto drawList = ImGui::GetForegroundDrawList();
	drawList->AddRectFilled(
		Math::Cast<ImVec2>(m_windowMin),
		Math::Cast<ImVec2>(Math::Vector2(m_displayPos.x, m_windowMax.y)),
		m_backgroundColor
	);

	drawList->AddRectFilled(
		Math::Cast<ImVec2>(m_displayPos + Math::Vector2(m_displaySize.x, 0)),
		Math::Cast<ImVec2>(m_windowMax),
		m_backgroundColor
	);

	drawList->AddRectFilled(
		Math::Cast<ImVec2>(m_windowMin),
		Math::Cast<ImVec2>(m_displayPos + Math::Vector2(m_displaySize.x, 0)),
		m_backgroundColor
	);

	drawList->AddRectFilled(
		Math::Cast<ImVec2>(m_displayPos + Math::Vector2(0, m_displaySize.y)),
		Math::Cast<ImVec2>(m_windowMax),
		m_backgroundColor
	);
}

void RectangleMode::Draw()
{
	if(m_data.whiteNodes == nullptr)
		return; // ノードがない場合は何もしない

	auto drawList = ImGui::GetForegroundDrawList();

	auto& mapSize = m_data.size;

	const uint32_t count = static_cast<uint32_t>(m_data.whiteNodes->size());
	for (uint32_t i = 0; i < count; i++)
	{
		auto& colorMap	= m_data.whiteNodes->at(i);
		auto& nodePos	= colorMap.node->position;
		auto& nodeSize	= colorMap.node->size;
		
		Math::Vector2 nodeDisplayPos;
		Math::Vector2 nodeDisplaySize;
		CalculateNodeDisplaySize(
			*colorMap.node,
			mapSize,
			m_displayPos,
			m_displayPos + m_displaySize,
			&nodeDisplayPos,
			&nodeDisplaySize
		);

		ImVec2 pixelMin = Math::Cast<ImVec2>(nodeDisplayPos);
		ImVec2 pixelMax = Math::Cast<ImVec2>(nodeDisplayPos + nodeDisplaySize);
		if (m_data.showTree)
		{
			auto color = QuadTreeColorsToColor(*colorMap.node);
			drawList->AddRectFilled(pixelMin, pixelMax, color.ToABGR32ColorCode());
			drawList->AddRect(pixelMin, pixelMax, Math::Colors::Black.ToABGR32ColorCode(), 0.0f, 0, QuadTreeColorFrameThickness);
		}
		else
		{
			drawList->AddRectFilled(
				pixelMin,
				pixelMax,
				Math::Colors::White.ToABGR32ColorCode()
			);

		}
	}

}
