#pragma once
#include "Common.h"
#include "QuadTree.h"
#include <memory>
#include <Utility/JobSystem/JobHandle.h>
#include <fmod/fmod.hpp>

class VideoProcessingJob;
struct VideoInfo;

class AnimationPlayer
{
	friend class PlayerWindow;
public:
	AnimationPlayer();
	~AnimationPlayer();

public:
	bool Initialize(std::string_view videoFilename, std::string_view audioFilename);

public:
	std::vector<ColoredNode>						const&	GetCurrentFrame()		const { return m_currentFrame; }
	std::unique_ptr<VideoProcessingJob>		const&	GetVideoJob()			const { return m_videoJob; }
	VideoInfo												const&	GetVideoInfo()			const;

	Math::Vector2													GetFrameSize()			const;
	bool 																	IsPaused()					const { return m_isPaused; }
	bool																	IsPlaying()					const { return m_isPlaying && !m_isPaused; }
	float 																	GetPlayTime()			const;
	void																	SetPlayTime(float time);

	void Update();
	
	void Play();
	void Stop();
	void Pause();
	void Resume();
	

private:
	std::unique_ptr<VideoProcessingJob>	m_videoJob;				//ビデオ処理ジョブ
	Utility::JobHandle								m_videoJobHandle;	//ビデオ処理ジョブのハンドル
	std::vector<ColoredNode>					m_currentFrame;
	bool													m_isPlaying			= false;	//再生中かどうか
	bool 													m_isPaused			= false;	//一時停止中かどうか
	float													m_currentTime		= 0.0f;	//現在の再生時間

private:
	FMOD::System*		m_soundSystem;
	FMOD::Sound*		m_sound;
	FMOD::Channel*	m_channel;
	
};

