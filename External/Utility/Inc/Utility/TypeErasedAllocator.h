#pragma once
#include "TypeInfo.h"

namespace Utility
{
	//å^è¡ãéÇ≥ÇÍÇΩä÷êîåƒÇ—èoÇµópÇÃç\ë¢ëÃ 
	struct TypeErasedAllocator
	{
		using ConstructFunc	= void* (*)(void* buffer);
		using DestructFunc		= void	(*)(void* instance);
		using MoveFunc			= void	(*)(void* src, void* dest);
		using CopyFunc			= void	(*)(void* src, void* dest);
		using SwapFunc			= void	(*)(void* src, void* dest);

		ConstructFunc	construct	= nullptr;
		DestructFunc 	destruct	= nullptr;
		MoveFunc			move		= nullptr;
		CopyFunc			copy			= nullptr;
		SwapFunc			swap			= nullptr;
		TypeInfo			typeInfo;

		template<typename T>
		static TypeErasedAllocator Create()
		{
			TypeErasedAllocator invoker;
			invoker.construct = [](void* buffer) -> void*
				{
					return new(buffer) T();
				};

			invoker.destruct = [](void* instance) -> void
				{
					reinterpret_cast<T*>(instance)->~T();
				};

			invoker.move = [](void* src, void* dst) -> void
				{
					*reinterpret_cast<T*>(dst) = std::move(*reinterpret_cast<T*>(src));
				};

			invoker.copy = [](void* src, void* dst) -> void
				{
					*reinterpret_cast<T*>(dst) = *reinterpret_cast<T*>(src);
				};

			invoker.swap = [](void* src, void* dst) -> void
				{
					std::swap(*reinterpret_cast<T*>(src), *reinterpret_cast<T*>(dst));
				};

			invoker.typeInfo = TypeInfo::Get<T>();
			return invoker;
		}
	};
}