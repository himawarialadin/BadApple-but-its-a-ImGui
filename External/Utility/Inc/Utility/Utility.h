#pragma once
#include <type_traits>
#include <utility>

#define TypeNameString(Type) #Type

namespace Utility 
{
	namespace TypeTraits
	{
		//debug���[�h�ł��邩����
		struct is_debug_mode :
#ifdef _DEBUG
			std::true_type
#else
			std::false_type
#endif
		{};

		template <typename T> class remove_all_pointers {
		public: typedef T type;
		};

		template <typename T> class remove_all_pointers<T*> {
		public: typedef typename remove_all_pointers<T>::type type;
		};

		//pointer,const,�Q��,�z���S�ď������^��Ԃ�
		template<typename T>
		using atomic_type = std::remove_const<typename std::remove_all_extents<typename remove_all_pointers<T>::type>::type>::type;

		//is_same�̕����w��o�[�W����(OR����)
		template <class T, class... Ts>
		struct is_any : std::disjunction<std::is_same<T, Ts>...> {};

		//is_same�̕����w��o�[�W����(AND����)
		template <class T, class... Ts>
		struct are_same : std::bool_constant<(std::is_same_v<T, Ts> && ...)> {};

		//�^�������^�ł��邩����
		template<typename T>
		struct is_char : is_any<atomic_type<T>, char, wchar_t, char8_t, char16_t, char32_t> {};

		

		template<class T, class U>
		T base_of(U T::*) {}

	};

	namespace Concepts
	{

		//�����_���œ���̖߂�l�A�����������Ă��邩���o
		template <class T, typename Return, typename ...Args >
		concept is_lamda = requires
		{
			std::is_invocable_v<T, Args...>&& std::is_same_v<std::invoke_result_t<T, Args...>, Return>;
		};

		//�����^�ł��邩���o
		template<typename T>
		concept is_charactor = TypeTraits::is_char<T>::value;

		template<typename T,typename Base>
		concept derived_from_or_type = requires
		{ std::derived_from<T, Base> || std::same_as<T, Base>; };

		template<class From, class To>
		concept is_cast_to = requires{ static_cast<To>(std::declval<From>()); };
	};

	/// <summary>
	/// �n�b�V������
	/// </summary>
	template<typename T>
	void CombineHash(size_t& seed, T const& v) 
	{
		//��{�^�Ɋւ���n�b�V�������͕W�����C�u�������񋟂��Ă���
		std::hash<T> primitive_type_hash;

		//���������n�b�V������������B���̃R�[�h��boost���̂��g�p����
		seed ^= primitive_type_hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	inline size_t operator""_z(unsigned long long value) 
	{
		return static_cast<size_t>(value);
	}

	//std::map,std::unorderd_map�ŃL�[��std::pair�ɂ������Ƃ��Ɏg�p����t�@���N�^
	struct HashPair 
	{
		template<class T1, class T2>
		size_t operator()(const std::pair<T1, T2>& p) const {

			//first�����n�b�V��������
			auto hash1 = std::hash<T1>{}(p.first);

			//second�����n�b�V��������
			auto hash2 = std::hash<T2>{}(p.second);

			//�d�����Ȃ��悤�Ƀn�b�V������
			size_t seed = 0;
			seed ^= hash1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}

		template<typename T>
		bool operator()(const T& l, const T& r) const
		{
			if (l.first == r.first) {
				return l.second > r.second;
			}

			return l.first < r.first;
		}
	};


}
