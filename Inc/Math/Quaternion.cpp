#include "Quaternion.h"
#include "Math.h"
#include "Matrix4x4.h"
#include "Vector3.h"

#include <cmath>
#include <stdexcept>

namespace Math
{
	constinit const Quaternion Quaternion::Identity = Quaternion(0, 0, 0, 1);
}

namespace Math
{
	void* Quaternion::operator new(size_t size)
	{
		return _aligned_malloc(size, 16); 
	}

	void* Quaternion::operator new[](size_t size)
	{
		return _aligned_malloc(size, 16); 
	}

	float& __vectorcall Quaternion::operator[](size_t index) & noexcept(false)
	{
		if (index > static_cast<size_t>(3))
			throw  std::out_of_range("Index out of range.");

		return _array[index];
	}

	float __vectorcall Quaternion::operator[](size_t index) const&& noexcept(false)
	{
		if (index > static_cast<size_t>(3))
			throw  std::out_of_range("Index out of range.");

		return _array[index];
	}

	Quaternion __vectorcall operator*(const Quaternion& Q1, const Quaternion& Q2)
	{
		return (Quaternion)DirectX::XMQuaternionMultiply(Q1._v, Q2._v);
	}

	Vector3 __vectorcall operator*(const Quaternion& Q, const Vector3& vector)
	{
		return static_cast<Math::Vector3>(DirectX::XMVector3Rotate(vector._v, Q._v));

		//同様の実装はDirectX::XMVector3Rotate()で可能
		//const auto temp = Quaternion::Inverse(Q) * Quaternion(vector.x, vector.y, vector.z, 0);
		//const auto result = temp * Q;
		//return Vector3(result.x, result.y, result.z);
	}

	bool __vectorcall operator==(const Quaternion& Q1, const Quaternion& Q2)
	{
		return DirectX::XMQuaternionEqual(Q1._v, Q2._v);
	}

	void __vectorcall Quaternion::GetAxisAngle(_Out_ Vector3& outAxis, _Out_ float& outAngle) const
	{
		DirectX::XMQuaternionToAxisAngle(&outAxis._v, &outAngle, _v);
	}

	Vector3 Quaternion::ToEuler() const
	{
		//参考ソースコード(パクリですごめんなさい)：
		// https://github.com/TomHacker/DX11/blob/5e669e72873b5298a30eba322ff8733376098600/Transform.cpp (没)
		// https://github.com/microsoft/DirectXTK/blob/main/Inc/SimpleMath.inl#L3123 (現状の計算、度数法になるように改造済み)
		// https://qiita.com/edo_m18/items/5db35b60112e281f840e (計算方法)

		const float xx = x * x;
		const float yy = y * y;
		const float zz = z * z;

		const float m31 = 2.f * x * z + 2.f * y * w;
		const float m32 = 2.f * y * z - 2.f * x * w;
		const float m33 = 1.f - 2.f * xx - 2.f * yy;

		const float cy = sqrtf(m33 * m33 + m31 * m31);
		const float cx = atan2f(-m32, cy);
		if (cy > 16.f * FLT_EPSILON)
		{
			const float m12 = 2.f * x * y + 2.f * z * w;
			const float m22 = 1.f - 2.f * xx - 2.f * zz;

			return Vector3(cx, atan2f(m31, m33), atan2f(m12, m22)) * Math::ToDegrees;
		}
		else
		{
			const float m11 = 1.f - 2.f * yy - 2.f * zz;
			const float m21 = 2.f * x * y - 2.f * z * w;

			return Vector3(cx, 0.f, atan2f(-m21, m11)) * Math::ToDegrees;
		}
	}

	Quaternion __vectorcall Quaternion::FromRotationMatrix(const Matrix4x4& rotation)
	{
		return (Quaternion)DirectX::XMQuaternionRotationMatrix(rotation._m);
		//auto q = DirectX::XMQuaternionRotationMatrix(rotation._m);
		//return (Quaternion)DirectX::XMQuaternionNormalize(q);
	}

	Quaternion __vectorcall Quaternion::Euler(float x, float y, float z)
	{
		return Euler(Math::Vector3(x, y, z));
	}

	Quaternion __vectorcall Quaternion::Euler(const Vector3& eulerAngle)
	{
		 return (Quaternion)DirectX::XMQuaternionRotationRollPitchYawFromVector((eulerAngle * Math::ToRadian)._v);
	}

	Quaternion __vectorcall Quaternion::Inverse(const Quaternion& Q)
	{
		return (Quaternion)DirectX::XMQuaternionConjugate(Q._v);
	}

	Quaternion __vectorcall Quaternion::LookRotation(const Vector3& _forward, const Vector3& _upward /*= Vector3::Up*/)
	{
		//https://edom18.hateblo.jp/entry/2018/04/18/104054 参考
		//向きたい方向から三軸の方向ベクトルを求める
		const auto forward	= _forward.Normalize();
		const auto right		= Vector3::Cross(_upward, forward).Normalize();
		const auto up			= Vector3::Cross(forward, right).Normalize();

		//求めた三軸から回転行列を作成
		//http://marupeke296.com/DXG_No58_RotQuaternionTrans.html 参考
		const auto rotationMatrix = Matrix4x4(
			right.x,		right.y,		right.z,		0,
			up.x,			up.y,			up.z,			0,
			forward.x, forward.y, forward.z, 0,
			0,				0,				0,				1);

		//回転行列からクォータニオンに変換
		return FromRotationMatrix(rotationMatrix);
	}

	Quaternion __vectorcall Quaternion::FromToRotation(const Vector3& from, const Vector3& to)
	{
		constexpr float AngleEpsilon = 0.999999f;

		float dot = Vector3::Dot(from, to);
		if (dot > AngleEpsilon)//平行な場合
		{
			return Quaternion::Identity;
		}
		else if (dot < -AngleEpsilon) //逆方向の場合
		{
			Vector3 ortho = (Vector3)DirectX::XMVector3Orthogonal(from._v);
			return (Quaternion)DirectX::XMQuaternionRotationAxis(ortho._v, Math::Pi);
		}

		Vector3	axis		= Vector3::Cross(from, to).Normalize();
		float		angle		= acosf(dot);
		return (Quaternion)DirectX::XMQuaternionRotationAxis(axis._v, angle);
	}

	Quaternion __vectorcall Quaternion::Slerp(const Quaternion& start, const Quaternion& end, float t)
	{

		return (Quaternion)DirectX::XMQuaternionSlerp(start._v, end._v, t);
	}

	Quaternion __vectorcall Quaternion::AngleAxis(const Vector3& axis, float angle)
	{
		return (Quaternion)DirectX::XMQuaternionRotationAxis(axis._v, angle * Math::ToRadian);
	}

	float __vectorcall Quaternion::Dot(const Quaternion& Q1, const Quaternion& Q2)
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(Q1._v, Q2._v));
	}

}
