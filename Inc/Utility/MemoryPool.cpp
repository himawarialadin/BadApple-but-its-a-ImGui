#include "MemoryPool.h"

namespace Utility
{
	MemoryPool& MemoryPool::operator=(MemoryPool&& other) noexcept
	{
		m_allocator	= std::move(other.m_allocator);
		m_buffer				= other.m_buffer;
		m_blockSize			= other.m_blockSize;
		m_capacity			= other.m_capacity;
		m_count				= other.m_count;

		other.m_allocator	= {};
		other.m_buffer				= nullptr;
		other.m_blockSize			= 0;
		other.m_capacity			= 0;
		other.m_count				= 0;
		return *this;
	}

	MemoryPool MemoryPool::Create(TypeErasedAllocator const& invoker, size_t reserveBlockCount)
	{
		MemoryPool pool;
		pool.m_blockSize			= invoker.typeInfo.Size();
		pool.m_allocator	= invoker;
		pool.Reallocate(reserveBlockCount);

		return pool;
	}

	void MemoryPool::Swap(size_t index1, size_t index2)
	{
		if (index1 == index2)
			return;

		assert(index1 < m_count && index2 < m_count);

		void* buffer1 = At_Unchkecd(index1);
		void* buffer2 = At_Unchkecd(index2);
		m_allocator.swap(buffer1, buffer2);
	}

	void MemoryPool::Reallocate(size_t capacity)
	{
		if (capacity == 0)
		{
			Clear();
			delete[] m_buffer;
			m_buffer = nullptr;
			m_capacity = 0;
			return;
		}

		std::byte* newBuffer = new std::byte[m_blockSize * capacity];

		for (size_t i = 0; i < m_count; i++)
		{
			//再確保する場合は、新しいバッファを確保し、元のサイズ分までは初期化のためにコンストラクタを呼び出す。(std::stringなどがムーブで元の変数を逆参照するため)
			m_allocator.construct(newBuffer + m_blockSize * i);
			
			//移動前の要素を新しいバッファの場所に移動
			m_allocator.move(At_Raw(i), newBuffer + m_blockSize * i);

			//移動後の要素のデストラクタを呼び出す。
			m_allocator.destruct(At_Raw(i));
		}
		delete[] m_buffer; //元の要素が全て削除されたので、元のバッファも解放

		m_buffer		= newBuffer;
		m_capacity	= capacity;
	}

}