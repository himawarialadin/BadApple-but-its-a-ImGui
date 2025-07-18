#include "PlayerWindow.h"
#include "AnimationPlayer.h"
#include "VideoProcessingJob.h"
#include <Math/Color.h>
#include <Math/Math.h>
#include <ImPlot/implot.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <Utility/String.h>

#include <format>
#include <vector>

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
	const float PlotWidth				= 1.0f;
	const float PlotSpacing			= 0.05f;
	const float HistogramHeight	= 30.0f;
	const float SeekBarThickness	= 0.1f;

	const Math::Color LoedSegColor			= Math::Colors::GreenYellow;
	const Math::Color SeekSegColor			= Math::Colors::Cyan;
	const Math::Color UnloadedSegColor	= Math::Colors::Gray;
	const Math::Color SeekBarColor			= Math::Colors::Red;

	ImVec2 size = ImGui::CalcTextSize(m_totalTimeText.c_str());
	size.x += ImGui::GetStyle().ItemSpacing.x;

	auto	drawList = ImPlot::GetPlotDrawList();
	auto&	videoJob					= m_player->GetVideoJob();
	auto&	videoInfo					= m_player->GetVideoInfo();
	auto&	jobDesc						= videoJob->GetDesc();
	U32Pair frameRange				= videoJob->GetCurrentFrameRange();

	uint32_t maxSegmentCount	= videoInfo.frameCount / jobDesc.framePerSegment;
	uint32_t seekPosSeg				= videoJob->GetSeekPosition() / jobDesc.framePerSegment;
	uint32_t beginSeg					= frameRange.first / jobDesc.framePerSegment;
	uint32_t endSeg						= frameRange.second / jobDesc.framePerSegment;

	if (!ImPlot::BeginPlot("Stream2", { -size.x, HistogramHeight }, ImPlotFlags_CanvasOnly))
		return;

	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, { 0, 0 });
	ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_NoHighlight);
	ImPlot::SetupAxesLimits(0, (maxSegmentCount + 1) + PlotSpacing * ( maxSegmentCount + 1), 1.0, 0, ImPlotCond_Always);
	

	float offsetX = 0.0f;
	for(uint32_t i = 0; i <= maxSegmentCount; i++)
	{
		float segmentBeginX	= offsetX;
		float segmentEndX		= offsetX + PlotWidth;
		
		ImPlot::PushPlotClipRect();

		// セグメントの読み込み状態に応じて色を設定
		{
			ImVec2 pixelMin		= ImPlot::PlotToPixels(segmentBeginX, 0.0);
			ImVec2 pixelMax	= ImPlot::PlotToPixels(segmentEndX, 1.0);

			Math::Color color = UnloadedSegColor;

			if (i >= beginSeg && i <= endSeg)
				color = LoedSegColor; //読み込み済みのセグメント
			if (i == seekPosSeg)
				color = SeekSegColor; //現在のシーク位置のセグメント

			drawList->AddRectFilled(pixelMin, pixelMax, color.ToABGR32ColorCode());
		}

		//シークバー
		if (i == seekPosSeg)
		{
			uint32_t	seekPosBeginFrame	= seekPosSeg * jobDesc.framePerSegment;
			uint32_t	seekPosEndFrame		= std::min((seekPosSeg + 1) * jobDesc.framePerSegment, videoInfo.frameCount);
			float			positionX					= Math::Remap(static_cast<float>(videoJob->GetSeekPosition()), 
				static_cast<float>(seekPosBeginFrame), static_cast<float>(seekPosEndFrame),
				segmentBeginX, segmentEndX);

			ImVec2 pixelMin		= ImPlot::PlotToPixels(positionX - SeekBarThickness / 2.0f, 0.0);
			ImVec2 pixelMax	= ImPlot::PlotToPixels(positionX + SeekBarThickness / 2.0f, 1.0);
			drawList->AddRectFilled(pixelMin, pixelMax, SeekBarColor.ToABGR32ColorCode());
		}
		ImPlot::PopPlotClipRect();

		offsetX += PlotWidth + PlotSpacing;
	}
	ImPlot::PopStyleVar();

	ImPlot::EndPlot();
	
}
