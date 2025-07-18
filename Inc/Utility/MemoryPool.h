#pragma once
#include <cassert>
#include <stdint.h>
#include <cstddef>
#include "TypeErasedAllocator.h"

namespace Utility
{
	class ElementRef
	{
	public:
		ElementRef() = delete;
		ElementRef(void* pointer, TypeErasedAllocator& invoker) : m_pointer(pointer), m_invoker(invoker) { }
		ElementRef(ElementRef const&)		= delete; //MemoryPoolが直接構築して返す処理しかしないのでコンストラクタは使わない
		ElementRef(ElementRef&&)			= delete; //MemoryPoolが直接構築して返す処理しかしないのでコンストラクタは使わない
		~ElementRef()	= default;

		//ElementAnyを直接代入は誤作動の原因になるため禁止
		ElementRef& operator=(ElementRef const&)			= delete; 
		ElementRef& operator=(ElementRef&&) noexcept	= delete;

		/**
		* @brief ElementRefの要素を取得します(ref)
		* @tparam T 取得する要素の型。取得したメモリプールが管理する型と同じである必要があります。
		*/
		template<typename T>
		T& As()
		{
			assert(m_invoker.typeInfo == TypeInfo::Get<T>());
			return *reinterpret_cast<T*>(m_pointer);
		}

		/**
		* @brief ElementRefの要素を取得します(const ref)
		* @tparam T 取得する要素の型。取得したメモリプールが管理する型と同じである必要があります。
		*/
		template<typename T>
		T const& As() const
		{
			assert(m_invoker.typeInfo == TypeInfo::Get<T>());
			return *reinterpret_cast<T*>(m_pointer);
		}

		/**
		* @brief 要素代入。
		* 取得したメモリプールが管理する型と同じである必要があります。
		*/
		template<typename T>
		ElementRef& operator=(T&& value)
		{
			using _Ty = std::remove_cvref_t<T>;
			assert(m_invoker.typeInfo == TypeInfo::Get<_Ty>());

			*reinterpret_cast<_Ty*>(m_pointer) = std::forward<T>(value);
			return *this;
		}

		/**
		* @brief 別のElementRefの要素をムーブ代入します。 
		* @param other 要素。
		* 別のメモリプールインスタンスから取得したものでも問題ありませんが、このインスタンスのメモリプールが管理する型と同じである必要があります。
		* ムーブ後、otherは無効な状態になります。
		*/
		ElementRef& MoveAssign(ElementRef&& other)
		{
			assert(m_invoker.typeInfo == other.m_invoker.typeInfo);
			m_invoker.move(other.m_pointer, m_pointer);
			other.m_pointer = nullptr;

			return *this;
		}

		/**
		* @brief 別のElementRefの要素をコピー代入します。
		* @param other 要素。
		* 別のメモリプールインスタンスから取得したものでも問題ありませんが、このインスタンスのメモリプールが管理する型と同じである必要があります。
		*/
		ElementRef& CopyAssign(ElementRef const& other)
		{
			assert(m_invoker.typeInfo == other.m_invoker.typeInfo);
			m_invoker.copy(other.m_pointer, m_pointer);
		
			return *this;
		}
		

	private:
		void*										m_pointer = nullptr;
		TypeErasedAllocator&	m_invoker;
	};

	//型消去されたメモリプール
	class MemoryPool
	{
	public:
		static constexpr size_t DefaultReserveBlockCount = 10000; //デフォルトで容量不足になった場合に追加で確保するブロック数

		template <typename T>
		using element_void_t = std::conditional_t<std::is_const_v<std::remove_reference_t<T>>, const void*, void*>;
		
		template <typename T, typename U>
		using element_reference_t = std::conditional_t<std::is_const_v<std::remove_reference_t<T>>, U const&, U&>;

	public:
		MemoryPool() = default;
		~MemoryPool() { Clear(); ShrinkToFit(); }
		MemoryPool(MemoryPool const&)						= delete;
		MemoryPool& operator=(MemoryPool const&)		= delete;

		MemoryPool(MemoryPool&& other) noexcept { this->operator=(std::move(other)); }
		MemoryPool& operator=(MemoryPool&& other) noexcept;

		/**
		* @brief メモリプールを生成します
		 * @tparam T メモリプールが管理する型
		 * @param reserveBlockCount 容量不足になった場合に追加で確保するブロック数
		 * @return 生成されたメモリプール
		*/
		template<typename T>
		[[nodiscard]] static MemoryPool Create(size_t reserveBlockCount = DefaultReserveBlockCount);

		[[nodiscard]] static MemoryPool Create(TypeErasedAllocator const& invoker, size_t reserveBlockCount = DefaultReserveBlockCount);

		/**@brief 現在の要素数 */
		size_t Count() const { return m_count; }

		/**@brief 現在のメモリブロック数 */
		size_t Capacity() const { return m_capacity; }

		/**@brief 型が要求するメモリブロック量 (sizeof(T))*/
		size_t BlockSize() const { return m_blockSize; }

		/** @brief このメモリプールが管理している型 */
		Utility::TypeInfo const& GetTypeInfo() const { return m_allocator.typeInfo; }

		template<typename T, typename Self>
		auto begin(this Self&& self) -> decltype(auto)
		{ 
			using _Ty = std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, const T*, T*>;

			assert(self.m_allocator.typeInfo == TypeInfo::Get<T>());
			return reinterpret_cast<_Ty>(self.m_buffer);
		}

		template<typename T, typename Self>
		auto end(this Self&& self) -> decltype(auto)
		{
			using _Ty = std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, const T*, T*>;

			assert(self.m_allocator.typeInfo == TypeInfo::Get<T>());
			return reinterpret_cast<_Ty>(self.m_buffer + self.m_blockSize * self.m_count);
		}

		/**
		* @brief メモリプールの容量を変更します
		* @param capacity 新しい容量。現在の容量より小さい値を指定すると、メモリプールの容量は変更されません。
		*/
		void Reserve(size_t capacity)
		{
			if (m_capacity >= capacity)
				return;

			Reallocate(capacity);
		}

		/**
		* @brief メモリプールに要素を末尾に追加します
		* @return 生成された要素のポインタ(型消去済み)
		*/
		void* EmplaceBack()
		{
			if (m_count == m_capacity)
				Reserve(m_capacity * 2);

			void* buffer = At_Unchkecd(m_count++);
			return m_allocator.construct(buffer);
		}

		/**
		* @brief メモリプールの指定した位置にある要素を取得します。
		* @param index 取得する要素のインデックス
		* @return 要素のポインタ(型消去済み)
		*/
		template<typename Self>
		element_void_t<Self> At_Raw(this Self&& self, size_t index)
		{
			assert(index < self.m_count);
			return self.At_Unchkecd(index);
		}

		/**
		* @brief メモリプールの指定した位置にある要素を取得します。
		* @param index 取得する要素のインデックス
		* @return 要素データ。安全に取得と代入ができるようにElementRefにラップされています。
		*/
		ElementRef At(size_t index)
		{
			assert(index < m_count);
			return ElementRef(At_Unchkecd(index), m_allocator);
		}

		/**
		* @brief メモリプールの指定した位置にある要素を取得します。
		* @tparam U 取得する要素の型。このメモリプールが管理する型と同じである必要があります。
		* @param index 取得する要素のインデックス
		*/
		template<typename U, typename Self>
		element_reference_t<Self, U> At(this Self&& self, size_t index)
		{
			using T = std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, const U*, U*>;

			assert(self.m_allocator.typeInfo == TypeInfo::Get<U>());
			assert(index < self.m_count);
			return *reinterpret_cast<T>(self.At_Unchkecd(index));
		}

		/**
		* @brief メモリプールの末尾の要素を削除します
		*/
		void PopBack()
		{
			if (m_count == 0)
				return;

			m_allocator.destruct(At_Raw(m_count - 1));
			m_count--;
		}

		/**
		* @brief
		* メモリプール内の要素をクリアします。
		* メモリプール内のメモリは解放されません。
		*/
		void Clear()
		{
			for (size_t i = 0; i < m_count; ++i)
				m_allocator.destruct(At_Raw(i));

			m_count = 0;
		}

		/**
		* @brief 
		* メモリプールの容量を使用量に合わせて縮小します。
		* 容量と使用量が等しい場合は何もしません。
		*/
		void ShrinkToFit() 
		{
			if (m_capacity == m_count)
				return;

			Reallocate(m_count);
		}

		/**
		* @brief メモリプールの要素を入れ替えます。
		*/
		void Swap(size_t index1, size_t index2);

	private:
		void Reallocate(size_t capacity);

		template<typename Self>
		element_void_t<Self> At_Unchkecd(this Self&& self, size_t index)
		{ return reinterpret_cast<void*>(self.m_buffer + self.m_blockSize * index); }

	private:
		TypeErasedAllocator	m_allocator;
		std::byte*								m_buffer				= nullptr;			//メモリバッファ
		size_t									m_blockSize			= 0;					//ブロックサイズ
		size_t 									m_capacity			= 0;					//最大ブロック数
		size_t									m_count				= 0;					//使用ブロック数
	};


	template<typename T>
	inline MemoryPool MemoryPool::Create(size_t reserveBlockCount /*= DefaultReserveBlockCount*/)
	{
		MemoryPool pool;
		pool.m_blockSize	= sizeof(T);
		pool.m_allocator	= TypeErasedAllocator::Create<T>();
		pool.Reallocate(reserveBlockCount);

		return pool;
	}


}