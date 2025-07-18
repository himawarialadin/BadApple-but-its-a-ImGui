#pragma once
#include "MathFwd.h"
#include <concepts>

namespace Math
{
	template<typename T>
	concept is_integer_vector = std::same_as<T, Vector2Int>;
}

namespace Math::Internal
{
	template<is_integer_vector T, typename Type, int _Element>
	class VectorIntBase
	{
	public:
		using value_type	= Type;
		using vector_type	= __m128i;

	public:
		static constexpr int Element			= _Element;
		static constexpr int Size					= _Element * sizeof(value_type);
		static constexpr int MaxElement	= 4;

		static_assert(Element < MaxElement + 1 && Element > 0, "Illegal Element!");
		static_assert(std::same_as<value_type, int32_t>/* || std::same_as<value_type, unsigned int>*/, "Illegal Type! Only int or unsigned int is allowed.");

	public:
		void* operator new	(size_t size);
		void* operator new	(size_t size, void* buf);
		void* operator new[](size_t size);

	public:
		static T			__vectorcall	Scale(T const& a, T const& b);
		static T			__vectorcall	Min(T const& value1, T const& value2);
		static T			__vectorcall	Max(T const& value1, T const& value2);
		static T			__vectorcall	Abs(T const& a);
	};

	template<is_integer_vector T, typename Type, int _Element>
	inline void* VectorIntBase<T, Type, _Element>::operator new(size_t size)
	{
		return ::operator new(size, static_cast<std::align_val_t>(sizeof(vector_type)));
	}

	template<is_integer_vector T, typename Type, int _Element>
	inline void* VectorIntBase<T, Type, _Element>::operator new(size_t size, void* buf)
	{
		return buf;
	}

	template<is_integer_vector T, typename Type, int _Element>
	inline void* VectorIntBase<T, Type, _Element>::operator new[](size_t size)
	{
		return ::operator new[](size, static_cast<std::align_val_t>(sizeof(vector_type)));
	}

	template<is_integer_vector T, typename Type, int _Element>
	inline T __vectorcall VectorIntBase<T, Type, _Element>::Scale(T const& a, T const& b)
	{
		return static_cast<T>(_mm_mullo_epi32(a._v, b._v));
	}

	template<is_integer_vector T, typename Type, int _Element>
	inline T __vectorcall VectorIntBase<T, Type, _Element>::Min(T const& value1, T const& value2)
	{
		return static_cast<T>(_mm_min_epi32(value1._v, value2._v));
	}

	template<is_integer_vector T, typename Type, int _Element>
	inline T __vectorcall VectorIntBase<T, Type, _Element>::Max(T const& value1, T const& value2)
	{
		return static_cast<T>(_mm_max_epi32(value1._v, value2._v));
	}

	template<is_integer_vector T, typename Type, int _Element>
	inline T __vectorcall VectorIntBase<T, Type, _Element>::Abs(T const& a)
	{
		return static_cast<T>(_mm_abs_epi32(a._v));
	}
}

//Basic Operation
namespace Math
{
	template<is_integer_vector T>
	inline T __vectorcall operator+(T const& left)
	{
		return left;
	}

	template<is_integer_vector T>
	inline T __vectorcall operator-(T const& left)
	{
		typename T::vector_type zero = _mm_set1_epi32(0); // Create a zero vector
		return static_cast<T>(_mm_sub_epi32(zero, left._v));
	}

	template<is_integer_vector T>
	inline T __vectorcall operator+(T const& left, T const& right)
	{
		return static_cast<T>(_mm_add_epi32(left._v, right._v));
	}

	template<is_integer_vector T>
	inline T __vectorcall operator-(T const& left, T const& right)
	{
		return static_cast<T>(_mm_sub_epi32(left._v, right._v));
	}

	template<is_integer_vector T>
	inline T __vectorcall operator*(T const& left, typename T::value_type  right)
	{
		return static_cast<T>(_mm_mullo_epi32(left._v, _mm_set1_epi32(right)));
	}

	template<is_integer_vector T>
	inline T __vectorcall operator*(typename T::value_type left, T const& right)
	{
		return static_cast<T>(_mm_mullo_epi32(right._v, _mm_set1_epi32(left)));
	}


	template<is_integer_vector T>
	inline bool __vectorcall operator==(T const& left, T const& right)
	{
		typename T::vector_type eq = _mm_cmpeq_epi32(left._v, right._v);
		return _mm_movemask_epi8(eq) == 0xFFFF; // All elements are equal if all bits are set
	}

	template<is_integer_vector T>
	inline bool __vectorcall operator!=(T const& left, T const& right)
	{
		return !(left == right);
	}

}
