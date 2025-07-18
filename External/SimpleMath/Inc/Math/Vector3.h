#pragma once
#include "VectorBase.h"
#include "Math.h"

#pragma warning(push)
#pragma warning(disable:4201)
#pragma warning(disable:4324) //C4324: アラインメント指定子のために構造体がパッドされました

namespace Math 
{
	//DirectX::XMVECTOR 3次元floatベクトル版拡張クラス
	class alignas(DirectX::XMVECTOR) Vector3 final : public Internal::VectorBase<Vector3, 3>
	{
	public:
		union
		{
			vector_type	_v;
			DirectX::XMFLOAT3A _float;

			struct alignas(vector_type)
			{
				value_type x, y, z;
			};

			struct alignas(vector_type)
			{
				mutable value_type _array[Element];
			};
		};

	public:
					constexpr	Vector3()																					: x(),			y(),		z()						{}
					constexpr	Vector3(value_type _x, value_type _y, value_type _z = 0.0f)		: x(_x),		y(_y),	z(_z)						{}
		explicit	constexpr	Vector3(_In_reads_(Element) value_type* array)						: _array{array[0],array[1],array[2] }	{}

					constexpr	Vector3(DirectX::XMVECTOR const & vector)								: _v(vector)										{}
		explicit	constexpr	Vector3(DirectX::XMFLOAT2 const & float3, value_type _z = 0)	: x(float3.x), y(float3.y), z(_z)			{}
					constexpr	Vector3(DirectX::XMFLOAT3 const & float3)									: x(float3.x), y(float3.y), z(float3.z)		{}
		explicit	constexpr	Vector3(DirectX::XMFLOAT4 const & float4)									: x(float4.x), y(float4.y), z(float4.z)		{}

	public:
		explicit	operator value_type* ()					const { return _array;		}
		explicit	operator DirectX::XMVECTOR()		const { return _v; }
		explicit	operator DirectX::XMFLOAT2()		const { return DirectX::XMFLOAT2(x, y);			}
					operator DirectX::XMFLOAT3()		const { return DirectX::XMFLOAT3(x, y, z);		}
		explicit	operator DirectX::XMFLOAT4()		const { return DirectX::XMFLOAT4(x, y, z, 0);	}

	public:
		float&		__vectorcall operator[]	(size_t index) &				noexcept(false);
		float			__vectorcall operator[]	(size_t index) const&&	noexcept(false);

		Vector3&	__vectorcall operator+=(Vector3 const& vector)		& { return *this = *this + vector;	}
		Vector3&	__vectorcall operator-=	(Vector3 const& vector)		& { return *this = *this - vector;	}
		Vector3&	__vectorcall operator*=	(value_type scaler)			& { return *this = *this * scaler;	}
		Vector3&	__vectorcall operator/=	(value_type scaler)			& { return *this = *this / scaler;		}

	public:
		static value_type	__vectorcall	Dot(Vector3 const& left, Vector3 const& right)		{ return DirectX::XMVectorGetX(DirectX::XMVector3Dot(left._v, right._v)); }
		static Vector3		__vectorcall	Cross(Vector3 const& left, Vector3 const& right) { return Vector3(DirectX::XMVector3Cross(left._v, right._v)); }
		static Vector3		__vectorcall	ProjectOnPlane(Vector3 const& vector, Vector3 const& planeNormal);

	public:
		Vector3 __vectorcall	Normalize	()	const { return static_cast<Vector3>(DirectX::XMVector3Normalize(_v)); }
		float		__vectorcall	Length		()	const { return DirectX::XMVectorGetX(DirectX::XMVector3Length(_v)); }
		float		__vectorcall	LengthSq	()	const { return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(_v)); }

	public://Constants
		static constinit const Vector3 Zero;
		static constinit const Vector3 One;
		static constinit const Vector3 Two;
		static constinit const Vector3 Three;
		static constinit const Vector3 For;
		static constinit const Vector3 Five;

		static constinit const Vector3 Up;
		static constinit const Vector3 Down;
		static constinit const Vector3 Forward;
		static constinit const Vector3 Back;
		static constinit const Vector3 Left;
		static constinit const Vector3 Right;
	};

	inline Vector3 __vectorcall Vector3::ProjectOnPlane(Vector3 const& vector, Vector3 const& planeNormal)
	{
		value_type num = Dot(planeNormal, planeNormal);
		if (num < Math::Epsilon)
			return vector;

		value_type num2 = Dot(vector, planeNormal);
		return vector - planeNormal * (num2 / num);
	}

}

#pragma warning(pop)