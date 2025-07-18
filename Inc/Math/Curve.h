#pragma once
#include "Vector2.h"
#include <vector>

namespace Math
{
	//�񎟌��X���[�Y�X�v���C����ԋȐ�
	struct Curve
	{
	public:
		constexpr Curve(std::initializer_list<Math::Vector2> _points) : points(_points) {}
		constexpr Curve()													= default;
		constexpr Curve(Curve const&)								= default;
		constexpr Curve(Curve&&) noexcept						= default;
		constexpr Curve& operator=(Curve const&)			= default;
		constexpr Curve& operator=(Curve&&) noexcept		= default;

	public:
		/*!
		@brief 	�P�ʒ����𐶐����܂��B
		�P�ʒ����Ƃ́A(0,height)����(length,height)�܂ł̒����ł��B
		@param	length	�����̒���
		@param	height	�����̍���
		*/
		static Curve UnitLine(float length = 1.0f, float height = 0.0f) { return Curve({ {0.0f, height}, {length, height} }); }

		/*!
		@brief 	�����𐶐����܂��B
		@param	start		�����̎n�_
		@param	end		�����̏I�_
		*/
		static Curve Lineer(Math::Vector2 const& start, Math::Vector2 const& end) { return Curve({ start, end }); }

	public:
		uint32_t	Count()			const { return static_cast<uint32_t>(points.size()); }
		float			GetMinX()		const { return points.empty() ? 0.0f : points.front().x; }
		float			GetMaxX()		const { return points.empty() ? 0.0f : points.back().x; }

		Math::Vector2 const&		BeginPoint()	const { return points.empty() ? Math::Vector2::Zero : points.front(); }
		Math::Vector2 const&		EndPoint()		const { return points.empty() ? Math::Vector2::Zero : points.back(); }
		float								Evaluate(float t);

	public:
		std::vector<Math::Vector2> points;

	};

}