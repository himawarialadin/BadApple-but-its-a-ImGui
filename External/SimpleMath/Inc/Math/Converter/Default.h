#pragma once
#include "ConverterFunc.h"
#include "../Vector2Int.h"
#include "../Vector2.h"
#include "../Vector3.h"
#include "../Vector4.h"

#include <DirectXMath.h>

namespace Math
{
	//-Vector2Int----------------------------------------------------//
	template<>
	constexpr Vector2Int Cast(DirectX::XMVECTOR const& v)
	{
		return Vector2Int(
			static_cast<Vector2Int::value_type>(DirectX::XMVectorGetX(v)), 
			static_cast<Vector2Int::value_type>(DirectX::XMVectorGetY(v)));
	}

	template<>
	constexpr Vector2Int Cast(Vector2 const& v)
	{
		return Vector2Int(static_cast<Vector2Int::value_type>(v.x), static_cast<Vector2Int::value_type>(v.y));
	}

	//-Vector2----------------------------------------------------//
	template<>
	constexpr Vector2 Cast(DirectX::XMVECTOR const& v)
	{
		return Vector2(v);
	}

	template<>
	constexpr Vector2 Cast(Vector2Int const& v)
	{
		return Vector2(static_cast<float>(v.x), static_cast<float>(v.y));
	}


	//-Vector3----------------------------------------------------//
	template<>
	constexpr Vector3 Cast(DirectX::XMVECTOR const& v)
	{
		return Vector3(v);
	}

	template<>
	constexpr Vector3 Cast(Vector2Int const& v, float z)
	{
		return Vector3(static_cast<float>(v.x), static_cast<float>(v.y), z);
	}

	template<>
	constexpr Vector3 Cast(Vector2 const& v, float z)
	{
		return Vector3(v.x, v.y, z);
	}

	//-Vector4----------------------------------------------------//
	template<>
	constexpr Vector4 Cast(DirectX::XMVECTOR const& v)
	{
		return Vector4(v);
	}

	template<>
	constexpr Vector4 Cast(Vector2 const& v, float z, float w)
	{
		return Vector4(v.x, v.y, z, w);
	}

	template<>
	constexpr Vector4 Cast(Vector3 const& v, float w)
	{
		return Vector4(v.x, v.y, v.z, w);
	}
	

	void Func()
	{
		Vector2 a(1.0f, 2.0f);
		Vector4 v = Cast<Vector4>(a, 3.0f, 4.0f);
	}

}
