#include "VideoWindow.h"
#include "AnimationPlayer.h"
#include "VideoProcessingJob.h"

#include <opencv2/opencv.hpp>
#include "Common.h"
#include "Constant.h"

#include <string>
#include <Windows.h>

VideoWindow::VideoWindow()
{
}

VideoWindow::~VideoWindow()
{
}

bool VideoWindow::Initialize(std::string_view filename)
{
	m_videoCapture		= std::make_unique<cv::VideoCapture>(VideoFileName.data());
	m_currentFrame	= std::make_unique<cv::Mat>();

	if (!m_videoCapture->isOpened())
		return false;

	return true;
}

void VideoWindow::Show()
{
	const std::string_view WindowName = "Original";

	if (!m_showing)
	{
		if(m_windowAlive)
		{
			cv::destroyWindow(WindowName.data());
			m_windowAlive = false;
		}

		return;
	}
	

	m_videoCapture->set(cv::CAP_PROP_POS_FRAMES, static_cast<double>(m_player->GetVideoJob()->GetSeekPosition()));
	m_videoCapture->read(*m_currentFrame);
	cv::imshow(WindowName.data(), *m_currentFrame);
	m_windowAlive = true;

	HWND hwnd = FindWindowA(NULL, WindowName.data());
	if (hwnd)
	{
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}

}
