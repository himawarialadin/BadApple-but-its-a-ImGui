#pragma once
#include "WindowBase.h"
#include <memory>
#include <string>


namespace cv
{
	class VideoCapture;
	class Mat;
}
class VideoProcessingJob;
class AnimationPlayer;

class VideoWindow : public IWindow
{
public:
	VideoWindow();
	~VideoWindow() override;

	bool Initialize(std::string_view filename);
	void SetAnimationPlayer(AnimationPlayer* player) { m_player = player; }
	void SetShowing(bool showing) { m_showing = showing; }

	bool IsShowing() const { return m_showing; }

public:
	void Show() override;

private:
	AnimationPlayer*								m_player = nullptr; // �r�f�I�����W���u
	bool													m_showing = false; // �E�B���h�E���\������Ă��邩�ǂ���
	bool													m_windowAlive = false;

	std::unique_ptr<cv::VideoCapture>	m_videoCapture; // OpenCV�̃r�f�I�L���v�`��
	std::unique_ptr<cv::Mat>					m_currentFrame; // ���݂̃t���[��
};