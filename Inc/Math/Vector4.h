#pragma once
#include "VectorBase.h"
#include "MathFwd.h"

#pragma warning(push)
#pragma warning(disable:4201)
#pragma warning(disable:4324) //C4324: アラインメント指定子のために構造体がパッドされました

namespace Math 
{
	//DirectX::XMVECTOR 4次元floatベクトル版拡張クラス
	class alignas(DirectX::XMVECTOR) Vector4 final : public Internal::VectorBase<Vector4, 4>
	{
	public:
		union
		{
			vector_type				_v;
			DirectX::XMFLOAT4A _float;

			struct alignas(vector_type)
			{
				float x, y, z, w;
			};

			struct alignas(vector_type)
			{
				mutable float _array[Element];
			};
		};

	public:
					constexpr	Vector4()																				
			: x(0), y(0), z(0), w(0) {}
					constexpr	Vector4(value_type _x, value_type _y, value_type _z, value_type _w = 0.0f)	
			: x(_x),	 y(_y), z(_z), w(_w) {}
		explicit	constexpr	Vector4(_In_reads_(Element) value_type* array)								
			: _array{array[0],array[1],array[2],array[3] } {}

		explicit	constexpr	Vector4(vector_type				const& vector)												
			: _v(vector)	{}
		explicit	constexpr	Vector4(DirectX::XMFLOAT2	const & float2, value_type	_z = 0, value_type _w = 0)		
			: x(float2.x), y(float2.y), z(_z),w(_w)	{}
		explicit	constexpr	Vector4(DirectX::XMFLOAT3	const & float3, value_type _w = 0)
			: x(float3.x), y(float3.y), z(float3.z),w(_w) {}
					constexpr	Vector4(DirectX::XMFLOAT4	const & float4)												
			: x(float4.x), y(float4.y), z(float4.z) {}

		
		explicit	operator value_type* ()					const { return _array; }
		explicit	operator vector_type()					const { return _v; }
		explicit	operator DirectX::XMFLOAT2()		const { return DirectX::XMFLOAT2(x, y);			}
		explicit	operator DirectX::XMFLOAT3()		const { return DirectX::XMFLOAT3(x, y, z);		}
					operator DirectX::XMFLOAT4()		const { return DirectX::XMFLOAT4(x, y, z, w);	}

	public://Operators
		friend Vector4	__vectorcall operator*(Matrix4x4 const& left, Vector4 const& vector);
		
		value_type&		__vectorcall operator[]	(size_t index) &				noexcept(false);
		value_type		__vectorcall operator[]	(size_t index) const&&	noexcept(false);

		Vector4&	__vectorcall operator+=(Vector4 const& vector)		& { return *this = *this + vector;	}
		Vector4&	__vectorcall operator-=	(Vector4 const& vector)		& { return *this = *this - vector;	}
		Vector4&	__vectorcall operator*=	(value_type scaler)			& { return *this = *this * scaler;	}
		Vector4&	__vectorcall operator/=	(value_type scaler)			& { return *this = *this / scaler;		}

	public:
		static float	  __vectorcall	 Dot(Vector4 const& left, Vector4 const& right){  return DirectX::XMVectorGetX(DirectX::XMVector4Dot(left._v, right._v)); }

	public:
		Vector4 __vectorcall	Normalize	()	const;
		float		__vectorcall	Length		()	const;
		float		__vectorcall	LengthSq	()	const;

	public://Constants
		static constinit const Vector4 Zero;
		static constinit const Vector4 One;
		static constinit const Vector4 Two;
		static constinit const Vector4 Three;
		static constinit const Vector4 For;
		static constinit const Vector4 Five;
	};

}

#pragma warning(pop)