#pragma once
#include <cstdint>
#include <DirectXMath.h>
#include "VectorIntBase.h"


#pragma warning(push)
#pragma warning(disable:4201)
#pragma warning(disable:4324) //C4324: アラインメント指定子のために構造体がパッドされました

namespace Math
{


	//整数2次元ベクトル  ※SIMD非対応
	class Vector2Int : public Internal::VectorIntBase<Vector2Int, int32_t, 2>
	{
	public:
		union
		{
			vector_type _v;

			struct alignas(vector_type)
			{
				DirectX::XMINT2 _int;
			};

			struct alignas(vector_type)
			{
				value_type x, y;
			};

			struct alignas(vector_type)
			{
				mutable value_type _array[Element];
			};
		};

	public:
					constexpr	Vector2Int() : x(), y() {}
					constexpr	Vector2Int(value_type _x, value_type _y) : x(_x), y(_y) {}
		explicit	constexpr	Vector2Int(_In_reads_(Element) value_type* array) : _array{ array[0],array[1] } {}
					constexpr	Vector2Int(DirectX::XMINT2 const& vector) : _int(vector) {}
		explicit					Vector2Int(vector_type const& vector) : _v(vector) {}

		Vector2Int& __vectorcall operator+=	(const Vector2Int& vector)& { return *this = *this + vector; }
		Vector2Int& __vectorcall operator-=	(const Vector2Int& vector)& { return *this = *this - vector; }
		Vector2Int& __vectorcall operator*=	(value_type scaler)& { return *this = *this * scaler; }

	public:
	};
}

#pragma warning(pop)