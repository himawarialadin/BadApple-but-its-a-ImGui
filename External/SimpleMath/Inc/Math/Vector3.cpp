#include "Vector3.h"
#include <stdexcept>

namespace Math
{
	constinit const Vector3 Vector3::Zero			= Vector3(0, 0, 0);
	constinit const Vector3 Vector3::One			= Vector3(1, 1, 1);
	constinit const Vector3 Vector3::Two			= Vector3(2, 2, 2);
	constinit const Vector3 Vector3::Three		= Vector3(3, 3, 3);
	constinit const Vector3 Vector3::For			= Vector3(4, 4, 4);
	constinit const Vector3 Vector3::Five			= Vector3(5, 5, 5);

	constinit const Vector3 Vector3::Up				= Vector3(0, 1, 0);
	constinit const Vector3 Vector3::Down		= Vector3(0, -1, 0);
	constinit const Vector3 Vector3::Forward	= Vector3(0, 0, 1);
	constinit const Vector3 Vector3::Back			= Vector3(0, 0, -1);
	constinit const Vector3 Vector3::Left			= Vector3(-1, 0, 0);
	constinit const Vector3 Vector3::Right		= Vector3(1, 0, 0);
}

namespace Math
{
	float& __vectorcall Vector3::operator[](size_t index) & noexcept(false)
	{
		if (index > static_cast<size_t>(Element - 1))
			throw std::out_of_range("Index out of range.");

		return _array[index];
	}

	float	__vectorcall Vector3::operator[](size_t index) const&& noexcept(false)
	{
		if (index > static_cast<size_t>(Element - 1))
			throw std::out_of_range("Index out of range.");

		return _array[index];
	}
}
