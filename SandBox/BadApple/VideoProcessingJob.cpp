#include "VideoProcessingJob.h"
#include <opencv2/opencv.hpp>
#include <Utility/Singleton.h>
#include <Utility/JobSystem/JobSystem.h>

/*!
@brief ��Ԃ̍��W���i���[A, B] - ���[C, D]�j�����߂�

@return 
���W���̌��� [first, secound] �̌`���ŕԂ� 
�E��Ԃ��d�Ȃ�Ȃ��ꍇ�� [A, B] ��Ԃ�
�E��Ԃ����S�ɕ�܂����ꍇ�ƕ����I�ɏd�Ȃ�ꍇ�� std::nullopt ��Ԃ�

@exception std::runtime_error �����̒l���s���ȏꍇ(�Ⴆ�� A > B, C > D �Ȃ�)
*/
static constexpr std::optional<std::pair<uint32_t, uint32_t>> RangeDiff(uint32_t A, uint32_t B, uint32_t C, uint32_t D)
{

	if(A > B || C > D)
		throw std::runtime_error("Invalid range: A > B or C > D");

	// ���[C, D] �� [A, B] �̊��S�O�ɂ���ꍇ�i�d�Ȃ�Ȃ��j
	if (D < A || B < C)
		return std::make_pair( A, B );

	// ���[C, D] �� [A, B] �����S�Ɋ܂ށA�܂��͋�ԑS�̂ɏd�Ȃ�
	if (C <= A && B <= D)
		return std::nullopt;

	// �����I�ɍ������d�Ȃ�: [C, D] �� [A, B] �̍������悤�ȏꍇ
	if (C <= A && D < B)
		return std::make_pair( D + 1, B );

	// �����I�ɉE�����d�Ȃ�: [C, D] �� [A, B] �̉E�����悤�ȏꍇ
	if (A < C && B <= D)
		return std::make_pair( A, C - 1 );

	// �����������悤�ȕ����I�d�Ȃ�i[A, C-1], [D+1, B] �̂悤�Ȍ`�j�͑ΏۊO
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
		return false; // �r�f�I�t�@�C�����J���Ȃ��ꍇ�͎��s
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

	MoveSeek(0, false); // ���������ɐ擪�t���[����ǂݍ���
	return true;
}

void VideoProcessingJob::SetQuadTreeDepth(uint32_t depth)
{
	if (depth == 0)
		throw std::runtime_error("QuadTree depth must be greater than 0");

	if (m_desc.quadTreeDepth == depth)
		return;

	//�����W���u���������Ă��Ȃ��ꍇ�͊�����҂�
	if (!m_jobHandle.IsEmpty())
		m_jobHandle.Complate();

	m_desc.quadTreeDepth = depth;

	// �N�A�b�h�c���[�̍Đ����ƃt���[���͈͂̏�����
	m_tree.Create({ m_videoInfo.frameSize, m_desc.quadTreeDepth });
	m_currentFrameRange.store({ 0, 0 }, std::memory_order_release);
	m_segmentParts.clear();

	MoveSeek(m_seekPos, true); // �[�x�ύX��ɍēǂݍ���
}

void VideoProcessingJob::SetFramePerSegment(uint32_t frame)
{
	if(m_desc.framePerSegment == frame)
		return;

	//�����W���u���������Ă��Ȃ��ꍇ�͊�����҂�
	if (!m_jobHandle.IsEmpty())
		m_jobHandle.Complate();
	
	m_desc.framePerSegment = frame;

	m_currentFrameRange.store({ 0, 0 }, std::memory_order_release);
	m_segmentParts.clear();

	MoveSeek(m_seekPos, true); // �[�x�ύX��ɍēǂݍ���
}

void VideoProcessingJob::SetKeepLoadSegmentCount(uint32_t segment)
{
	if(m_desc.loadSegmentCount == segment)
		return;

	//�����W���u���������Ă��Ȃ��ꍇ�͊�����҂�
	if (!m_jobHandle.IsEmpty())
		m_jobHandle.Complate();

	m_desc.loadSegmentCount = segment;

	m_currentFrameRange.store({ 0, 0 }, std::memory_order_release);
	m_segmentParts.clear();

	MoveSeek(m_seekPos, true); // �[�x�ύX��ɍēǂݍ���
}

void VideoProcessingJob::MoveSeek(uint32_t seek, bool syncIfLoad)
{
	auto& jobSystem = Utility::Singleton::Instance<Utility::JobSystem>();
	m_seekPos = seek;

	//�񓯊��ǂݍ��݂ŁA�����W���u���������Ă��Ȃ��ꍇ�́A�������Ȃ�
	if (!syncIfLoad && !m_jobHandle.IsEmpty() && !m_jobHandle.IsComplate())
		return;

	//�V�[�N���ړ������ۂɁA�ǉ��œǂݍ��ޕK�v�����邩�ǂ����𔻒f
	{
		auto [headSeg, tailSeg] = m_currentFrameRange.load(std::memory_order_acquire);
		headSeg	/= m_desc.framePerSegment; 
		tailSeg		/= m_desc.framePerSegment;
	
		uint32_t seekSeg	= seek / m_desc.framePerSegment;
		uint32_t maxSeg	= m_videoInfo.frameCount / m_desc.framePerSegment;
	
		//�Đ��ʒu�����݂̐擪�Z�O�����g�ȏ�A�����Z�O�����g�����Ȃ�s�v
		//��N = m_desc.loadSegmentCount
		if( seekSeg >= headSeg && seekSeg < tailSeg)
			return;

		//���݈ʒu�������Z�O�����g�Ɠ���̍ŏI�Z�O�����g�̈ʒu�ɂ���Ȃ�K�v�Ȃ��@�i���ǂݍ��݂ōŏI�ʒu�ɂ��邽�ߒǉ��ǂݍ��ݕs�v�j
		if (seekSeg == tailSeg && seekSeg == maxSeg)
			return;


		//�����܂ł�����A�ǂݍ��݂��K�v
		uint32_t newHeadSeg		= seekSeg;
		uint32_t newTailSeg		= std::min(seekSeg + m_desc.loadSegmentCount - 1, maxSeg);
		m_newSegRange = std::make_pair(newHeadSeg, newTailSeg);

		//�ȑO�̃W���u���������Ă��Ȃ��ꍇ�́A������҂�
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

	//�ǂݍ��ݍς݂̃t���[���O���v�����ꂽ�ꍇ�́A�����܂œǂݍ���
	{
		auto [headFrame, tailFrame] = m_currentFrameRange.load(std::memory_order_acquire);

		if(frame > tailFrame || frame < headFrame)
		{
			MoveSeek(frame, true); // �t���[���ʒu���X�V�A�ǂݍ��݂��K�v�ȏꍇ�͓����I�ɍs��
			headFrame = m_currentFrameRange.load(std::memory_order_acquire).first;
			uint32_t index = frame - headFrame;

			auto& segment = m_segmentParts.at(index);
			if(segment.frameIndex != frame)
				throw std::runtime_error("Frame index mismatch: expected " + std::to_string(frame) + ", got " + std::to_string(segment.frameIndex));

			return segment.frame;
		}
	}
	
	//�ǂݍ��ݍς݂̃t���[�����v�����ꂽ�ꍇ�́A���̃t���[����Ԃ�
	{
		std::unique_lock lock(m_mutex);
		auto [headFrame, tailFrame] = m_currentFrameRange.load(std::memory_order_acquire); //�����X���b�h����Ƀ��b�N���擾�����ۂɁA��Ԃ��ς�邽�߁A�ēx�擾

		uint32_t index = frame - headFrame;
		auto& segment = m_segmentParts.at(index);
		
		if (segment.frameIndex != frame)
			throw std::runtime_error("Frame index mismatch: expected " + std::to_string(frame) + ", got " + std::to_string(segment.frameIndex));

		MoveSeek(frame, false); // �t���[���ʒu���X�V�A�ǂݍ��݂��K�v�ȏꍇ�͔񓯊��ōs��
		return segment.frame;
	}
}

//�񓯊��Ŏ����ő���Ȃ������L���[�Ƀt���[����ǉ�����
void VideoProcessingJob::Execute()
{
	//�t���[���̐���
	auto [head, tail] = m_currentFrameRange.load(std::memory_order_relaxed);
	uint32_t newSegHead		= m_newSegRange.first * m_desc.framePerSegment;
	uint32_t newSegTail		= (m_newSegRange.second + 1) * m_desc.framePerSegment;

	//�͈͊O�̃t���[���𐶐�����
	{
		U32Pair loadSegRange = m_newSegRange;
		if (!m_segmentParts.empty())
		{
			auto range = RangeDiff(loadSegRange.first, loadSegRange.second, head / m_desc.framePerSegment, tail / m_desc.framePerSegment);
			if (!range) return; //�ǂݍ��ݔ͈͂��Ȃ��ꍇ�͏I��

			loadSegRange = *range;
		}

		uint32_t loadBeginFrame	= loadSegRange.first * m_desc.framePerSegment;
		uint32_t loadEndFrame		= std::min((loadSegRange.second + 1) * m_desc.framePerSegment, m_videoInfo.frameCount);
		DecodeFrames(loadBeginFrame, loadEndFrame, m_segBuffer);
	}

	if(!m_segBuffer.empty())
	{
		std::unique_lock lock(m_mutex);

		//�O���ɐ�������ꍇ
		if (!m_segmentParts.empty() && m_segBuffer.back().frameIndex < head)
		{
			//�͈͊O�̋�Ԃ��폜
			auto range = RangeDiff(head, tail, newSegHead, newSegTail);
			for (uint32_t i = 0; range && i < range->second - range->first + 1; i++)
				m_segmentParts.pop_back();
			
			//�V�����t���[����擪�ɒǉ�
			m_segmentParts.assign_range(std::move(m_segBuffer));
		}
		//����ɐ�������ꍇ or �����ς݂���̏ꍇ
		else
		{
			//�͈͊O�̋�Ԃ��폜
			if (!m_segmentParts.empty())
			{
				auto range = RangeDiff(head, tail, newSegHead, newSegTail);
				for (uint32_t i = 0; range && i < range->second - range->first + 1; i++)
					m_segmentParts.pop_front();
			}

			//�V�����t���[��������ɒǉ�
			m_segmentParts.append_range(std::move(m_segBuffer));
		}

		uint32_t newBeginFrame	= m_segmentParts.front().frameIndex;
		uint32_t newEndFrame		= m_segmentParts.back().frameIndex;
		m_currentFrameRange.store(std::make_pair(newBeginFrame, newEndFrame), std::memory_order_release);

	}

	m_segBuffer.clear(); // �o�b�t�@���N���A���Ď��̃t���[�������ɔ�����
}


/*!
@brief 
[Unsafe]
�t���[�����w��͈̔͂Ő�������B�������A����̍Đ��ʒu���ړ�������B
�Đ��ʒu�́A�I���t���[�� - 1�Ŏw�肳���B

@param beginFrame �J�n�t���[��
@param endFrame �I���t���[�� (���̃t���[���͊܂܂�Ȃ�)
@param queueBuffer �������ꂽ�t���[���f�[�^���i�[����o�b�t�@
@return true: �p���\, false: �ǂݍ��ݏI��/���s
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
			return false; //�ǂݍ��ݏI��/���s

		auto generatedColorMap = BuildColorMaps(mat);

		buffer.emplace_back(SegmentPart{
			.frame			= std::move(generatedColorMap),
			.frameIndex	= beginFrame + i,
			});
	}

	return true; //�ǂݍ��݂̌p��
}

/*!
@brief [Unsafe] �t���[���f�[�^����J���[�}�b�v�𐶐�����
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
