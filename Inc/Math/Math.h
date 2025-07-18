#pragma once
#include <limits>
#include <concepts>
#include <cmath>

//Math::Vector2,Math::Vector3,Math::Quertarnionなどで、高負荷関数(Length(),Normalize()など)を高速化する場合はこれをアンコメント
//高速化した場合は、計算誤差が大きくなる場合があるので注意
//#define MATH_VECTOR_USE_XMVECTOR_EST_FUNCTIONS


//数学系統のメソッド群
namespace Math
{
	//floatで表現できる最小値
	static constexpr float Min		= std::numeric_limits<float>::lowest();
	//floatで表現できる最大値
	static constexpr float Max		= (std::numeric_limits<float>::max)();
	//floatの許容誤差値
	static constexpr float Epsilon	= std::numeric_limits<float>::epsilon();

	static constexpr float Pi			= 3.14159265358979323846f;		//円周率π
	static constexpr float Pi2		= Pi * 2.0f;								//円周率の二倍τ(2π)
	static constexpr float DivPi		= 1.0f / Pi;								//πの逆数 
	static constexpr float DivPi2	= 1.0f / Pi2;								//2πの逆数 
	static constexpr float PiDiv2	= Pi / 2.0f;								//π / 2.0f
	static constexpr float PiDiv4	= Pi / 4.0f;								//π / 4.0f

	// radian = (degree) * ToRadian
	static constexpr float ToRadian		= Pi / 180.0f;
	// degree = (radian) * ToDegrees
	static constexpr float ToDegrees		= 180.0f / Pi;

	/// <summary>
	/// 指数が符号なし整数の累乗計算を行います。
	/// </summary>
	constexpr float PowInt(float a, unsigned int b);

	/// <summary>
	/// 平方根の逆数を計算します。(非constexpr)
	/// std::sqrtの代わりにこの関数を使うことで高速化が期待できます。
	/// </summary>
	float InvSqrt(float x);

	/// <summary>
	/// 平方根を計算します。(非constexpr)
	/// </summary>
	inline float Sqrt(float x) { return 1.0f / InvSqrt(x); }

	/// <summary>
	///  数値numberについて、小数点normal以下で四捨五入したものを返します。(非constexpr)
	/// </summary>
	float RoundN(float number, unsigned int normal);

	/// <summary>
	/// 2 つの浮動小数点値を比較し、近似している場合は true を返します。
	/// </summary>
	constexpr bool Approximately(float a, float b, float epsilon = Epsilon);
	
	/// <summary>
	/// 数値の符号を返します。
	/// <para>value が正の場合は1を、負の場合は-1を、0の場合は0を返します。</para>
	/// </summary>
	constexpr float Sign(float value);

	/// <summary>
	/// 値が指定された範囲内にあるかどうかを判断します。
	/// </summary>
	constexpr bool InRange(float value, float min, float max);

	/// <summary>
	/// 線形補間を行います。
	/// </summary>
	constexpr float Lerp(float a, float b, float t);

	/// <summary>
	/// 指定された値を指定された倍数に切り上げます。
	/// </summary>
	template<std::integral T>
	constexpr T CeilAtMultiple(T value, T malutiple);

	/// <summary>
	/// 指定された値を指定された2のべき乗に切り上げます。
	/// </summary>
	template<std::integral T>
	constexpr T CeilAtPow2(T value, T pow2);

	/// <summary>
	/// valueの値をminからmaxの範囲からnewMinからnewMaxの範囲に変換します。
	/// </summary>
	constexpr float Remap(float value, float min, float max, float newMin = 0, float newMax = 1);

	/// <summary>
	/// 値を一定の範囲に収める
	/// </summary>
	constexpr float Clamp(float value, float min, float max);

	/// <summary>
	/// 値の最大値を返します。
	/// </summary>
	constexpr float Maxf(float a, float b);

	/// <summary>
	/// 値の最小値を返します。
	/// </summary>
	constexpr float Minf(float a, float b);

	/// <summary>
	/// 値の絶対値を返します。
	/// </summary>
	constexpr float Absf(float value);

	/// <summary>
	/// 実数の少数部分を返します。
	/// </summary>
	inline float Fraction(float value)
	{
		return value - std::floor(value);
	}
}

#pragma region 

namespace Math
{
	constexpr float Maxf(float a, float b)
	{
		return a > b ? a : b;
	}

	constexpr float Minf(float a, float b)
	{
		return a < b ? a : b;
	}

	constexpr float Absf(float a)
	{
		return a < 0.0f ? -a : a;
	}
}

namespace Math
{
	//https://martin.ankerl.com/2012/01/25/optimized-approximative-pow-in-c-and-cpp/ より引用
	constexpr float PowInt(float a, unsigned int b)
	{
		double a_double = a;

		double r = 1.0;
		while (b) {
			if (b & 1) {
				r *= a_double;
			}
			a_double *= a_double;
			b >>= 1;
		}

		return static_cast<float>(a_double);
	}

	inline float InvSqrt(float x)
	{
		// y = 1/sqrt(x) = 2 ^ (-1/2 * log2(x))
		long X, Y;
		float y;
		X = *(long*)&x;
		Y = 0x5F3759DF - (X >> 1); // Magic number!
		y = *(float*)&Y;

		// Newton's method
		const float threehalfs = 1.5F;
		float x2 = x * 0.5F;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration
		//  y = y * (threehalfs - (x2 * y * y));   // 2nd iteration

		return y;
	}

	inline float RoundN(float number, unsigned int normal)
	{
		number = number * static_cast<float>(PowInt(10.0, normal - 1U));
		number = std::roundf(number);
		number /= static_cast<float>(PowInt(10.0, normal - 1U));
		return number;
	}

	constexpr bool Approximately(float a, float b, float epsilon)
	{
		//a,bの値がイプシロン値より小さい場合は、それを掛けて比較
		return Absf(a - b) <= epsilon * Maxf(1.0f, Maxf(Absf(a), Absf(b)));
	}

	constexpr float Sign(float value)
	{
		return static_cast<float>(value > 0.0f) - static_cast<float>(value < 0.0f);
	}

	constexpr bool InRange(float value, float min, float max)
	{
		return min > value && value < max;
	}
		
	constexpr float Lerp(float a, float b, float t)
	{
		return a + (b - a) * t;
	}

	template<std::integral T>
	constexpr T CeilAtMultiple(T value, T malutiple)
	{
		return value + malutiple - value % malutiple;
	}

	template<std::integral T>
	constexpr T CeilAtPow2(T value, T pow2)
	{
		auto bit = pow2 - static_cast<T>(1);
		return (value + bit) & ~bit;
	}

	constexpr float Remap(float value, float min, float max, float newMin, float newMax)
	{
		return newMin + ((value - min) / (max - min) * (newMax - newMin));
	}

	constexpr float Clamp(float value, float min, float max)
	{
		return Minf(Maxf(value, min), max);
	}
}



#pragma endregion
