#pragma once
#include <limits>
#include <concepts>
#include <cmath>

//Math::Vector2,Math::Vector3,Math::Quertarnion�ȂǂŁA�����׊֐�(Length(),Normalize()�Ȃ�)������������ꍇ�͂�����A���R�����g
//�����������ꍇ�́A�v�Z�덷���傫���Ȃ�ꍇ������̂Œ���
//#define MATH_VECTOR_USE_XMVECTOR_EST_FUNCTIONS


//���w�n���̃��\�b�h�Q
namespace Math
{
	//float�ŕ\���ł���ŏ��l
	static constexpr float Min		= std::numeric_limits<float>::lowest();
	//float�ŕ\���ł���ő�l
	static constexpr float Max		= (std::numeric_limits<float>::max)();
	//float�̋��e�덷�l
	static constexpr float Epsilon	= std::numeric_limits<float>::epsilon();

	static constexpr float Pi			= 3.14159265358979323846f;		//�~������
	static constexpr float Pi2		= Pi * 2.0f;								//�~�����̓�{��(2��)
	static constexpr float DivPi		= 1.0f / Pi;								//�΂̋t�� 
	static constexpr float DivPi2	= 1.0f / Pi2;								//2�΂̋t�� 
	static constexpr float PiDiv2	= Pi / 2.0f;								//�� / 2.0f
	static constexpr float PiDiv4	= Pi / 4.0f;								//�� / 4.0f

	// radian = (degree) * ToRadian
	static constexpr float ToRadian		= Pi / 180.0f;
	// degree = (radian) * ToDegrees
	static constexpr float ToDegrees		= 180.0f / Pi;

	/// <summary>
	/// �w���������Ȃ������̗ݏ�v�Z���s���܂��B
	/// </summary>
	constexpr float PowInt(float a, unsigned int b);

	/// <summary>
	/// �������̋t�����v�Z���܂��B(��constexpr)
	/// std::sqrt�̑���ɂ��̊֐����g�����Ƃō����������҂ł��܂��B
	/// </summary>
	float InvSqrt(float x);

	/// <summary>
	/// ���������v�Z���܂��B(��constexpr)
	/// </summary>
	inline float Sqrt(float x) { return 1.0f / InvSqrt(x); }

	/// <summary>
	///  ���lnumber�ɂ��āA�����_normal�ȉ��Ŏl�̌ܓ��������̂�Ԃ��܂��B(��constexpr)
	/// </summary>
	float RoundN(float number, unsigned int normal);

	/// <summary>
	/// 2 �̕��������_�l���r���A�ߎ����Ă���ꍇ�� true ��Ԃ��܂��B
	/// </summary>
	constexpr bool Approximately(float a, float b, float epsilon = Epsilon);
	
	/// <summary>
	/// ���l�̕�����Ԃ��܂��B
	/// <para>value �����̏ꍇ��1���A���̏ꍇ��-1���A0�̏ꍇ��0��Ԃ��܂��B</para>
	/// </summary>
	constexpr float Sign(float value);

	/// <summary>
	/// �l���w�肳�ꂽ�͈͓��ɂ��邩�ǂ����𔻒f���܂��B
	/// </summary>
	constexpr bool InRange(float value, float min, float max);

	/// <summary>
	/// ���`��Ԃ��s���܂��B
	/// </summary>
	constexpr float Lerp(float a, float b, float t);

	/// <summary>
	/// �w�肳�ꂽ�l���w�肳�ꂽ�{���ɐ؂�グ�܂��B
	/// </summary>
	template<std::integral T>
	constexpr T CeilAtMultiple(T value, T malutiple);

	/// <summary>
	/// �w�肳�ꂽ�l���w�肳�ꂽ2�ׂ̂���ɐ؂�グ�܂��B
	/// </summary>
	template<std::integral T>
	constexpr T CeilAtPow2(T value, T pow2);

	/// <summary>
	/// value�̒l��min����max�͈̔͂���newMin����newMax�͈̔͂ɕϊ����܂��B
	/// </summary>
	constexpr float Remap(float value, float min, float max, float newMin = 0, float newMax = 1);

	/// <summary>
	/// �l�����͈̔͂Ɏ��߂�
	/// </summary>
	constexpr float Clamp(float value, float min, float max);

	/// <summary>
	/// �l�̍ő�l��Ԃ��܂��B
	/// </summary>
	constexpr float Maxf(float a, float b);

	/// <summary>
	/// �l�̍ŏ��l��Ԃ��܂��B
	/// </summary>
	constexpr float Minf(float a, float b);

	/// <summary>
	/// �l�̐�Βl��Ԃ��܂��B
	/// </summary>
	constexpr float Absf(float value);

	/// <summary>
	/// �����̏���������Ԃ��܂��B
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
	//https://martin.ankerl.com/2012/01/25/optimized-approximative-pow-in-c-and-cpp/ �����p
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
		//a,b�̒l���C�v�V�����l��菬�����ꍇ�́A������|���Ĕ�r
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
