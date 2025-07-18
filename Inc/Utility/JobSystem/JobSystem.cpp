#include "JobSystem.h"
#include <format>
#include <ranges>
#include <Windows.h>

#undef min
#undef max

namespace Utility
{
	
	inline static std::vector<JobHandle> MakeDepends(std::span<JobHandle> dependJobs)
	{
		if (dependJobs.empty())
			return {};

		auto range = std::views::filter(dependJobs, [](JobHandle& handle) {return !handle.IsEmpty(); });
		return std::vector<JobHandle>(range.begin(), range.end());
	}

	void JobSystem::Initialize()
	{
		for (uint32_t i = 0; i < MaxWorkers; i++)
			m_workers[i] = std::jthread(&JobSystem::WokerThread, this, i);
	}


	JobHandle JobSystem::ImplSchedule(PackagedTask&& task)
	{
		JobHandle handle(task.latch);
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_jobQueue.push(std::move(task));
			m_jobCount++;
			m_isComplete = false;
		}

		m_condition.notify_one();
		return handle;
	}

	JobHandle JobSystem::Schedule(IJob* job, std::span<JobHandle> dependJobs)
	{
		PackagedTask task;
		task.exexute			= [job] { job->Execute(); };
		task.latch				= std::make_shared<std::latch>(1);
		task.dependJobs	= MakeDepends(dependJobs);

		return ImplSchedule(std::move(task));
	}

	JobHandle JobSystem::ParallelFor(uint32_t elementCount, uint32_t parallelCount, IJobParallelFor* job, std::span<JobHandle> dependJobs /*= {}*/)
	{
		//要素数が並列数より少ない場合は、要素数分だけスレッドを立てる
		if (elementCount < parallelCount)
			parallelCount = elementCount;

		//並列数が０の場合は何もしない
		if (parallelCount == 0)
			return JobHandle();

		auto latch = std::make_shared<std::latch>(parallelCount);
		for (uint32_t parallelIndex = 0; parallelIndex < parallelCount; parallelIndex++)
		{
			//要素数が割り切れない場合は切り上げて、最後のスレッドが余りを無視する
			uint32_t sizePerParallel = (elementCount + parallelCount - 1) / parallelCount;

			auto wrapper = [job, sizePerParallel, parallelIndex, elementCount]()
				{
					job->OnBegin();

					for (uint32_t i = 0; i < sizePerParallel; i++)
					{
						uint32_t globalIndex = parallelIndex * sizePerParallel + i;

						//要素数オーバーしたら終了
						if (globalIndex >= elementCount)
							break;

						job->Execute(globalIndex);
					}

					job->OnEnd();
				};
		
			PackagedTask task;
			task.exexute			= wrapper;
			task.latch				= latch;
			task.dependJobs	= MakeDepends(dependJobs);
			ImplSchedule(std::move(task));
		}

		return JobHandle(latch);
	}

	JobHandle JobSystem::ParallelFor(uint32_t elementCount, uint32_t parallelCount, IJobParallelForEX* job, std::span<JobHandle> dependJobs)
	{
		//要素数が並列数より少ない場合は、要素数分だけスレッドを立てる
		if (elementCount < parallelCount)
			parallelCount = elementCount;

		//並列数が０の場合は何もしない
		if (parallelCount == 0)
			return JobHandle();

		auto latch = std::make_shared<std::latch>(parallelCount);

		for (uint32_t parallelIndex = 0; parallelIndex < parallelCount; parallelIndex++)
		{
			//要素数が割り切れない場合は切り上げて、最後のスレッドが余りを無視する
			uint32_t sizePerParallel = (elementCount + parallelCount - 1) / parallelCount;

			auto wrapper = [job, sizePerParallel, parallelIndex, elementCount]()
				{
					uint32_t begin	= parallelIndex * sizePerParallel;
					uint32_t end		= std::min(begin + sizePerParallel, elementCount);
					job->Execute(begin, end);
				};

			PackagedTask task;
			task.exexute			= wrapper;
			task.latch				= latch;
			task.dependJobs	= MakeDepends(dependJobs);
			ImplSchedule(std::move(task));
		}

		return JobHandle(latch);
	}

	void JobSystem::WokerThread(uint32_t threadID)
	{
		for (auto& lunchCallBack : m_lunchCallBackFunc)
			lunchCallBack(threadID);
		
		//名前つけ
		SetThreadDescription(GetCurrentThread(), std::format(L"JobWorker[{}]", threadID).c_str());
		
		while (true)
		{
			//ジョブをキューから取り出す
			PackagedTask job;
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_condition.wait(lock, [this] { return !m_jobQueue.empty() || m_isDestroy; });

				//デストラクタが呼ばれたらジョブキューを全て終わらしてから終了
				if (m_isDestroy && m_jobQueue.empty())
					break;

				job = std::move(m_jobQueue.front());
				m_jobQueue.pop();
			}

			//ジョブを実行
			{
				//依存ジョブが終わるまで待機
				for (auto& dependJob : job.dependJobs)
					dependJob.Complate();

				//ジョブを実行
				job.exexute();
			}

			//ジョブ終了
			if (m_jobCount.fetch_sub(1) == 1)
			{
				m_isComplete.store(true);
				m_isComplete.notify_all();
			}

				job.latch->count_down();
		}
	}

}