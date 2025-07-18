#pragma once
#include "Macros.h"
#include <cassert>
#include <utility>

namespace Utility::Internal
{
	class SingletonFinalizer final
	{
		STATIC_CLASS(SingletonFinalizer);
	public:
		using FinalizerFunc = void(*)();

	public:
		static void AddFinalizer(FinalizerFunc func, int destoryPriority);

		//追加された終了処理を全て呼び出す
		//注意：アプリケーション終了時に必ず呼んでください。
		static void Finalize();		
	};
}

namespace Utility
{
	/// <summary>
	/// シングルトンを有効にするクラス
	/// </summary>
	template<typename T>
	class EnableSingleton final
	{
		friend class Singleton;
		STATIC_CLASS(EnableSingleton);
	public:
		static bool HasValue()
		{
			return instance != nullptr;
		}

	private:
		static T*& Instance()
		{
			return instance;
		}

		template<typename ...Args>
		static void	Constract(Args&& ...args)
		{
			instance = new T(std::forward<Args>(args)...);
		}

		static void Destory()
		{
			//C++14以降からnullptrをdeleteしても問題ない
			delete instance;
			instance = nullptr;
		}

	private:
		static inline bool		initFlag;
		static inline T*		instance = nullptr;
	};

	class Singleton final
	{
		STATIC_CLASS(Singleton);
	public:
		template<typename T, int _DestoryPriority = 0, typename ...Args>
		static T& Register(Args&&... constractArgs)
		{
			//コピー、ムーブ構築および代入ができないかどうか
			static_assert(!std::is_copy_constructible_v<T> || !std::is_copy_assignable_v<T> || !std::is_move_constructible_v<T> || !std::is_move_assignable_v<T>,
				"To make a singleton, copy, move construction and copy, move assignment must be disabled.");

			assert(EnableSingleton<T>::Instance() == nullptr);

			EnableSingleton<T>::Constract(std::forward<Args>(constractArgs)...);
			Internal::SingletonFinalizer::AddFinalizer(EnableSingleton<T>::Destory, _DestoryPriority);			

			return *EnableSingleton<T>::Instance();
		}

		template<typename T, typename ...Args>
		static T& DestoryAndConstruct(Args&&... constractArgs)
		{
			if (EnableSingleton<T>::HasValue())
				EnableSingleton<T>::Destory();

			EnableSingleton<T>::Constract(std::forward<Args>(constractArgs)...);
			return *EnableSingleton<T>::Instance();
		}

		template<typename T>
		static T& Instance()
		{
			auto& instance = EnableSingleton<T>::Instance();
			assert(instance != nullptr);

			return *instance;
		}

		template<typename T>
		static bool HasValue()
		{
			return EnableSingleton<T>::HasValue();
		}
	};
}