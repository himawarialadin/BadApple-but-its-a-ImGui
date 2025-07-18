#pragma once
#include "WindowBase.h"
#include <vector>

class FPSWindow : public IWindow
{
public:
	FPSWindow();
	~FPSWindow() override = default;

	void CalculateFPS(float deltaTime);
	void Show() override;

private:
	constexpr static uint32_t FPSHistorySize = 100;

	std::vector<float> m_fpsHistory;
	uint32_t				m_fpsHistoryIndex = 0;
	float m_time	= 0.0f;
	float m_fps	= 0.0f;
};