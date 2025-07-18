#pragma once
#include <cstdint>
#include <cstddef>
#include <utility>

namespace Utility
{


	/*!
	@brief �V���v���ȒP�������X�g���Œ蒷�������o�b�t�@
	
	@details
	�v�f�͒P�������X�g�̃m�[�h�ō\������Ă��܂��B
		|---DataSize(8byte)---|------Data(any byte)------||---DataSize(8byte)---|------Data(any byte)------||---DataSize(8byte)---|------Data(any byte)------|�c
	*/
	class MemoryBuffer final
	{
	public:
		/*!
		@brief 
		�R���X�g���N�^
		�w�肳�ꂽ�T�C�Y�̃o�b�t�@���m�ۂ��܂��B
	
		@brief bufferSize �o�b�t�@�T�C�Y
		*/
		MemoryBuffer(size_t bufferSize) : m_bufferSize(bufferSize)
		{ m_buffer = new std::byte[bufferSize]; }
	
		/*!
		@brief 
		�f�X�g���N�^�B
		�S�Ẵo�b�t�@��������܂��B
	
		@note 
		���̊֐��̓o�b�t�@���͉̂�����܂����A
		�m�ۂ����������̒��ō\�z���ꂽ�C���X�^���X�̃f�X�g���N�^�͌Ăт܂���B
		*/
		~MemoryBuffer()
		{ delete[] m_buffer;}
	
		MemoryBuffer(MemoryBuffer const&) = delete;
		MemoryBuffer(MemoryBuffer&& other) noexcept { *this = std::move(other); }
		MemoryBuffer& operator=(MemoryBuffer const&) = delete;
		MemoryBuffer& operator=(MemoryBuffer&& other) noexcept
		{
			if (this != &other)
			{
				std::exchange(m_buffer,						other.m_buffer);
				std::exchange(m_head,							other.m_head);
				std::exchange(m_bufferSize,					other.m_bufferSize);
				std::exchange(m_allocatedSize,				other.m_allocatedSize);
				std::exchange(m_allocatedNodeCount,	other.m_allocatedNodeCount);
			}
			return *this;
		}
	public:
		
		/*!
		@brief �m�ۍς݂̃T�C�Y���擾���܂��B
		*/
		size_t GetAllocatedSize() const { return m_allocatedSize; }
	
		/*!
		@brief �m�ۍς݂̃m�[�h�����擾���܂��B
		*/
		size_t GetAllocatedNodeCount() const { return m_allocatedNodeCount; }
	
		/*!
		@brief ���������m�ۂ��܂��B
		@param size �m�ۂ���T�C�Y
		@return �m�ۂ����������̃|�C���^�B�m�ۂɎ��s�����ꍇ��nullptr��Ԃ��܂��B
	
		@details
		�m�ۂ����������́A�m�ۂ����T�C�Y�̑O��sizeof(size_t)�o�C�g�̃T�C�Y��񂪏������܂�܂��B
		�擪���珇�ԂɊm�ۂ���܂��B
		�߂�l�́A�T�C�Y�����������f�[�^�̐擪�|�C���^�ł��B
		*/
		void* Allocate(size_t size)
		{
			//�o�b�t�@������Ȃ��ꍇ��nullptr��Ԃ�
			if (m_allocatedSize + size + sizeof(size_t) > m_bufferSize)
				return nullptr; 
			m_allocatedSize += size + sizeof(size_t); //�m�ۍς݃T�C�Y���X�V
	
			std::byte* nextHead;
	
			//�擪�m�[�h��nullptr�̏ꍇ�̓o�b�t�@�̐擪����m��
			if (m_head == nullptr)
			{
				nextHead = m_buffer;
			}
			//����ȊO�̏ꍇ�́A���݂̐擪�m�[�h����m��
			else
			{
				size_t headBufferSize = *reinterpret_cast<size_t*>(m_head); //���̐擪�m�[�h����f�[�^�T�C�Y���擾
				nextHead = m_head + sizeof(size_t) + headBufferSize; //���̐擪�m�[�h�̃|�C���^���擾
			}
	
			//�擪�m�[�h�̐擪sizeof(size_t)�o�C�g���ɁA�m�ۂ���f�[�^�̃T�C�Y����������
			size_t* nextHeadBufferSize = reinterpret_cast<size_t*>(nextHead);
			*nextHeadBufferSize = size;
			
			m_head = nextHead;						//�擪�m�[�h���X�V
			m_allocatedNodeCount++;			//�m�ۍς݃m�[�h�����X�V
			return nextHead + sizeof(size_t);	//�f�[�^�̐擪�|�C���^��Ԃ�
		}
	
		/*!
		@brief �m�ۂ�������������w�肵���C���f�b�N�X�̃f�[�^���擾���܂��B
		@param index �C���f�b�N�X
		@return �f�[�^�̃|�C���^�B�v�f�͈͊O�̏ꍇ��nullptr��Ԃ��܂��B
		*/
		void* At(size_t index) const
		{
			if (index >= m_allocatedNodeCount)
				return nullptr;
	
			std::byte* head = m_buffer;
			while (index > 0)
			{
				size_t headBufferSize = *reinterpret_cast<size_t*>(head);
				head += sizeof(size_t) + headBufferSize;
				index--;
			}
	
			return head + sizeof(size_t);
		}
	
	private:
		std::byte*		m_buffer;										//�o�b�t�@
		std::byte*		m_head							= nullptr;	//�m�ۍς݂̐擪�|�C���^
		size_t			m_bufferSize					= 0;			//�o�b�t�@�T�C�Y
		size_t			m_allocatedSize				= 0;			//�m�ۍς݃T�C�Y
		size_t			m_allocatedNodeCount	= 0;			//�m�ۍς݃m�[�h��
	};

}