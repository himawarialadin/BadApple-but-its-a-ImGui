#include "MainDokingBuilder.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

MainDockingBuilder::MainDockingBuilder()
{
}

void MainDockingBuilder::Dock()
{
	m_dockSpaceID = ImGui::GetID("Main_DockSpace");

	//
	auto dockspace_flags	= ImGuiDockNodeFlags_PassthruCentralNode;
	auto viewport				=	ImGui::GetMainViewport();
	{
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags host_window_flags = 0;
		host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
		host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		host_window_flags |= ImGuiWindowFlags_NoBackground;
		

		char label[32];
		sprintf_s(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0, 0, 0, 0 });
		ImGui::Begin(label, NULL, host_window_flags);
		ImGui::PopStyleVar(3);
	}

	//
	{
		for (auto& windowName : m_dockWindowNames)
			ImGui::DockBuilderDockWindow(windowName.c_str(), m_dockSpaceID);

		ImGui::DockBuilderFinish(m_dockSpaceID);
	}

	//
	{
		ImGui::DockSpace(m_dockSpaceID, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);
		ImGui::End();
		ImGui::PopStyleColor();
	}

}

void MainDockingBuilder::RegisterDock(std::string_view windowName)
{
	m_dockWindowNames.emplace_back(windowName);
}
