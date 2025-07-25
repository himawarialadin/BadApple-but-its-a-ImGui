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
	//型TがEnum型であればその基底型を返し、それなければ型Tを返します。
	struct underlying_type_or_default { using type = Internal::internal_underlying_type_or_default<T>::type; };


	template<typename T>
	using underlying_type_or_default_t = underlying_type_or_default<T>;


}
