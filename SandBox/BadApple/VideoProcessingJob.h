#pragma once
#include <Utility/JobSystem/JobInterface.h>
#include <Utility/JobSystem/JobHandle.h>
#include <Math/Vector2.h>
#include "QuadTree.h"
#include "Common.h"

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <deque>
#include <optional>

namespace cv
{
	class VideoCapture;
	class Mat;
}

/*
+動画の処理方法+
全部読み込むとメモリを圧迫し、読み込みにも時間がかかるため、セグメント単位で動画を処理する。
実装はyoutubeのようなMSE処理を行い、セグメント単位で動画を処理し、キューに追加する。

現在フレームが最後のセグメントの範囲であった場合、追加読み込みを行う。


例：（1セグメントあたり120フレームの場合）
					|----------------------||----------------------||----------------------||----------------------||----------------------|
フレーム範囲：		0 - 120					121 - 240					241 - 360					361 - 480				481 - 600 

*/

using U32Pair = std::pair<uint32_t, uint32_t>;

struct VideoInfo
{
	Math::Vector2	frameSize;
	uint32_t			frameCount;
	float					fps;				//フレームレート(FPS)
	float					durationSec;	//再生時間(s)
};

class VideoProcessingJob : public Utility::IJob
{
	friend class PlayerWindow;
public:

	struct Desc
	{
		std::string		videoFilename;
		uint32_t		quadTreeDepth			= 8;		//クアッドツリーの分割深度
		uint32_t		framePerSegment		= 120;	//1セグメントあたりのフレーム数
		uint32_t		loadSegmentCount		= 3;		//常に読み込むセグメント数
	};

	struct SegmentPart
	{
		std::vector<ColoredNode>	frame;
		uint32_t							frameIndex;	//フレームインデックス
	};


public:
	VideoProcessingJob();
	~VideoProcessingJob();

public:
	/*!
	@brief ビデオ処理ジョブの初期化
	@param desc ビデオ処理ジョブの設定
	*/
	bool Initialize(Desc const& desc);

	/*!
	@brief 四分木の深度を設定
	@warning 頻繁に変更するとパフォーマンスが低下するため要注意
	*/
	void SetQuadTreeDepth(uint32_t depth);

	void SetFramePerSegment(uint32_t frame);
	void SetKeepLoadSegmentCount(uint32_t segment);

	/*!
	@brief ビデオ処理ジョブの設定を取得
	*/
	Desc const& GetDesc() const { return m_desc; }

	/*!
	@brief 読み込んだビデオの情報を取得
	*/
	VideoInfo const& GetVideoInfo()	const { return m_videoInfo; }
	
	/*!
	@brief 現在の読み込んだ先頭のフレームインデックスを取得
	*/
	uint32_t GetSeekPosition()	const { return m_seekPos;}

	/*!
	@brief 現在の読み込まれているフレーム範囲を取得
	@return 現在のフレーム範囲 (開始フレーム, 終了フレーム)
	*/
	U32Pair GetCurrentFrameRange() const { return m_currentFrameRange.load(std::memory_order_relaxed); }

	/*!
	@brief シーク位置を指定のフレームに移動し、必要なフレームを読み込む。
	@param seek シーク位置のフレームインデックス
	@param syncIfLoad 読み込みが必要な場合は同期的に読み込むかどうか
	*/
	void MoveSeek(uint32_t seek, bool syncIfLoad = true);

	/*!
	@brief 
	指定したフレームのカラー情報を取得。
	フレームが読み込み範囲外なら同期的に読み込み、読み込み済みならキューから即座に返す。
	キュー内のフレームは取得時にコピーしたものを返却する。
	@param frame フレームインデックス
	*/
	[[nodiscard]] std::vector<ColoredNode> FetchColorMapByFrame(uint32_t frame);

private:
	void Execute() override;
	bool DecodeFrames(uint32_t beginFrame, uint32_t endFrame, std::vector<SegmentPart>& buffer);
	std::vector<ColoredNode> BuildColorMaps(cv::Mat const& mat);

private://Settings
	Utility::JobHandle		m_jobHandle;				//ジョブハンドル
	std::mutex					m_mutex;
	Desc							m_desc;

private://Initialization
	std::unique_ptr<cv::VideoCapture>	m_capture = nullptr;	//ビデオキャプチャ
	VideoInfo											m_videoInfo = {};
	QuadTree											m_tree;			//クアッドツリー

private://Progress
	std::unique_ptr<unsigned char[]>	m_tempColorBuffer;		//フレームデータのバッファ
	std::vector<SegmentPart>				m_segBuffer;

	U32Pair											m_newSegRange;			//読み込みセグメント範囲 (開始セグメント, 終了セグメント)
	std::atomic<U32Pair>					m_currentFrameRange;	//キュー内のフレーム範囲
	std::deque<SegmentPart>				m_segmentParts;			//処理済みセグメントのキュー
	uint32_t										m_seekPos		= 0;		//現在のフレームインデックス

};

