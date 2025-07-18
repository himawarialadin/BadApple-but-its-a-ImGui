#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Math.h"

#include <stdexcept>


namespace Math
{
	constinit const Matrix4x4 Matrix4x4::Zero = 
		Matrix4x4(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

	constinit const Matrix4x4 Matrix4x4::Identity =
		Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
}

namespace Math 
{
	float Matrix4x4::operator()(size_t row, size_t column) noexcept(false)
	{
		if (row > 3)		throw std::out_of_range("Index out of range");
		if (column > 3)	throw std::out_of_range("Index out of range");
		return _array[row][column];	
	}

	Matrix4x4 __vectorcall Matrix4x4::operator-()
	{
		Matrix4x4 m;
		m._v[0] = DirectX::XMVectorNegate(_v[0]);
		m._v[1] = DirectX::XMVectorNegate(_v[1]);
		m._v[2] = DirectX::XMVectorNegate(_v[2]);
		m._v[3] = DirectX::XMVectorNegate(_v[3]);
		return m;
	}

	Matrix4x4 __vectorcall Math::Matrix4x4::SetSRT(Vector3 const& scale, Vector3 const& rotation, Vector3 const& transrate)
	{
		Matrix4x4 S = DirectX::XMMatrixScalingFromVector(scale._v);
		Matrix4x4 R = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation._v);
		Matrix4x4 T = DirectX::XMMatrixTranslationFromVector(transrate._v);
		return S * R * T;
	}

	Matrix4x4 __vectorcall Matrix4x4::SetSRT(Vector3 const& scale, Quaternion const& rotation, Vector3 const& transrate)
	{
		Matrix4x4 S = DirectX::XMMatrixScalingFromVector(scale._v);
		Matrix4x4 R = DirectX::XMMatrixRotationQuaternion(rotation._v);
		Matrix4x4 T = DirectX::XMMatrixTranslationFromVector(transrate._v);
		return S * R * T;	
	}

	Matrix4x4 __vectorcall Matrix4x4::LookAt(Vector3 const& eyePosition, Vector3 const& focusPosition, Vector3 const& upDirection)
	{
		return DirectX::XMMatrixLookAtLH(eyePosition._v, focusPosition._v, upDirection._v);
	}

	Matrix4x4 __vectorcall Matrix4x4::LookTo(Vector3 const& eyePosition, Vector3 const& eyeDirection, Vector3 const& upDirection)
	{
		return DirectX::XMMatrixLookToLH(eyePosition._v, eyeDirection._v, upDirection._v);
	}

	Matrix4x4 __vectorcall Matrix4x4::Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ)
	{
		return DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
	}

	Matrix4x4 __vectorcall Matrix4x4::AffineTransformation2D(Vector2 const& scale, Vector2 const& rotationOrigin, float rotateAngleScaler, Vector2 const& translation)
	{
		return DirectX::XMMatrixAffineTransformation2D(scale._v, rotationOrigin._v, rotateAngleScaler * ToRadian, translation._v);
	}

	Matrix4x4 __vectorcall Matrix4x4::Parspective(float fov, float aspect, float nearZ, float farZ)
	{
		return DirectX::XMMatrixPerspectiveFovLH(fov * Math::ToRadian, aspect, nearZ, farZ);
	}

	Matrix4x4 __vectorcall Matrix4x4::Scale(Vector3 const& scale)
	{
		return DirectX::XMMatrixScalingFromVector(scale._v);
	}

	Matrix4x4 __vectorcall Matrix4x4::Translate(Vector3 const& translate)
	{
		return DirectX::XMMatrixTranslationFromVector(translate._v);
	}

	Matrix4x4 __vectorcall Matrix4x4::Rotation(Quaternion const& rotation)
	{
		return DirectX::XMMatrixRotationQuaternion(rotation._v);
		
	}

	Matrix4x4 __vectorcall Matrix4x4::Rotation(Vector3 const& angle)
	{
		return DirectX::XMMatrixRotationRollPitchYawFromVector((angle * Math::ToRadian)._v);
	}

	Matrix4x4 __vectorcall operator+(const Matrix4x4& left, const Matrix4x4& right)
	{
		Matrix4x4 m;
		m._v[0] = DirectX::XMVectorAdd(left._v[0], right._v[0]);
		m._v[1] = DirectX::XMVectorAdd(left._v[1], right._v[1]);
		m._v[2] = DirectX::XMVectorAdd(left._v[2], right._v[2]);
		m._v[3] = DirectX::XMVectorAdd(left._v[3], right._v[3]);
		return m;
	}

	Matrix4x4 __vectorcall operator-(const Matrix4x4& left, const Matrix4x4& right)
	{
		Matrix4x4 m;
		m._v[0] = DirectX::XMVectorSubtract(left._v[0], right._v[0]);
		m._v[1] = DirectX::XMVectorSubtract(left._v[1], right._v[1]);
		m._v[2] = DirectX::XMVectorSubtract(left._v[2], right._v[2]);
		m._v[3] = DirectX::XMVectorSubtract(left._v[3], right._v[3]);

		return m;
	}

	Matrix4x4 __vectorcall operator*(const Matrix4x4& left, const Matrix4x4& right)
	{
		 return DirectX::XMMatrixMultiply(left, right);
	}

	Matrix4x4 __vectorcall operator*(const Matrix4x4& left, float scaler)
	{
		Matrix4x4 m;
		m._v[0] = DirectX::XMVectorScale(left._v[0], scaler);
		m._v[1] = DirectX::XMVectorScale(left._v[1], scaler);
		m._v[2] = DirectX::XMVectorScale(left._v[2], scaler);
		m._v[3] = DirectX::XMVectorScale(left._v[3], scaler);
		return m;
	}

	Matrix4x4 __vectorcall operator/(const Matrix4x4& left, float scaler)
	{
		return left * (scaler / 1.0f);
	}

}
