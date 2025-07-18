#pragma once
#include <ranges>
#include <tuple>

namespace Utility
{
	/*!
	@brief インデックス付き範囲アダプタ。範囲for文で要素とそのインデックスを取得するために使用します。
	@tparam T インデックスの型
	*/
	template <typename T, std::ranges::range R>
	auto enumerate_with_index(R&& range) requires std::is_arithmetic_v<T>
	{
		return std::views::transform(
			std::views::iota(static_cast<T>(0), static_cast<T>(std::ranges::distance(range))),
			[&range](auto i) { return std::pair{ i, std::ranges::begin(range)[i] }; });
	}
}