#pragma once
#include <memory>

namespace Utility
{
	class Any
	{
	public:
		template <typename T>
		struct is_not_void_pointer {
			static constexpr bool value = !std::is_same_v<void*, std::remove_cvref_t<T>>;
		};

	public:
		Any() = default;
		virtual ~Any() {}

		virtual const void*	Get()								const		= 0;
		//virtual void*			Get()											= 0;
		virtual void			Set(const void* value)				= 0;

		template<typename T>
		const T* Get() const
		{ 
			return static_cast<const T*>(Get()); 
		}

		//template<typename T>
		//T* Get() const
		//{
		//	return static_cast<T*>(Get());
		//}

		template<typename T>
		void Set(T const& value)
		{
			Set(static_cast<const void*>(&value));
		}
	};

	template<typename T>
	class AnyT : public Any
	{
	public:
		AnyT(T const& value) : m_value(value) {}
		~AnyT() override {}

		const void*	Get()								const		override { return &m_value; }
		//void*				Get()											override { return &m_value; }
		void				Set(const void* value)				override { m_value = *static_cast<const T*>(value); }

	private:
		T m_value;
	};

	template<typename T>
	inline std::unique_ptr<Any> CreateAny(T const& value) requires Any::is_not_void_pointer<T>::value
	{
		return std::make_unique<AnyT<std::remove_cvref_t< T>>>(value);
	}

}
