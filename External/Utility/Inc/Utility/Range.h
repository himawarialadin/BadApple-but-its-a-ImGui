#pragma once
#include <ranges>
#include <tuple>

namespace Utility
{
	/*!
	@brief �C���f�b�N�X�t���͈̓A�_�v�^�B�͈�for���ŗv�f�Ƃ��̃C���f�b�N�X���擾���邽�߂Ɏg�p���܂��B
	@tparam T �C���f�b�N�X�̌^
	*/
	template <typename T, std::ranges::range R>
	auto enumerate_with_index(R&& range) requires std::is_arithmetic_v<T>
	{
		return std::views::transform(
			std::views::iota(static_cast<T>(0), static_cast<T>(std::ranges::distance(range))),
			[&range](auto i) { return std::pair{ i, std::ranges::begin(range)[i] }; });
	}
}