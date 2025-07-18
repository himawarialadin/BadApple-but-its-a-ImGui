#pragma once

namespace Math
{
	class Vector2;
	class Vector3;
	class Vector4;

	class Vector2Int;

	class Quaternion;
	class Matrix4x4;

	class Color;
	class Bounds;

	static constexpr size_t VectorAlignment = 16;
}


#define __MATH_STATIC_CLASS(Type) \
Type() = delete; \
~Type() = delete; \
Type(Type const&) = delete; \
Type(Type&&) noexcept = delete; \
Type& operator=(Type const&) = delete; \
Type& operator=(Type&&) noexcept = delete;