#pragma once
#include "Vector2.h"
#include <vector>

namespace Math
{
	//二次元スムーズスプライン補間曲線
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
		@brief 	単位直線を生成します。
		単位直線とは、(0,height)から(length,height)までの直線です。
		@param	length	直線の長さ
		@param	height	直線の高さ
		*/
		static Curve UnitLine(float length = 1.0f, float height = 0.0f) { return Curve({ {0.0f, height}, {length, height} }); }

		/*!
		@brief 	直線を生成します。
		@param	start		直線の始点
		@param	end		直線の終点
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