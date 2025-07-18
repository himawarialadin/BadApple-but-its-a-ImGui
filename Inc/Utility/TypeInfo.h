#pragma once
#include "Hash.h"

namespace Utility
{
	/// <summary>
	/// ê√ìIÇ»å^èÓïÒÇï€éùÇ∑ÇÈÉNÉâÉX
	/// </summary>
	class TypeInfo
	{
	public:
		constexpr TypeInfo()												= default;
		constexpr ~TypeInfo()											= default;
		constexpr TypeInfo(TypeInfo const&)						= default;
		constexpr TypeInfo(TypeInfo&&)								= default;
		constexpr TypeInfo& operator=(TypeInfo const&)		= default;
		constexpr TypeInfo& operator=(TypeInfo&&)			= default;

		template<typename T>
		static consteval TypeInfo Get();

		constexpr bool					Vaild()			const { return m_id != 0; }
		constexpr std::string_view	ClassName()	const { return m_name; }
		constexpr uint32_t				ID()				const { return m_id; }
		constexpr uint32_t				Size()			const { return m_size; }

		friend constexpr bool operator==	(TypeInfo const& lhs, TypeInfo const& rhs) { return lhs.m_id == rhs.m_id; }
		friend constexpr bool operator!=		(TypeInfo const& lhs, TypeInfo const& rhs) { return lhs.m_id != rhs.m_id; }
		friend constexpr bool operator<		(TypeInfo const& lhs, TypeInfo const& rhs) { return lhs.m_id < rhs.m_id; }
		friend constexpr bool operator>		(TypeInfo const& lhs, TypeInfo const& rhs) { return lhs.m_id > rhs.m_id; }

	private:
		template<typename T>
		static consteval std::string_view GetTypeName();

	private:
		std::string_view		m_name	= "";
		uint32_t				m_id			= 0;
		uint32_t				m_size		= 0;
	};

	template<typename T>
	inline consteval TypeInfo TypeInfo::Get()
	{
		TypeInfo info;
		info.m_name = GetTypeName<T>();
		info.m_id		= Utility::Hash32(info.m_name);
		info.m_size	= static_cast<uint32_t>( sizeof(T));
		return info;
	}

	template<typename T>
	inline consteval std::string_view TypeInfo::GetTypeName()
	{
		std::string_view name = __FUNCSIG__;
		std::string_view beginSplit = "GetTypeName<";
		std::string_view endSplit = ">(void)";

		auto beginPos = name.find(beginSplit) + beginSplit.size();
		auto endPos = name.find(endSplit);

		name = name.substr(beginPos, endPos - beginPos);
		return name;
	}


}

namespace std
{
	template <>
	struct std::hash<Utility::TypeInfo>
	{
		std::size_t operator()(Utility::TypeInfo const& k) const
		{
			return static_cast<size_t>(k.ID());
		}
	};
}