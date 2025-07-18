#pragma once
#include "MathFwd.h"
#include "Vector3.h"
#include <random>

namespace Math 
{
	/*!
	@brief	
	ランダムな値を生成するクラス。
	このクラスはスレッドセーフですが、スレッド毎に独立した乱数エンジンを持ちます。
	*/
	class Random
	{
		__MATH_STATIC_CLASS(Random);
	public:
		//指定された範囲内のランダムな整数値を返します。
		template<typename T> requires std::is_integral_v<T>
		static T	Range(T min, T max);
		
		template<typename T> requires std::is_floating_point_v<T>
		static T	Range(T min, T max);

		//0.0 ~ 1.0の範囲内のランダムな浮動小数値を返します。
		static float Value() { return Range<float>(0.0f, 1.0f); }

		//半径1.0の球の内部または球上のランダムな点を返します。
		static Vector3 InsideUnitSphere(Math::Vector3 const& radius = Math::Vector3::One);

		//半径1.0の立方体の内部または立方体の境界上のランダムな点を返します。
		static Vector3 InsideUnitBox(Math::Vector3 const& harfExtents = Math::Vector3::One / 2.0f);

		//ランダムな Quaternion 型の値を返します
		static Quaternion Rotation();

		//指定された Bounds の内部または Bounds の境界上のランダムな点を返します。
		static Vector3 InsideBounds(Bounds const& bounds);

	public:
		//任意の分布に従うランダムな値を返します。
		template<typename T, typename Distribution>
		static T	Range(Distribution& dist);

	private:
		static std::mt19937& Engine()
		{
			thread_local std::mt19937 engine(m_randomDevice());
			return engine;
		}

	private:
		inline static std::random_device	m_randomDevice;
	};

	template<typename T> requires std::is_integral_v<T>
	inline T Random::Range(T min, T max)
	{
		if (min == max) return min;

		auto range = std::uniform_int_distribution<T>(min, max);
		return range(Engine());
	}

	template<typename T> requires std::is_floating_point_v<T>
	inline T Random::Range(T min, T max)
	{
		if (min == max) return min;

		auto	 range = std::uniform_real_distribution<T>(min, max);
		return range(Engine());
	}

	template<typename T, typename Distribution>
	inline T Random::Range(Distribution& dist)
	{
		return dist(Engine());
	}
}