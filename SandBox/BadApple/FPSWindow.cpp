#include "FPSWindow.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

FPSWindow::FPSWindow()
{
	m_fpsHistory.resize(FPSHistorySize, 0.0f);
	m_fpsHistoryIndex = 0;
	m_time = 0.0f;
	m_fps = 0.0f;
}

void FPSWindow::CalculateFPS(float deltaTime)
{
	m_time += deltaTime;
	if (m_time >= 1.0f)
	{
		m_fps	= deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
		m_time = 0.0f;
	}

	m_fpsHistory[m_fpsHistoryIndex] = deltaTime * 1000.0f;
	m_fpsHistoryIndex = (m_fpsHistoryIndex + 1) % FPSHistorySize;
}

void FPSWindow::Show()
{
	auto context = ImGui::GetCurrentContext();
	if (ImGui::Begin("FPS"))
	{
		float deltaTime		= m_fps > 0.0f ? 1.0f / m_fps : 0.0f;
		float oldFontScale	= context->FontSize;

		context->FontSize = oldFontScale * 1.5f;
		ImGui::Text("FPS: %.2f (%.2f ms)", m_fps, deltaTime * 1000.0f);
		context->FontSize = oldFontScale;
		
		ImGui::NewLine();
		ImGui::SeparatorText("FPSHistory");
		ImGui::PlotLines("##FPSHistory", 
			m_fpsHistory.data(), 
			FPSHistorySize, 
			m_fpsHistoryIndex, 
			nullptr, 
			0.0f, 
			100.0f, 
			ImVec2(-1.0f, 80.0f));

	}
	ImGui::End();
}
