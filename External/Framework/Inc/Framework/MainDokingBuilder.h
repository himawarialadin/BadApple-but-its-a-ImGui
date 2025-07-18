#pragma once
#include <ImGui/imgui.h>
#include <string>
#include <vector>

class MainDockingBuilder
{
public:
	MainDockingBuilder();

	void Dock();
	void RegisterDock(std::string_view windowName);

private:
	ImGuiID							m_dockSpaceID = 0;
	std::vector<std::string>	m_dockWindowNames;
};

