#include "AnimationPlayer.h"
#include "VideoProcessingJob.h"
#include "Constant.h"

#include <Utility/Singleton.h>
#include <Utility/JobSystem/JobSystem.h>
#include <opencv2/opencv.hpp>

AnimationPlayer::AnimationPlayer()
{
}

AnimationPlayer::~AnimationPlayer()
{
	if (m_sound)
	{
		m_sound->release(); // サウンドの解放
		m_sound = nullptr;
	}

	m_soundSystem->release(); // FMODシステムの解放
}

bool AnimationPlayer::Initialize(std::string_view videoFilename, std::string_view audioFilename)
{
	auto& jobSystem = Utility::Singleton::Instance<Utility::JobSystem>();
	m_videoJob = std::make_unique<VideoProcessingJob>();

	if (!m_videoJob->Initialize({
		.videoFilename			= videoFilename.data(),
		.quadTreeDepth			= 8,

		.framePerSegment		= 120,
		.loadSegmentCount	= 3,
		}))
	{
		return false;
	}

	FMOD::System_Create(&m_soundSystem);
	m_soundSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	if (m_soundSystem->createSound(audioFilename.data(), FMOD_DEFAULT, nullptr, &m_sound) != FMOD_OK)
		return false;

	return true;
}

Math::Vector2 AnimationPlayer::GetFrameSize() const
{
	return m_videoJob->GetVideoInfo().frameSize;
}

float AnimationPlayer::GetPlayTime() const
{
	uint32_t	seekPos		= m_videoJob->GetSeekPosition();
	float			fps			= m_videoJob->GetVideoInfo().fps;
	
	if(fps == 0.0f)
		return 0.0f; // FPSが0の場合は再生時間を0にする

	return static_cast<float>(seekPos) / fps; 
}

void AnimationPlayer::SetPlayTime(float time)
{
	if (time < 0.0f || time > m_videoJob->GetVideoInfo().durationSec)
		return; // 時間が範囲外の場合は何もしない

	uint32_t seekPos = static_cast<uint32_t>(time * m_videoJob->GetVideoInfo().fps);
	m_videoJob->MoveSeek(seekPos, false); // 非同期でシーク
	m_currentTime	= time;
	m_channel->setPosition(static_cast<uint32_t>(m_currentTime * 1000), FMOD_TIMEUNIT_MS);
}

VideoInfo const& AnimationPlayer::GetVideoInfo() const
{
	return m_videoJob->GetVideoInfo();
}

void AnimationPlayer::Update()
{
	auto&		videoInfo		= m_videoJob->GetVideoInfo();
	uint32_t	frameIndex	= static_cast<uint32_t>(m_currentTime * videoInfo.fps);
	if(frameIndex >= videoInfo.frameCount - 1)
	{
		Stop(); // 再生が終了した場合は停止
		return;
	}

	m_currentFrame = m_videoJob->FetchColorMapByFrame(frameIndex);

	if (IsPlaying())
	{
		uint32_t soundCurrentPos = 0;
		m_channel->getPosition(&soundCurrentPos, FMOD_TIMEUNIT_MS);
		m_currentTime = soundCurrentPos / 1000.0f; // Convert milliseconds to seconds
	}
}

void AnimationPlayer::Play()
{
	if (m_isPlaying) return;

	m_isPlaying = true;

	m_channel->stop();
	m_soundSystem->playSound(m_sound, nullptr, false, &m_channel);
	m_channel->setPosition(static_cast<uint32_t>(m_currentTime * 1000), FMOD_TIMEUNIT_MS); // Set position in milliseconds
}

void AnimationPlayer::Stop()
{
	m_isPlaying = false;
	m_isPaused = false;

	m_channel->stop();
	m_channel->setPaused(false);
}

void AnimationPlayer::Pause()
{
	m_isPaused = true;
	m_channel->setPaused(true);
}

void AnimationPlayer::Resume()
{
	m_isPaused = false;
	m_channel->setPaused(false);
}
