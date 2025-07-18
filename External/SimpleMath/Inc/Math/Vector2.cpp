#include "Vector2.h"

#include "Vector3.h"
#include "Vector4.h"
#include "Math.h"

#include <stdexcept>


namespace Math
{
	constinit const Vector2 Vector2::Zero		= Vector2(0, 0);
	constinit const Vector2 Vector2::One		= Vector2(1, 1);
	constinit const Vector2 Vector2::Two		= Vector2(2, 2);
	constinit const Vector2 Vector2::Three	= Vector2(3, 3);
	constinit const Vector2 Vector2::For		= Vector2(4, 4);
	constinit const Vector2 Vector2::Five		= Vector2(5, 5);
}

namespace Math 
{
	float& __vectorcall Vector2::operator[](size_t index) & noexcept(false)
	{
		if (index > static_cast<size_t>(Element - 1))
			throw std::out_of_range("Index out of range.");

		return _array[index];
	}

	float	__vectorcall Vector2::operator[](size_t index) const&& noexcept(false)
	{
		if (index > static_cast<size_t>(Element - 1))
			throw std::out_of_range("Index out of range.");

		return _array[index];
	}
}
