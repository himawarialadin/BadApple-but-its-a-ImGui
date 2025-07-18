#pragma once
#include <type_traits>

#define ENABLE_ENUM_CLASS_BITWISE_OPERATORS(T) template<> constexpr bool ::Utility::enable_enum_class_bitwise_operators_v<T> = true

namespace Utility
{
	/// <summary>
	/// ���̒萔���t���O�n���Z�q��L���ɂ�����Enum�^�œ��ꉻ����true�������Ă�������
	/// </summary>
	template<typename T> requires std::is_enum_v<T>
	constexpr bool enable_enum_class_bitwise_operators_v = false;

	/// <summary>
	/// a�̒l��b�̃t���O���܂܂�Ă��邩�擾���܂��B
	/// <para>���̊֐��̓t���O�̗񋓑̌^T��enable_enum_class_bitwise_operators_v��T����true�łȂ��Ǝg�p�ł��܂���B</para>
	/// </summary>
	/// <returns></returns>
	template<typename T> requires enable_enum_class_bitwise_operators_v<T>
	constexpr bool HasFlag(T a, T b) 
	{ 
		using UnderlyingType = std::underlying_type_t<T>;
		return (static_cast<UnderlyingType>(a) & static_cast<UnderlyingType>(b)) != static_cast<UnderlyingType>(0);
	}
}

template<typename T> requires Utility::enable_enum_class_bitwise_operators_v<T>
constexpr T operator&(T a, T b)
{
	using UnderlyingType = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<UnderlyingType>(a) & static_cast<UnderlyingType>(b));
}

template<typename T> requires Utility::enable_enum_class_bitwise_operators_v<T>
constexpr T operator|(T a, T b)
{
	using UnderlyingType = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<UnderlyingType>(a) | static_cast<UnderlyingType>(b));
}

template<typename T> requires Utility::enable_enum_class_bitwise_operators_v<T>
constexpr T operator^(T a, T b)
{
	using UnderlyingType = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<UnderlyingType>(a) ^ static_cast<UnderlyingType>(b));
}

template<typename T> requires Utility::enable_enum_class_bitwise_operators_v<T>
constexpr T operator~(T a)
{
	using UnderlyingType = std::underlying_type_t<T>;
	return static_cast<T>(~static_cast<UnderlyingType>(a));
}

template<typename T> requires Utility::enable_enum_class_bitwise_operators_v<T>
constexpr T operator&=(T& a, T b) { return a = a & b; }

template<typename T> requires Utility::enable_enum_class_bitwise_operators_v<T>
constexpr T operator|=(T& a, T b) { return a = a | b; }

template<typename T> requires Utility::enable_enum_class_bitwise_operators_v<T>
constexpr T operator^=(T& a, T b) { return a = a ^ b; }