#pragma once
#include <cstdint>
#include <cstddef>
#include <utility>

namespace Utility
{


	/*!
	@brief シンプルな単方向リスト＆固定長メモリバッファ
	
	@details
	要素は単方向リストのノードで構成されています。
		|---DataSize(8byte)---|------Data(any byte)------||---DataSize(8byte)---|------Data(any byte)------||---DataSize(8byte)---|------Data(any byte)------|…
	*/
	class MemoryBuffer final
	{
	public:
		/*!
		@brief 
		コンストラクタ
		指定されたサイズのバッファを確保します。
	
		@brief bufferSize バッファサイズ
		*/
		MemoryBuffer(size_t bufferSize) : m_bufferSize(bufferSize)
		{ m_buffer = new std::byte[bufferSize]; }
	
		/*!
		@brief 
		デストラクタ。
		全てのバッファを解放します。
	
		@note 
		この関数はバッファ自体は解放しますが、
		確保したメモリの中で構築されたインスタンスのデストラクタは呼びません。
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
		@brief 確保済みのサイズを取得します。
		*/
		size_t GetAllocatedSize() const { return m_allocatedSize; }
	
		/*!
		@brief 確保済みのノード数を取得します。
		*/
		size_t GetAllocatedNodeCount() const { return m_allocatedNodeCount; }
	
		/*!
		@brief メモリを確保します。
		@param size 確保するサイズ
		@return 確保したメモリのポインタ。確保に失敗した場合はnullptrを返します。
	
		@details
		確保したメモリは、確保したサイズの前にsizeof(size_t)バイトのサイズ情報が書き込まれます。
		先頭から順番に確保されます。
		戻り値は、サイズ情報を除いたデータの先頭ポインタです。
		*/
		void* Allocate(size_t size)
		{
			//バッファが足りない場合はnullptrを返す
			if (m_allocatedSize + size + sizeof(size_t) > m_bufferSize)
				return nullptr; 
			m_allocatedSize += size + sizeof(size_t); //確保済みサイズを更新
	
			std::byte* nextHead;
	
			//先頭ノードがnullptrの場合はバッファの先頭から確保
			if (m_head == nullptr)
			{
				nextHead = m_buffer;
			}
			//それ以外の場合は、現在の先頭ノードから確保
			else
			{
				size_t headBufferSize = *reinterpret_cast<size_t*>(m_head); //今の先頭ノードからデータサイズを取得
				nextHead = m_head + sizeof(size_t) + headBufferSize; //次の先頭ノードのポインタを取得
			}
	
			//先頭ノードの先頭sizeof(size_t)バイト分に、確保するデータのサイズを書き込む
			size_t* nextHeadBufferSize = reinterpret_cast<size_t*>(nextHead);
			*nextHeadBufferSize = size;
			
			m_head = nextHead;						//先頭ノードを更新
			m_allocatedNodeCount++;			//確保済みノード数を更新
			return nextHead + sizeof(size_t);	//データの先頭ポインタを返す
		}
	
		/*!
		@brief 確保したメモリから指定したインデックスのデータを取得します。
		@param index インデックス
		@return データのポインタ。要素範囲外の場合はnullptrを返します。
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
		std::byte*		m_buffer;										//バッファ
		std::byte*		m_head							= nullptr;	//確保済みの先頭ポインタ
		size_t			m_bufferSize					= 0;			//バッファサイズ
		size_t			m_allocatedSize				= 0;			//確保済みサイズ
		size_t			m_allocatedNodeCount	= 0;			//確保済みノード数
	};

}