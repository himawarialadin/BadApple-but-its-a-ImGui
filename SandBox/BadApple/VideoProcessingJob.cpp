#include "VideoProcessingJob.h"
#include <opencv2/opencv.hpp>
#include <Utility/Singleton.h>
#include <Utility/JobSystem/JobSystem.h>

/*!
@brief 閉区間の差集合（区間[A, B] - 区間[C, D]）を求める

@return 
差集合の結果 [first, secound] の形式で返す 
・区間が重ならない場合は [A, B] を返す
・区間が完全に包含される場合と部分的に重なる場合は std::nullopt を返す

@exception std::runtime_error 引数の値が不正な場合(例えば A > B, C > D など)
*/
static constexpr std::optional<std::pair<uint32_t, uint32_t>> RangeDiff(uint32_t A, uint32_t B, uint32_t C, uint32_t D)
{

	if(A > B || C > D)
		throw std::runtime_error("Invalid range: A > B or C > D");

	// 区間[C, D] が [A, B] の完全外にある場合（重ならない）
	if (D < A || B < C)
		return std::make_pair( A, B );

	// 区間[C, D] が [A, B] を完全に含む、または区間全体に重なる
	if (C <= A && B <= D)
		return std::nullopt;

	// 部分的に左側が重なる: [C, D] が [A, B] の左を削るような場合
	if (C <= A && D < B)
		return std::make_pair( D + 1, B );

	// 部分的に右側が重なる: [C, D] が [A, B] の右を削るような場合
	if (A < C && B <= D)
		return std::make_pair( A, C - 1 );

	// 中央部を削るような部分的重なり（[A, C-1], [D+1, B] のような形）は対象外
	return std::nullopt;
}


VideoProcessingJob::VideoProcessingJob()
{
}

VideoProcessingJob::~VideoProcessingJob()
{
}

bool VideoProcessingJob::Initialize(Desc const& desc)
{
	m_desc = desc;
	m_capture = std::make_unique<cv::VideoCapture>(m_desc.videoFilename);
	if (!m_capture->isOpened())
	{
		return false; // ビデオファイルが開けない場合は失敗
	}

	// Initialize video information
	{
		m_videoInfo.fps					= static_cast<float>(m_capture->get(cv::CAP_PROP_FPS));
		m_videoInfo.frameCount		= static_cast<uint32_t>(m_capture->get(cv::CAP_PROP_FRAME_COUNT));
		m_videoInfo.frameSize.x		= static_cast<float>(m_capture->get(cv::CAP_PROP_FRAME_WIDTH));
		m_videoInfo.frameSize.y		= static_cast<float>(m_capture->get(cv::CAP_PROP_FRAME_HEIGHT));
		m_videoInfo.durationSec	= m_videoInfo.frameCount / m_videoInfo.fps;
	}

	m_tempColorBuffer	= std::make_unique<unsigned char[]>(static_cast<size_t>(m_videoInfo.frameSize.x * m_videoInfo.frameSize.y));
	m_tree.Create({ m_videoInfo.frameSize, m_desc.quadTreeDepth });

	MoveSeek(0, false); // 初期化時に先頭フレームを読み込む
	return true;
}

void VideoProcessingJob::SetQuadTreeDepth(uint32_t depth)
{
	if (depth == 0)
		throw std::runtime_error("QuadTree depth must be greater than 0");

	if (m_desc.quadTreeDepth == depth)
		return;

	//生成ジョブが完了していない場合は完了を待つ
	if (!m_jobHandle.IsEmpty())
		m_jobHandle.Complate();

	m_desc.quadTreeDepth = depth;

	// クアッドツリーの再生成とフレーム範囲の初期化
	m_tree.Create({ m_videoInfo.frameSize, m_desc.quadTreeDepth });
	m_currentFrameRange.store({ 0, 0 }, std::memory_order_release);
	m_segmentParts.clear();

	MoveSeek(m_seekPos, true); // 深度変更後に再読み込み
}

void VideoProcessingJob::SetFramePerSegment(uint32_t frame)
{
	if(m_desc.framePerSegment == frame)
		return;

	//生成ジョブが完了していない場合は完了を待つ
	if (!m_jobHandle.IsEmpty())
		m_jobHandle.Complate();
	
	m_desc.framePerSegment = frame;

	m_currentFrameRange.store({ 0, 0 }, std::memory_order_release);
	m_segmentParts.clear();

	MoveSeek(m_seekPos, true); // 深度変更後に再読み込み
}

void VideoProcessingJob::SetKeepLoadSegmentCount(uint32_t segment)
{
	if(m_desc.loadSegmentCount == segment)
		return;

	//生成ジョブが完了していない場合は完了を待つ
	if (!m_jobHandle.IsEmpty())
		m_jobHandle.Complate();

	m_desc.loadSegmentCount = segment;

	m_currentFrameRange.store({ 0, 0 }, std::memory_order_release);
	m_segmentParts.clear();

	MoveSeek(m_seekPos, true); // 深度変更後に再読み込み
}

void VideoProcessingJob::MoveSeek(uint32_t seek, bool syncIfLoad)
{
	auto& jobSystem = Utility::Singleton::Instance<Utility::JobSystem>();
	m_seekPos = seek;

	//非同期読み込みで、生成ジョブが完了していない場合は、何もしない
	if (!syncIfLoad && !m_jobHandle.IsEmpty() && !m_jobHandle.IsComplate())
		return;

	//シークを移動した際に、追加で読み込む必要があるかどうかを判断
	{
		auto [headSeg, tailSeg] = m_currentFrameRange.load(std::memory_order_acquire);
		headSeg	/= m_desc.framePerSegment; 
		tailSeg		/= m_desc.framePerSegment;
	
		uint32_t seekSeg	= seek / m_desc.framePerSegment;
		uint32_t maxSeg	= m_videoInfo.frameCount / m_desc.framePerSegment;
	
		//再生位置が現在の先頭セグメント以上、末尾セグメント未満なら不要
		//※N = m_desc.loadSegmentCount
		if( seekSeg >= headSeg && seekSeg < tailSeg)
			return;

		//現在位置が末尾セグメントと動画の最終セグメントの位置にいるなら必要なし　（現読み込みで最終位置にいるため追加読み込み不要）
		if (seekSeg == tailSeg && seekSeg == maxSeg)
			return;


		//ここまできたら、読み込みが必要
		uint32_t newHeadSeg		= seekSeg;
		uint32_t newTailSeg		= std::min(seekSeg + m_desc.loadSegmentCount - 1, maxSeg);
		m_newSegRange = std::make_pair(newHeadSeg, newTailSeg);

		//以前のジョブが完了していない場合は、完了を待つ
		m_jobHandle.Complate();
		m_jobHandle = jobSystem.Schedule(this);

		if (syncIfLoad)
			m_jobHandle.Complate();
	}
}

std::vector<ColoredNode> VideoProcessingJob::FetchColorMapByFrame(uint32_t frame)
{
	if (m_segmentParts.empty())
		return {};
	if(frame >= m_videoInfo.frameCount)
		throw std::out_of_range("Frame index out of range: " + std::to_string(frame));

	//読み込み済みのフレーム外が要求された場合は、そこまで読み込み
	{
		auto [headFrame, tailFrame] = m_currentFrameRange.load(std::memory_order_acquire);

		if(frame > tailFrame || frame < headFrame)
		{
			MoveSeek(frame, true); // フレーム位置を更新、読み込みが必要な場合は同期的に行う
			headFrame = m_currentFrameRange.load(std::memory_order_acquire).first;
			uint32_t index = frame - headFrame;

			auto& segment = m_segmentParts.at(index);
			if(segment.frameIndex != frame)
				throw std::runtime_error("Frame index mismatch: expected " + std::to_string(frame) + ", got " + std::to_string(segment.frameIndex));

			return segment.frame;
		}
	}
	
	//読み込み済みのフレームが要求された場合は、そのフレームを返す
	{
		std::unique_lock lock(m_mutex);
		auto [headFrame, tailFrame] = m_currentFrameRange.load(std::memory_order_acquire); //処理スレッドが先にロックを取得した際に、区間が変わるため、再度取得

		uint32_t index = frame - headFrame;
		auto& segment = m_segmentParts.at(index);
		
		if (segment.frameIndex != frame)
			throw std::runtime_error("Frame index mismatch: expected " + std::to_string(frame) + ", got " + std::to_string(segment.frameIndex));

		MoveSeek(frame, false); // フレーム位置を更新、読み込みが必要な場合は非同期で行う
		return segment.frame;
	}
}

//非同期で自動で足りない分をキューにフレームを追加する
void VideoProcessingJob::Execute()
{
	//フレームの生成
	auto [head, tail] = m_currentFrameRange.load(std::memory_order_relaxed);
	uint32_t newSegHead		= m_newSegRange.first * m_desc.framePerSegment;
	uint32_t newSegTail		= (m_newSegRange.second + 1) * m_desc.framePerSegment;

	//範囲外のフレームを生成する
	{
		U32Pair loadSegRange = m_newSegRange;
		if (!m_segmentParts.empty())
		{
			auto range = RangeDiff(loadSegRange.first, loadSegRange.second, head / m_desc.framePerSegment, tail / m_desc.framePerSegment);
			if (!range) return; //読み込み範囲がない場合は終了

			loadSegRange = *range;
		}

		uint32_t loadBeginFrame	= loadSegRange.first * m_desc.framePerSegment;
		uint32_t loadEndFrame		= std::min((loadSegRange.second + 1) * m_desc.framePerSegment, m_videoInfo.frameCount);
		DecodeFrames(loadBeginFrame, loadEndFrame, m_segBuffer);
	}

	if(!m_segBuffer.empty())
	{
		std::unique_lock lock(m_mutex);

		//前方に生成する場合
		if (!m_segmentParts.empty() && m_segBuffer.back().frameIndex < head)
		{
			//範囲外の区間を削除
			auto range = RangeDiff(head, tail, newSegHead, newSegTail);
			for (uint32_t i = 0; range && i < range->second - range->first + 1; i++)
				m_segmentParts.pop_back();
			
			//新しいフレームを先頭に追加
			m_segmentParts.assign_range(std::move(m_segBuffer));
		}
		//後方に生成する場合 or 生成済みが空の場合
		else
		{
			//範囲外の区間を削除
			if (!m_segmentParts.empty())
			{
				auto range = RangeDiff(head, tail, newSegHead, newSegTail);
				for (uint32_t i = 0; range && i < range->second - range->first + 1; i++)
					m_segmentParts.pop_front();
			}

			//新しいフレームを後方に追加
			m_segmentParts.append_range(std::move(m_segBuffer));
		}

		uint32_t newBeginFrame	= m_segmentParts.front().frameIndex;
		uint32_t newEndFrame		= m_segmentParts.back().frameIndex;
		m_currentFrameRange.store(std::make_pair(newBeginFrame, newEndFrame), std::memory_order_release);

	}

	m_segBuffer.clear(); // バッファをクリアして次のフレーム生成に備える
}


/*!
@brief 
[Unsafe]
フレームを指定の範囲で生成する。生成時、動画の再生位置を移動させる。
再生位置は、終了フレーム - 1で指定される。

@param beginFrame 開始フレーム
@param endFrame 終了フレーム (このフレームは含まれない)
@param queueBuffer 生成されたフレームデータを格納するバッファ
@return true: 継続可能, false: 読み込み終了/失敗
*/
bool VideoProcessingJob::DecodeFrames(uint32_t beginFrame, uint32_t endFrame, std::vector<SegmentPart>& buffer)
{
	if(beginFrame >= m_videoInfo.frameCount || endFrame > m_videoInfo.frameCount || beginFrame >= endFrame)
		throw std::runtime_error("Invalid frame range: [" + std::to_string(beginFrame) + ", " + std::to_string(endFrame) + ")");
	
	m_capture->set(cv::CAP_PROP_POS_FRAMES, static_cast<double>(beginFrame));

	cv::Mat mat;
	for (uint32_t i = 0; i < endFrame - beginFrame; i++)
	{
		if (!m_capture->read(mat))
			return false; //読み込み終了/失敗

		auto generatedColorMap = BuildColorMaps(mat);

		buffer.emplace_back(SegmentPart{
			.frame			= std::move(generatedColorMap),
			.frameIndex	= beginFrame + i,
			});
	}

	return true; //読み込みの継続
}

/*!
@brief [Unsafe] フレームデータからカラーマップを生成する
*/
std::vector<ColoredNode> VideoProcessingJob::BuildColorMaps(cv::Mat const& mat)
{
	uint32_t frameSize = static_cast<uint32_t>(m_videoInfo.frameSize.x * m_videoInfo.frameSize.y);
	uint8_t current_byte = 0;
	int bit_index = 0;

	for (uint32_t i = 0; i < frameSize; i++)
	{
		uint8_t r = mat.data[i * mat.channels() + 0];
		bool isWhite = (r >= 127);

		current_byte |= (isWhite << (7 - bit_index));
		++bit_index;

		if (bit_index == 8)
		{
			m_tempColorBuffer[i / 8] = current_byte;
			current_byte = 0;
			bit_index = 0;
		}
	}

	if (bit_index > 0)
		m_tempColorBuffer[frameSize / 8] = current_byte;

	return m_tree.GenerateWhiteNodes(m_tempColorBuffer.get());
}
