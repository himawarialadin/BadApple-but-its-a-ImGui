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
+����̏������@+
�S���ǂݍ��ނƃ��������������A�ǂݍ��݂ɂ����Ԃ������邽�߁A�Z�O�����g�P�ʂœ������������B
������youtube�̂悤��MSE�������s���A�Z�O�����g�P�ʂœ�����������A�L���[�ɒǉ�����B

���݃t���[�����Ō�̃Z�O�����g�͈̔͂ł������ꍇ�A�ǉ��ǂݍ��݂��s���B


��F�i1�Z�O�����g������120�t���[���̏ꍇ�j
					|----------------------||----------------------||----------------------||----------------------||----------------------|
�t���[���͈́F		0 - 120					121 - 240					241 - 360					361 - 480				481 - 600 

*/

using U32Pair = std::pair<uint32_t, uint32_t>;

struct VideoInfo
{
	Math::Vector2	frameSize;
	uint32_t			frameCount;
	float					fps;				//�t���[�����[�g(FPS)
	float					durationSec;	//�Đ�����(s)
};

class VideoProcessingJob : public Utility::IJob
{
	friend class PlayerWindow;
public:

	struct Desc
	{
		std::string		videoFilename;
		uint32_t		quadTreeDepth			= 8;		//�N�A�b�h�c���[�̕����[�x
		uint32_t		framePerSegment		= 120;	//1�Z�O�����g������̃t���[����
		uint32_t		loadSegmentCount		= 3;		//��ɓǂݍ��ރZ�O�����g��
	};

	struct SegmentPart
	{
		std::vector<ColoredNode>	frame;
		uint32_t							frameIndex;	//�t���[���C���f�b�N�X
	};


public:
	VideoProcessingJob();
	~VideoProcessingJob();

public:
	/*!
	@brief �r�f�I�����W���u�̏�����
	@param desc �r�f�I�����W���u�̐ݒ�
	*/
	bool Initialize(Desc const& desc);

	/*!
	@brief �l���؂̐[�x��ݒ�
	@warning �p�ɂɕύX����ƃp�t�H�[�}���X���ቺ���邽�ߗv����
	*/
	void SetQuadTreeDepth(uint32_t depth);

	void SetFramePerSegment(uint32_t frame);
	void SetKeepLoadSegmentCount(uint32_t segment);

	/*!
	@brief �r�f�I�����W���u�̐ݒ���擾
	*/
	Desc const& GetDesc() const { return m_desc; }

	/*!
	@brief �ǂݍ��񂾃r�f�I�̏����擾
	*/
	VideoInfo const& GetVideoInfo()	const { return m_videoInfo; }
	
	/*!
	@brief ���݂̓ǂݍ��񂾐擪�̃t���[���C���f�b�N�X���擾
	*/
	uint32_t GetSeekPosition()	const { return m_seekPos;}

	/*!
	@brief ���݂̓ǂݍ��܂�Ă���t���[���͈͂��擾
	@return ���݂̃t���[���͈� (�J�n�t���[��, �I���t���[��)
	*/
	U32Pair GetCurrentFrameRange() const { return m_currentFrameRange.load(std::memory_order_relaxed); }

	/*!
	@brief �V�[�N�ʒu���w��̃t���[���Ɉړ����A�K�v�ȃt���[����ǂݍ��ށB
	@param seek �V�[�N�ʒu�̃t���[���C���f�b�N�X
	@param syncIfLoad �ǂݍ��݂��K�v�ȏꍇ�͓����I�ɓǂݍ��ނ��ǂ���
	*/
	void MoveSeek(uint32_t seek, bool syncIfLoad = true);

	/*!
	@brief 
	�w�肵���t���[���̃J���[�����擾�B
	�t���[�����ǂݍ��ݔ͈͊O�Ȃ瓯���I�ɓǂݍ��݁A�ǂݍ��ݍς݂Ȃ�L���[���瑦���ɕԂ��B
	�L���[���̃t���[���͎擾���ɃR�s�[�������̂�ԋp����B
	@param frame �t���[���C���f�b�N�X
	*/
	[[nodiscard]] std::vector<ColoredNode> FetchColorMapByFrame(uint32_t frame);

private:
	void Execute() override;
	bool DecodeFrames(uint32_t beginFrame, uint32_t endFrame, std::vector<SegmentPart>& buffer);
	std::vector<ColoredNode> BuildColorMaps(cv::Mat const& mat);

private://Settings
	Utility::JobHandle		m_jobHandle;				//�W���u�n���h��
	std::mutex					m_mutex;
	Desc							m_desc;

private://Initialization
	std::unique_ptr<cv::VideoCapture>	m_capture = nullptr;	//�r�f�I�L���v�`��
	VideoInfo											m_videoInfo = {};
	QuadTree											m_tree;			//�N�A�b�h�c���[

private://Progress
	std::unique_ptr<unsigned char[]>	m_tempColorBuffer;		//�t���[���f�[�^�̃o�b�t�@
	std::vector<SegmentPart>				m_segBuffer;

	U32Pair											m_newSegRange;			//�ǂݍ��݃Z�O�����g�͈� (�J�n�Z�O�����g, �I���Z�O�����g)
	std::atomic<U32Pair>					m_currentFrameRange;	//�L���[���̃t���[���͈�
	std::deque<SegmentPart>				m_segmentParts;			//�����ς݃Z�O�����g�̃L���[
	uint32_t										m_seekPos		= 0;		//���݂̃t���[���C���f�b�N�X

};

