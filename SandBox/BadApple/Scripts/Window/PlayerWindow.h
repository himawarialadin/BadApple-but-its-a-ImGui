#pragma once
#include "IWindow.h"
#include <string>

class AnimationPlayer;
class PlayerWindow : public IWindow
{
public:
	void SetPlayer(AnimationPlayer* player) { m_player = player; }
	void Show() override;

private:

	void DrawSeekBar();
	void DrawStreamInfo();
	void DrawControls();

	void DrawStrem();

private:
	std::string m_currentTimeText	= "00:00";
	std::string m_totalTimeText		= "00:00";

	AnimationPlayer* m_player = nullptr;
};

