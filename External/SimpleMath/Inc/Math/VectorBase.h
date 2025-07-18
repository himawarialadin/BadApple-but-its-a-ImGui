#pragma once
#include "MathFwd.h"
#include <DirectXMath.h>
#include <concepts>

//Conceptrs
namespace Math
{
	template<typename T>
	concept is_floating_point_vector = std::same_as<T, Vector2> || std::same_as<T, Vector3> || std::same_as<T, Vector4>;
}

namespace Math::Internal
{
	template<is_floating_point_vector T, int _Element>
	class VectorBase
	{
	public:
		using Base				= VectorBase<T, _Element>;
		using value_type	= float;
		using vector_type	= DirectX::XMVECTOR;

	public:
		static constexpr int Element			= _Element;
		static constexpr int Size					= _Element * sizeof(value_type);
		static constexpr int MaxElement	= 4;
		
		static_assert(Element < MaxElement + 1 && Element > 0, "Illegal Element!");

	public:
		void* operator new	(size_t size);
		void* operator new	(size_t size, void* buf);
		void* operator new[]	(size_t size);

	public:
		static value_type		__vectorcall	Distance		(T const& a,			T const& b								);
		static value_type		__vectorcall	Length			(T const& a);
		static value_type		__vectorcall	LengthSq		(T const& a);
		static T						__vectorcall	Normalize	(T const& a);
		static T						__vectorcall	Scale			(T const& a,			T const& b								);
		static T						__vectorcall	Divide			(T const& a,			T const& b);
		static T						__vectorcall	Lerp				(T const& begin,	T const& end, value_type t);
		static T						__vectorcall	Clamp			(T const& value,	T const& min, T const& max	);
		static T						__vectorcall	Min				(T const& value1,	T const& value2);
		static T						__vectorcall	Max				(T const& value1,	T const& value2);
		static T						__vectorcall	Abs				(T const& a);
	};

	template<is_floating_point_vector T, int _Element>
	inline void* VectorBase<T, _Element>::operator new(size_t size)
	{
		return ::operator new(size, static_cast<std::align_val_t>(sizeof(vector_type)));
	}

	template<is_floating_point_vector T, int _Element>
	inline void* Math::Internal::VectorBase<T, _Element>::operator new(size_t size, void* buf)
	{
		return buf;
	}

	template<is_floating_point_vector T, int _Element>
	inline void* VectorBase<T, _Element>::operator new[](size_t size)
	{
		return ::operator new[](size, static_cast<std::align_val_t>(sizeof(vector_type)));
	}

	template<is_floating_point_vector T, int _Element>
	inline typename VectorBase<T, _Element>::value_type __vectorcall VectorBase<T, _Element>::Distance(T const& a, T const& b)
	{ return (b - a).Length(); }

	template<is_floating_point_vector T, int _Element>
	inline typename VectorBase<T, _Element>::value_type __vectorcall VectorBase<T, _Element>::Length(T const& a)
	{
		if constexpr (Element == 2)
			return DirectX::XMVectorGetX(DirectX::XMVector2Length(a._v));
		else if constexpr (Element == 3)
			return DirectX::XMVectorGetX(DirectX::XMVector3Length(a._v));
		else if constexpr (Element == 4)
			return DirectX::XMVectorGetX(DirectX::XMVector4Length(a._v));
		else
			[]<bool flag = false>()
			{ static_assert(flag, "Illegal Element"); }();
	}

	template<is_floating_point_vector T, int _Element>
	inline typename VectorBase<T, _Element>::value_type __vectorcall VectorBase<T, _Element>::LengthSq(T const& a)
	{
		if constexpr (Element == 2)
			return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(a._v));
		else if constexpr (Element == 3)
			return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(a._v));
		else if constexpr (Element == 4)
			return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(a._v));
		else
			[]<bool flag = false>()
			{ static_assert(flag, "Illegal Element"); }();
	}

	template<is_floating_point_vector T, int _Element>
	inline T __vectorcall VectorBase<T, _Element>::Normalize(T const& a)
	{
		if constexpr (Element == 2)
			return static_cast<T>(DirectX::XMVector2Normalize(a._v));
		else if constexpr(Element == 3)
			return static_cast<T>(DirectX::XMVector3Normalize(a._v));
		else if constexpr(Element == 4)
			return static_cast<T>(DirectX::XMVector4Normalize(a._v));
		else
			[]<bool flag = false>()
			{static_assert(flag, "Illegal Element");}();
		
	}

	template<is_floating_point_vector T, int _Element>
	inline T __vectorcall VectorBase<T, _Element>::Scale(T const& a, T const& b)
	{ 
		return static_cast<T>(DirectX::XMVectorMultiply(a._v, b._v)); 
	}

	template<is_floating_point_vector T, int _Element>
	inline T __vectorcall VectorBase<T, _Element>::Divide(T const& a, T const& b)
	{
		return static_cast<T>(DirectX::XMVectorDivide(a._v, b._v));
	}

	template<is_floating_point_vector T, int _Element>
	inline T __vectorcall VectorBase<T, _Element>::Lerp(T const& begin, T const& end, VectorBase<T, _Element>::value_type t)
	{
		return static_cast<T>(DirectX::XMVectorLerp(begin._v, end._v, t)); 
	}

	template<is_floating_point_vector T, int _Element>
	inline T __vectorcall VectorBase<T, _Element>::Clamp(T const& value, T const& min, T const& max)
	{ 
		return static_cast<T>(DirectX::XMVectorClamp(value._v, min._v, max._v)); 
	}

	template<is_floating_point_vector T, int _Element>
	inline T __vectorcall VectorBase<T, _Element>::Min(T const& value1, T const& value2)
	{
		return static_cast<T>(DirectX::XMVectorMin(value1._v, value2._v));
	}

	template<is_floating_point_vector T, int _Element>
	inline T __vectorcall VectorBase<T, _Element>::Max(T const& value1, T const& value2)
	{
		return static_cast<T>(DirectX::XMVectorMax(value1._v, value2._v));
	}

	template<is_floating_point_vector T, int _Element>
	inline T __vectorcall VectorBase<T, _Element>::Abs(T const& a)
	{
		return static_cast<T>(DirectX::XMVectorAbs(a._v));
	}
}

//Basic Operation
namespace Math
{
	template<is_floating_point_vector T>
	inline T __vectorcall operator+(T const& left)
	{
		return left;
	}

	template<is_floating_point_vector T>
	inline T __vectorcall operator-(T const& left)
	{
		return static_cast<T>(DirectX::XMVectorNegate(left._v));
	}

	template<is_floating_point_vector T>
	inline T __vectorcall operator+(T const& left, T const& right)
	{
		return static_cast<T>(DirectX::XMVectorAdd(left._v, right._v));
	}

	template<is_floating_point_vector T>
	inline T __vectorcall operator-(T const& left, T const& right)
	{
		return static_cast<T>(DirectX::XMVectorSubtract(left._v, right._v));
	}

	template<is_floating_point_vector T>
	inline T __vectorcall operator*(T const& left, typename T::value_type right)
	{
		return static_cast<T>(DirectX::XMVectorScale(left._v, right));
	}

	template<is_floating_point_vector T>
	inline T __vectorcall operator*(typename T::value_type  left, T const& right)
	{
		return static_cast<T>(DirectX::XMVectorScale(right._v, left));
	}

	template<is_floating_point_vector T>
	inline T __vectorcall operator/(T const& left, typename T::value_type right)
	{
		return left * (1.0f / right);
	}

	template<is_floating_point_vector T>
	inline bool __vectorcall operator==(T const& left, T const& right)
	{
		auto nearEqual	= DirectX::XMVectorNearEqual(left._v, right._v, DirectX::XMVectorSplatEpsilon());
		auto test			= _mm_movemask_ps(nearEqual);
		return test == 0xf;
	}

	template<is_floating_point_vector T>
	inline bool __vectorcall operator!=(T const& left, T const& right)
	{
		return !(left == right);
	}

}
