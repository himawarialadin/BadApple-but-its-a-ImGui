#pragma once
#include "MathFwd.h"
#include "Vector3.h"
#include <random>

namespace Math 
{
	/*!
	@brief	
	�����_���Ȓl�𐶐�����N���X�B
	���̃N���X�̓X���b�h�Z�[�t�ł����A�X���b�h���ɓƗ����������G���W���������܂��B
	*/
	class Random
	{
		__MATH_STATIC_CLASS(Random);
	public:
		//�w�肳�ꂽ�͈͓��̃����_���Ȑ����l��Ԃ��܂��B
		template<typename T> requires std::is_integral_v<T>
		static T	Range(T min, T max);
		
		template<typename T> requires std::is_floating_point_v<T>
		static T	Range(T min, T max);

		//0.0 ~ 1.0�͈͓̔��̃����_���ȕ��������l��Ԃ��܂��B
		static float Value() { return Range<float>(0.0f, 1.0f); }

		//���a1.0�̋��̓����܂��͋���̃����_���ȓ_��Ԃ��܂��B
		static Vector3 InsideUnitSphere(Math::Vector3 const& radius = Math::Vector3::One);

		//���a1.0�̗����̂̓����܂��͗����̂̋��E��̃����_���ȓ_��Ԃ��܂��B
		static Vector3 InsideUnitBox(Math::Vector3 const& harfExtents = Math::Vector3::One / 2.0f);

		//�����_���� Quaternion �^�̒l��Ԃ��܂�
		static Quaternion Rotation();

		//�w�肳�ꂽ Bounds �̓����܂��� Bounds �̋��E��̃����_���ȓ_��Ԃ��܂��B
		static Vector3 InsideBounds(Bounds const& bounds);

	public:
		//�C�ӂ̕��z�ɏ]�������_���Ȓl��Ԃ��܂��B
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