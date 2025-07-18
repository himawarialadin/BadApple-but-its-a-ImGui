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
		ElementRef(ElementRef const&)		= delete; //MemoryPool�����ڍ\�z���ĕԂ������������Ȃ��̂ŃR���X�g���N�^�͎g��Ȃ�
		ElementRef(ElementRef&&)			= delete; //MemoryPool�����ڍ\�z���ĕԂ������������Ȃ��̂ŃR���X�g���N�^�͎g��Ȃ�
		~ElementRef()	= default;

		//ElementAny�𒼐ڑ���͌�쓮�̌����ɂȂ邽�ߋ֎~
		ElementRef& operator=(ElementRef const&)			= delete; 
		ElementRef& operator=(ElementRef&&) noexcept	= delete;

		/**
		* @brief ElementRef�̗v�f���擾���܂�(ref)
		* @tparam T �擾����v�f�̌^�B�擾�����������v�[�����Ǘ�����^�Ɠ����ł���K�v������܂��B
		*/
		template<typename T>
		T& As()
		{
			assert(m_invoker.typeInfo == TypeInfo::Get<T>());
			return *reinterpret_cast<T*>(m_pointer);
		}

		/**
		* @brief ElementRef�̗v�f���擾���܂�(const ref)
		* @tparam T �擾����v�f�̌^�B�擾�����������v�[�����Ǘ�����^�Ɠ����ł���K�v������܂��B
		*/
		template<typename T>
		T const& As() const
		{
			assert(m_invoker.typeInfo == TypeInfo::Get<T>());
			return *reinterpret_cast<T*>(m_pointer);
		}

		/**
		* @brief �v�f����B
		* �擾�����������v�[�����Ǘ�����^�Ɠ����ł���K�v������܂��B
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
		* @brief �ʂ�ElementRef�̗v�f�����[�u������܂��B 
		* @param other �v�f�B
		* �ʂ̃������v�[���C���X�^���X����擾�������̂ł���肠��܂��񂪁A���̃C���X�^���X�̃������v�[�����Ǘ�����^�Ɠ����ł���K�v������܂��B
		* ���[�u��Aother�͖����ȏ�ԂɂȂ�܂��B
		*/
		ElementRef& MoveAssign(ElementRef&& other)
		{
			assert(m_invoker.typeInfo == other.m_invoker.typeInfo);
			m_invoker.move(other.m_pointer, m_pointer);
			other.m_pointer = nullptr;

			return *this;
		}

		/**
		* @brief �ʂ�ElementRef�̗v�f���R�s�[������܂��B
		* @param other �v�f�B
		* �ʂ̃������v�[���C���X�^���X����擾�������̂ł���肠��܂��񂪁A���̃C���X�^���X�̃������v�[�����Ǘ�����^�Ɠ����ł���K�v������܂��B
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

	//�^�������ꂽ�������v�[��
	class MemoryPool
	{
	public:
		static constexpr size_t DefaultReserveBlockCount = 10000; //�f�t�H���g�ŗe�ʕs���ɂȂ����ꍇ�ɒǉ��Ŋm�ۂ���u���b�N��

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
		* @brief �������v�[���𐶐����܂�
		 * @tparam T �������v�[�����Ǘ�����^
		 * @param reserveBlockCount �e�ʕs���ɂȂ����ꍇ�ɒǉ��Ŋm�ۂ���u���b�N��
		 * @return �������ꂽ�������v�[��
		*/
		template<typename T>
		[[nodiscard]] static MemoryPool Create(size_t reserveBlockCount = DefaultReserveBlockCount);

		[[nodiscard]] static MemoryPool Create(TypeErasedAllocator const& invoker, size_t reserveBlockCount = DefaultReserveBlockCount);

		/**@brief ���݂̗v�f�� */
		size_t Count() const { return m_count; }

		/**@brief ���݂̃������u���b�N�� */
		size_t Capacity() const { return m_capacity; }

		/**@brief �^���v�����郁�����u���b�N�� (sizeof(T))*/
		size_t BlockSize() const { return m_blockSize; }

		/** @brief ���̃������v�[�����Ǘ����Ă���^ */
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
		* @brief �������v�[���̗e�ʂ�ύX���܂�
		* @param capacity �V�����e�ʁB���݂̗e�ʂ�菬�����l���w�肷��ƁA�������v�[���̗e�ʂ͕ύX����܂���B
		*/
		void Reserve(size_t capacity)
		{
			if (m_capacity >= capacity)
				return;

			Reallocate(capacity);
		}

		/**
		* @brief �������v�[���ɗv�f�𖖔��ɒǉ����܂�
		* @return �������ꂽ�v�f�̃|�C���^(�^�����ς�)
		*/
		void* EmplaceBack()
		{
			if (m_count == m_capacity)
				Reserve(m_capacity * 2);

			void* buffer = At_Unchkecd(m_count++);
			return m_allocator.construct(buffer);
		}

		/**
		* @brief �������v�[���̎w�肵���ʒu�ɂ���v�f���擾���܂��B
		* @param index �擾����v�f�̃C���f�b�N�X
		* @return �v�f�̃|�C���^(�^�����ς�)
		*/
		template<typename Self>
		element_void_t<Self> At_Raw(this Self&& self, size_t index)
		{
			assert(index < self.m_count);
			return self.At_Unchkecd(index);
		}

		/**
		* @brief �������v�[���̎w�肵���ʒu�ɂ���v�f���擾���܂��B
		* @param index �擾����v�f�̃C���f�b�N�X
		* @return �v�f�f�[�^�B���S�Ɏ擾�Ƒ�����ł���悤��ElementRef�Ƀ��b�v����Ă��܂��B
		*/
		ElementRef At(size_t index)
		{
			assert(index < m_count);
			return ElementRef(At_Unchkecd(index), m_allocator);
		}

		/**
		* @brief �������v�[���̎w�肵���ʒu�ɂ���v�f���擾���܂��B
		* @tparam U �擾����v�f�̌^�B���̃������v�[�����Ǘ�����^�Ɠ����ł���K�v������܂��B
		* @param index �擾����v�f�̃C���f�b�N�X
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
		* @brief �������v�[���̖����̗v�f���폜���܂�
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
		* �������v�[�����̗v�f���N���A���܂��B
		* �������v�[�����̃������͉������܂���B
		*/
		void Clear()
		{
			for (size_t i = 0; i < m_count; ++i)
				m_allocator.destruct(At_Raw(i));

			m_count = 0;
		}

		/**
		* @brief 
		* �������v�[���̗e�ʂ��g�p�ʂɍ��킹�ďk�����܂��B
		* �e�ʂƎg�p�ʂ��������ꍇ�͉������܂���B
		*/
		void ShrinkToFit() 
		{
			if (m_capacity == m_count)
				return;

			Reallocate(m_count);
		}

		/**
		* @brief �������v�[���̗v�f�����ւ��܂��B
		*/
		void Swap(size_t index1, size_t index2);

	private:
		void Reallocate(size_t capacity);

		template<typename Self>
		element_void_t<Self> At_Unchkecd(this Self&& self, size_t index)
		{ return reinterpret_cast<void*>(self.m_buffer + self.m_blockSize * index); }

	private:
		TypeErasedAllocator	m_allocator;
		std::byte*								m_buffer				= nullptr;			//�������o�b�t�@
		size_t									m_blockSize			= 0;					//�u���b�N�T�C�Y
		size_t 									m_capacity			= 0;					//�ő�u���b�N��
		size_t									m_count				= 0;					//�g�p�u���b�N��
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