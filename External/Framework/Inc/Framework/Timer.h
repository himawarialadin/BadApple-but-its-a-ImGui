#pragma once
#include <chrono>

class Timer
{
public:
	static void Update()
	{
		auto currentTime = Clock::now();
		g_deltaTime		= std::chrono::duration<float>(currentTime - s_prevTime).count();
		s_prevTime		= currentTime;

		if (g_deltaTime > 0.1f)
			g_deltaTime = 0.1f;
	
		g_totalTime += g_deltaTime;
	}

	static void SetTimeScale(float scale)
	{ g_scaleTime = scale; }

	static float TimeScale() { return g_scaleTime; }
	static float DeltaTime() { return g_deltaTime * g_scaleTime; }
	static float TotalTime() { return g_totalTime; }
	static float FPS()			{ return 1.0f / g_deltaTime; }
private:
	using Clock = std::chrono::steady_clock;

	inline static Clock::time_point s_prevTime;
	inline static float					g_deltaTime;
	inline static float					g_totalTime;
	inline static float					g_scaleTime = 1.0f;
};