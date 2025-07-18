#pragma once
#include "VectorBase.h"

#pragma warning(push)
#pragma warning(disable:4201) //C4201: 非標準の拡張機能が使用されています: 無名の構造体または共用体です。
#pragma warning(disable:4324) //C4324: アラインメント指定子のために構造体がパッドされました

namespace Math 
{
	//DirectX::XMVECTOR 2次元floatベクトル版拡張クラス
	class alignas(DirectX::XMVECTOR) Vector2 final : public Internal::VectorBase<Vector2, 2>
	{
	public:
		union
		{
			vector_type	_v;
			DirectX::XMFLOAT2A _float;

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
					constexpr Vector2()																: x(),		y()									{}
					constexpr Vector2(value_type _x, value_type _y)						: x(_x), y(_y)									{}
		explicit	constexpr Vector2(_In_reads_(Element) value_type* array)		: _array{array[0],array[1]}			{}

		explicit	Vector2(vector_type				const& vector)	: _v(vector)									{}
					Vector2(DirectX::XMFLOAT2	const& float2)	: x(float2.x), y(float2.y)					{}
		explicit	Vector2(DirectX::XMFLOAT3	const& float3)	: x(float3.x), y(float3.y)					{}
		explicit	Vector2(DirectX::XMFLOAT4	const& float4)	: x(float4.x), y(float4.y)					{}

	public:
		explicit	operator value_type* ()					const { return _array;		}
		explicit	operator DirectX::XMVECTOR()		const { return _v; }
					operator DirectX::XMFLOAT2()		const { return DirectX::XMFLOAT2(x, y);			}
		explicit	operator DirectX::XMFLOAT3()		const { return DirectX::XMFLOAT3(x, y, 0);		}
		explicit	operator DirectX::XMFLOAT4()		const { return DirectX::XMFLOAT4(x, y, 0, 0);	}

	public:
		float&		__vectorcall operator[]	(size_t index) &				noexcept(false);
		float			__vectorcall operator[]	(size_t index) const&&	noexcept(false);

		Vector2&	__vectorcall operator+=(Vector2 const& vector)		& { return *this = *this + vector;	}
		Vector2&	__vectorcall operator-=	(Vector2 const& vector)		& { return *this = *this - vector;	}
		Vector2&	__vectorcall operator*=	(float scaler)						& { return *this = *this * scaler;	}
		Vector2&	__vectorcall operator/=	(float scaler)						& { return *this = *this / scaler;		}

	public:
		static float	 __vectorcall	Dot		(Vector2 const& left, Vector2 const& right) { return DirectX::XMVectorGetX(DirectX::XMVector2Dot(left._v, right._v)); }
		static float	 __vectorcall	Cross	(Vector2 const& left, Vector2 const& right) { return DirectX::XMVectorGetX(DirectX::XMVector2Cross(left._v, right._v));}

	public:
		Vector2 __vectorcall	Normalize	()	const { return static_cast<Vector2>(DirectX::XMVector2Normalize(_v)); }
		float		__vectorcall	Length		()	const { return DirectX::XMVectorGetX(DirectX::XMVector2Length(_v)); }
		float		__vectorcall	LengthSq	()	const { return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(_v)); }
	
	public://Constants
		static constinit const Vector2 Zero;
		static constinit const Vector2 One;
		static constinit const Vector2 Two;
		static constinit const Vector2 Three;
		static constinit const Vector2 For;
		static constinit const Vector2 Five;
	};
}

#pragma warning(pop)