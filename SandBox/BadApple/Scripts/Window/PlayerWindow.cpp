#include "PlayerWindow.h"
#include "AnimationPlayer.h"
#include "VideoProcessingJob.h"
#include <Math/Color.h>
#include <Math/Math.h>
#include <Math/Vector2.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <Utility/String.h>
#include <Math/Converter/ImVec.h>
#include <format>
#include <vector>

const float				PlotSpacing			= 1.25f;
const float				HistogramHeight	= 30.0f;
const float				SeekBarThickness	= 2.25f;
const float				DefaultItemSize		= 1000.0f;

const Math::Color	LoadSegColor			= Math::Colors::GreenYellow;
const Math::Color	SeekSegColor			= Math::Colors::Cyan;
const Math::Color	UnloadedSegColor	= Math::Colors::Gray;
const Math::Color	SeekBarColor			= Math::Colors::Red;

static std::string TimeToString(float timeInSeconds)
{
	int minutes = static_cast<int>(timeInSeconds / 60);
	int seconds = static_cast<int>(timeInSeconds) % 60;
	return std::format("{:02}:{:02}", minutes, seconds);
}


void PlayerWindow::Show()
{
	m_currentTimeText		= TimeToString(m_player->GetPlayTime());
	m_totalTimeText		= TimeToString(m_player->GetVideoInfo().durationSec);

	if (ImGui::Begin("Player"))
	{
		DrawSeekBar();
		DrawStreamInfo();
		DrawControls();

	}
	ImGui::End();
}

void PlayerWindow::DrawSeekBar()
{
	ImGui::BeginHorizontal("SeekBar", { ImGui::GetContentRegionAvail().x, 0.0f });

	ImGui::Text(m_currentTimeText.c_str());
	ImGui::Spring(0.0f);

	{
		ImVec2 size = ImGui::CalcTextSize(m_totalTimeText.c_str());
		size.x += ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetNextItemWidth(-size.x);

		float time = m_player->GetPlayTime();
		if (ImGui::SliderFloat("##Seek", &time, 0.0f, m_player->GetVideoInfo().durationSec))
		{
			m_player->SetPlayTime(time);
		}
	}

	ImGui::Text(m_totalTimeText.c_str());
	ImGui::Spring(0.0f);

	ImGui::EndHorizontal();
}

void PlayerWindow::DrawStreamInfo()
{
	const float HistogramHeight = 30.0f;
	ImGui::BeginHorizontal("Stream", { ImGui::GetContentRegionAvail().x, 0.0f });
	
	ImGui::Text(m_currentTimeText.c_str());
	ImGui::Spring(0.0f);

	DrawStrem();

	ImGui::Spring(0.0f);
	ImGui::Text(m_totalTimeText.c_str());

	ImGui::EndHorizontal();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
}

void PlayerWindow::DrawControls()
{
	const ImVec2 SkipButtonSize = { 0.0f, 30.0f };
	const ImVec2 PlayButtonSize = { 40.0f, 40.0f };
	
	std::string playButtonText = m_player->IsPlaying() ? 
		reinterpret_cast<const char*>(u8"\u2759\u2759") :
		reinterpret_cast<const char*>(u8"\u25B6");


	ImGui::BeginHorizontal("Buttons", { ImGui::GetContentRegionAvail().x, 0.0f });

	ImGui::Spring(0.5f);
	if (ImGui::Button("<<", SkipButtonSize))
	{
		m_player->SetPlayTime(0.0f); // シーク位置を先頭に戻す
		m_player->Stop();
	}
	
	ImGui::Spring(0.0f);

	ImGui::PushFont(ImGui::GetIO().FontDefault, 30.0f);
	if (ImGui::Button(playButtonText.c_str(), PlayButtonSize))
	{
		if(m_player->IsPaused())
			m_player->Resume();
		else if(m_player->IsPlaying())
			m_player->Pause();
		else
			m_player->Play();

	}
	ImGui::PopFont();
	
	ImGui::Spring(0.0f);
	if (ImGui::Button(">>", SkipButtonSize))
	{
		m_player->SetPlayTime(m_player->GetVideoInfo().durationSec); 
		m_player->Stop();
	}

	ImGui::Spring(0.5f);
	ImGui::EndHorizontal();
}

void PlayerWindow::DrawStrem()
{

	//フレーム情報
	auto&	videoJob					= m_player->GetVideoJob();
	auto&	jobDesc						= videoJob->GetDesc();
	auto&	videoInfo					= m_player->GetVideoInfo();
	U32Pair frameRange				= videoJob->GetCurrentFrameRange();
	
	const uint32_t maxSegmentCount	= videoInfo.frameCount / jobDesc.framePerSegment + 1;
	const uint32_t seekPosSeg				= videoJob->GetSeekPosition() / jobDesc.framePerSegment;
	const uint32_t beginSeg					= frameRange.first / jobDesc.framePerSegment;
	const uint32_t endSeg					= frameRange.second / jobDesc.framePerSegment;
	
	{
		ImVec2 size = ImGui::CalcTextSize(m_totalTimeText.c_str());
		size.x += ImGui::GetStyle().ItemSpacing.x;
		if (!ImGui::BeginChild("Stream2", { -size.x, HistogramHeight }, ImGuiChildFlags_Border))
		{
			ImGui::EndChild();
			return;
		}
	}

	auto		drawList	= ImGui::GetForegroundDrawList();
	auto&	style			= ImGui::GetStyle();
	
	const float					itemWidth				= ImGui::GetItemRectSize().x;
	const Math::Vector2	windowMin			= Math::Cast<Math::Vector2>(ImGui::GetItemRectMin()) + Math::Vector2(style.ChildBorderSize, style.ChildBorderSize);
	const Math::Vector2	windowMax			= windowMin + Math::Vector2(itemWidth, HistogramHeight) - Math::Vector2(style.ChildBorderSize, style.ChildBorderSize);
	const float					plotSpacing			= PlotSpacing * (itemWidth / DefaultItemSize);
	const float					seekBarThickness	= SeekBarThickness * (itemWidth / DefaultItemSize);
	const float					plotWidth				= (itemWidth - (plotSpacing * (maxSegmentCount - 1))) / (maxSegmentCount);

	// セグメントの読み込み状態に応じて色を設定
	float offsetX = 0.0f;
	for(uint32_t i = 0; i < maxSegmentCount; i++)
	{
		Math::Vector2 segmentBegin	= Math::Vector2{ windowMin.x + offsetX , windowMin.y };
		Math::Vector2 segmentEnd		= Math::Vector2{ segmentBegin.x + plotWidth, windowMax.y };
		Math::Color color = UnloadedSegColor;
		
		if (i >= beginSeg && i <= endSeg)
			color = LoadSegColor; //読み込み済みのセグメント
		if (i == seekPosSeg)
			color = SeekSegColor; //現在のシーク位置のセグメント
		
		drawList->AddRectFilled(Math::Cast<ImVec2>(segmentBegin), Math::Cast<ImVec2>(segmentEnd), color.ToABGR32ColorCode());
		offsetX += plotWidth + plotSpacing;


		if (i == seekPosSeg)
		{
			uint32_t seekPosition					= videoJob->GetSeekPosition();
			uint32_t segmentBeginFrame		= i * jobDesc.framePerSegment;
			uint32_t segmentEndFrame		= std::min(segmentBeginFrame + jobDesc.framePerSegment, videoInfo.frameCount);

			float positionX = Math::Remap(static_cast<float>(seekPosition),
				static_cast<float>(segmentBeginFrame), static_cast<float>(segmentEndFrame),
				segmentBegin.x, segmentEnd.x);

			Math::Vector2 seekBarBegin	= Math::Vector2{ positionX - seekBarThickness / 2.0f,  windowMin.y };
			Math::Vector2 seekBarEnd		= Math::Vector2{ positionX + seekBarThickness / 2.0f, windowMax.y };
			drawList->AddRectFilled(Math::Cast<ImVec2>(seekBarBegin), Math::Cast<ImVec2>(seekBarEnd), SeekBarColor.ToABGR32ColorCode());
		}
	}


	ImGui::EndChild();
	
}
