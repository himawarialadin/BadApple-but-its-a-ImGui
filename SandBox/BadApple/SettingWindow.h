#pragma once
#include "WindowBase.h"
#include "Common.h"
#include <cstdint>
#include <memory>

class IMode;
class VideoWindow;
class AnimationPlayer;

class SettingWindow : public IWindow
{
public:
	SettingWindow();
	~SettingWindow() override;

	void SetModeBuilders(ModeBuildFuncTable* builders) { m_modeBuilders = builders; }
	void SetVideoWindow(VideoWindow* videoWindow) { m_videoWindow = videoWindow; }
	void SetAnimationPlayer(AnimationPlayer* player);

	std::unique_ptr<IMode> const& GetMode() const { return m_mode; }

private:
	void Show() override;
	void ShowAnimationSettings();
	void ShowProcessSettings();

private:
	ModeBuildFuncTable*			m_modeBuilders		= nullptr; 
	VideoWindow*					m_videoWindow			= nullptr;
	AnimationPlayer*				m_player					= nullptr;

	uint32_t							m_modeIndex			= 0;
	std::unique_ptr<IMode>	m_mode					= nullptr;


	uint32_t								m_quadTreeDepth			= 1;
	uint32_t								m_framePerSegment		= 120;
	uint32_t								m_loadSegmentCount		= 3;	
};