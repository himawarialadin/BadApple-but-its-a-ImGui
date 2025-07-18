#pragma once
#include "JobInterface.h"
#include <functional>

namespace Utility
{
	/*!
	@brief 空っぽのジョブ。グループ化したジョブの最後に追加することで、ジョブの終了を検知できます。
	*/
	struct EmptyJob : public IJob
	{
		void Execute() override {}
	};

	struct LambdaJob : public IJob
	{
	public:
		std::function<void()> execute;

	private:
		void Execute() override
		{ execute(); }
	};

	/*!@brief 
	ラムダ式での並列Forジョブ
	このジョブはOnBegin、OnEndは呼ばれません。
	*/
	struct LambdaParallelForJob : public IJobParallelFor
	{
	public:
		//! @brief 実行する関数
		std::function<void(uint32_t)> execute;

	private:
		void Execute(uint32_t index) override
		{ execute(index); }

	};

	struct LambdaParallelForEXJob : public IJobParallelForEX
	{
	public:
		//! @brief 実行する関数
		std::function<void(uint32_t, uint32_t)> execute;

	private:
		void Execute(uint32_t beginIndex, uint32_t endIndex) override
		{
			execute(beginIndex, endIndex);
		}
	};

}