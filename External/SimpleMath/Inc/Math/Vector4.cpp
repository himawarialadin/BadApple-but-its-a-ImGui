#include "Vector4.h"
#include "Matrix4x4.h"
#include <stdexcept>

namespace Math
{
	constinit const Vector4 Vector4::Zero		= Vector4(0, 0, 0, 0);
	constinit const Vector4 Vector4::One		= Vector4(1, 1, 1, 1);
	constinit const Vector4 Vector4::Two		= Vector4(2, 2, 2, 2);
	constinit const Vector4 Vector4::Three	= Vector4(3, 3, 3, 3);
	constinit const Vector4 Vector4::For		= Vector4(4, 4, 4, 4);
	constinit const Vector4 Vector4::Five		= Vector4(5, 5, 5, 5);
}

namespace Math
{

	Vector4	__vectorcall operator*(const Matrix4x4& left, const Vector4& vector)
	{
		return (Vector4)DirectX::XMVector4Transform(vector._v, left._m);
	}

	float& __vectorcall Vector4::operator[](size_t index) & noexcept(false)
	{
		if (index > static_cast<size_t>( Element - 1))
			throw  std::out_of_range("Index out of range.");

		return _array[index];
	}

	float __vectorcall Vector4::operator[](size_t index) const&& noexcept(false)
	{
		if (index > static_cast<size_t>(Element - 1))
			throw std::out_of_range("Index out of range.");

		return _array[index];
	}

}
