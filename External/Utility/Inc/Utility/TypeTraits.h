#pragma once
#include <type_traits>

namespace Utility::Internal
{
	template<typename T, bool = std::is_enum_v<T>>
	struct internal_underlying_type_or_default { using type = std::underlying_type_t<T>; };

	template<typename T>
	struct internal_underlying_type_or_default<T, false> { using type = T; };
}

namespace Utility
{
	template<typename T>
	//�^T��Enum�^�ł���΂��̊��^��Ԃ��A����Ȃ���Ό^T��Ԃ��܂��B
	struct underlying_type_or_default { using type = Internal::internal_underlying_type_or_default<T>::type; };


	template<typename T>
	using underlying_type_or_default_t = underlying_type_or_default<T>;


}
