#include "Color.h"
#include "Vector4.h"
#include <stdexcept>

namespace Math
{
	constinit const Color Color::Clear = { 0, 0, 0, 0 };
}

namespace Math
{
	Color::Color(Vector4 const& v) : _v(v._v) { }
	
	Color::operator Vector4() const
	{
		return static_cast<Vector4>(_v);
	}

	float& __vectorcall Color::operator[](size_t index) & noexcept(false)
	{
		if (index > 3)
			throw std::out_of_range("index out of range");
	
		return _array[index];
	}

	float __vectorcall Color::operator[](size_t index) const&& noexcept(false)
	{
		if (index > 3)
			throw  std::out_of_range("index out of range");

		return _array[index];
	}


	bool __vectorcall operator==(Color const& left, Color const& right)
	{
		return DirectX::XMColorEqual(left._v, right._v);
	}

	bool __vectorcall operator!=(Color const& left, Color const& right)
	{
		return !(left == right);
	}

	Color __vectorcall operator+(Color const& left, Color const& right)
	{
		Color c = static_cast<Color>( DirectX::XMVectorAdd(left._v, right._v));
		return c;
	}

	Color __vectorcall operator-(Color const& left, Color const& right)
	{
		Color c = static_cast<Color>(DirectX::XMVectorSubtract(left._v, right._v));
		return c;
	}

	Color __vectorcall operator*(Color const& left, Color const& right)
	{
		Color c = static_cast<Color>(DirectX::XMVectorMultiply(left._v, right._v));
		return c;
	}

	Color __vectorcall operator*(Color const& left, float scaler)
	{
		Color c = static_cast<Color>(DirectX::XMVectorScale(left._v, scaler));
		return c;
	}

	Color __vectorcall operator/(Color const& left, float scaler)
	{
		return left * (1.0f / scaler);
	}

	unsigned int Color::ToRGBA32ColorCode() const
	{
		__m128 color = _mm_mul_ps(_v, _mm_set1_ps(255));
		float colorArray[4];
		_mm_store_ps(colorArray, color);
		return ((int)colorArray[0] << 24) + ((int)colorArray[1] << 16) + ((int)colorArray[2] << 8) + ((int)colorArray[3] << 0);
	}

	unsigned int Color::ToABGR32ColorCode() const
	{
		__m128 color = _mm_mul_ps(_v, _mm_set1_ps(255));
		float colorArray[4];
		_mm_store_ps(colorArray, color);
		return ((int)colorArray[3] << 24) + ((int)colorArray[2] << 16) + ((int)colorArray[1] << 8) + ((int)colorArray[0] << 0);
	}

	Color Color::HSVToRGB(float H, float S, float V)
	{
		return static_cast<Color>(DirectX::XMColorHSVToRGB(DirectX::XMVectorSet(H, S, V, 1.0f)));
	}

	void Color::RGBToHSV(Color const& rgb, _Out_ float& outH, _Out_ float& outS, _Out_ float& outV)
	{
		Vector4 hsv = static_cast<Vector4>(DirectX::XMColorRGBToHSV(rgb._v));
		outH = hsv.x;
		outS = hsv.y;
		outV = hsv.z;
	}

	Color Color::Lerp(Color const& a, Color const& b, float t)
	{
		return static_cast<Color>(DirectX::XMVectorLerp(a._v, b._v, t));
	}

	
}