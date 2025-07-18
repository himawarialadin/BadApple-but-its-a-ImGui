#include "ImGuiWindowMode.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui.h>

#include "Common.h"
#include "Constant.h"
#include <Math/Converter/ImVec.h>

ImGuiWindowMode::ImGuiWindowMode()
{
	auto& style = ImGui::GetStyle();
	m_windowTitleColor		= Math::Cast<Math::Color>(style.Colors[ImGuiCol_Text]);
	m_windowTitleBarColor	= Math::Cast<Math::Color>(style.Colors[ImGuiCol_TitleBg]);
	m_windowFontSize			= style.FontSizeBase;
	m_windowFontSizeMin	= style.FontSizeBase;
}

void ImGuiWindowMode::Show()
{
	auto& style = ImGui::GetStyle();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
	if (ImGui::Begin(DisplayWindowName.data()))
	{
		Math::Vector2 windowMin		= Math::Cast<Math::Vector2>(ImGui::GetCursorScreenPos()) - Math::Cast<Math::Vector2>(style.WindowPadding);
		Math::Vector2 windowMax		= Math::Cast<Math::Vector2>(ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail()) + Math::Cast<Math::Vector2>(style.WindowPadding);
		Math::Vector2 windowSize		= windowMax - windowMin;

		if (m_isKeepAspectRatio)
		{
			m_displaySize = CalculateDisplaySize(windowSize.x, windowSize.y, m_data.size.x, m_data.size.y);
			m_displayPos = windowMin + (windowSize - m_displaySize) * 0.5f;
		}
		else
		{
			m_displaySize = windowSize;
			m_displayPos = windowMin;
		}

		Draw();
	}
	ImGui::End();
	ImGui::PopStyleColor(1);
}

void ImGuiWindowMode::ShowSettings()
{
	ImGui::Checkbox("Keep aspect ratio", &m_isKeepAspectRatio);
	ImGui::Checkbox("Show title bar", &m_showTitleBar);
	ImGui::Checkbox("Hide windows smaller than the title bar", &m_hideTinyWindows);
	ImGui::DragFloat("Titlebar Font Size", &m_windowFontSize, 0.1f, 1.0f, m_windowFontSizeMin);

	ImGui::ColorEdit3("Title bar color", &m_windowTitleBarColor.R);
	ImGui::ColorEdit3("Title text color", &m_windowTitleColor.R);
}

void ImGuiWindowMode::Draw()
{
	if (m_data.whiteNodes == nullptr)
		return; // ÉmÅ[ÉhÇ™Ç»Ç¢èÍçáÇÕâΩÇ‡ÇµÇ»Ç¢


	auto					drawList	= ImGui::GetForegroundDrawList();
	const auto&		mapSize	= m_data.size;
	const	 uint32_t	count			= static_cast<uint32_t>(m_data.whiteNodes->size());

	ImGui::PushFont(ImGui::GetFont(), m_windowFontSize);
	ImGui::PushStyleColor(ImGuiCol_TitleBg,		m_windowTitleBarColor.ToABGR32ColorCode());
	ImGui::PushStyleColor(ImGuiCol_Text,			m_windowTitleColor.ToABGR32ColorCode());
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

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
			ImGui::PushStyleColor(ImGuiCol_WindowBg, Math::Cast<ImVec4>(color));
			ShowWindow(i, nodeDisplayPos, nodeDisplaySize);
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, { 1.0f, 1.0f, 1.0f, 1.0f });
			ShowWindow(i, nodeDisplayPos, nodeDisplaySize);
			ImGui::PopStyleColor();
		}
	}

	ImGui::PopFont();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(2);
}

void ImGuiWindowMode::ShowWindow(uint32_t index, Math::Vector2 const& windowPos, Math::Vector2 const& windowSize)
{
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoFocusOnAppearing;

	auto& style = ImGui::GetStyle();
	float titleBarHeight = style.FontSizeBase + style.FramePadding.y * 2.0f;
	if (m_showTitleBar && m_hideTinyWindows && titleBarHeight > windowSize.y)
		return;

	if (!m_showTitleBar)
		windowFlags |= ImGuiWindowFlags_NoTitleBar;

	std::string	name = "Node##" + std::to_string(index);
	ImGui::SetNextWindowPos(Math::Cast<ImVec2>(windowPos), ImGuiCond_Always);
	ImGui::SetNextWindowSize(Math::Cast<ImVec2>(windowSize), ImGuiCond_Always);
	ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);

	ImGui::Begin(name.c_str(), nullptr, windowFlags);
	ImGui::End();
}
