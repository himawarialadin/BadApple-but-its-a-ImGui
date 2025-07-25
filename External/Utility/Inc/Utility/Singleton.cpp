#include "Singleton.h"
#include <vector>
#include <mutex>

namespace Utility::Internal
{
	std::mutex																				mutex;
	std::vector<std::pair<SingletonFinalizer::FinalizerFunc, int>>	finalizers;

	void SingletonFinalizer::AddFinalizer(FinalizerFunc func, int destoryPriority)
	{
		std::lock_guard<std::mutex> lock(mutex);
	
		auto iter = finalizers.begin();
		while (iter != finalizers.end())
		{
			auto& [finalizer, priority] = *iter;
			if (priority >= destoryPriority) //優先度が高いものが見つかったらそこに挿入
				break;
	
			++iter;
		}
	
		finalizers.emplace(iter, func, destoryPriority);
	}
	
	void SingletonFinalizer::Finalize()
	{
		std::lock_guard<std::mutex> lock(mutex);
		for (auto& [finalizer, priority] : finalizers)
			(*finalizer)();
	}
}