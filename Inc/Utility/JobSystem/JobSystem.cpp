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
		//�v�f�������񐔂�菭�Ȃ��ꍇ�́A�v�f���������X���b�h�𗧂Ă�
		if (elementCount < parallelCount)
			parallelCount = elementCount;

		//���񐔂��O�̏ꍇ�͉������Ȃ�
		if (parallelCount == 0)
			return JobHandle();

		auto latch = std::make_shared<std::latch>(parallelCount);
		for (uint32_t parallelIndex = 0; parallelIndex < parallelCount; parallelIndex++)
		{
			//�v�f��������؂�Ȃ��ꍇ�͐؂�グ�āA�Ō�̃X���b�h���]��𖳎�����
			uint32_t sizePerParallel = (elementCount + parallelCount - 1) / parallelCount;

			auto wrapper = [job, sizePerParallel, parallelIndex, elementCount]()
				{
					job->OnBegin();

					for (uint32_t i = 0; i < sizePerParallel; i++)
					{
						uint32_t globalIndex = parallelIndex * sizePerParallel + i;

						//�v�f���I�[�o�[������I��
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
		//�v�f�������񐔂�菭�Ȃ��ꍇ�́A�v�f���������X���b�h�𗧂Ă�
		if (elementCount < parallelCount)
			parallelCount = elementCount;

		//���񐔂��O�̏ꍇ�͉������Ȃ�
		if (parallelCount == 0)
			return JobHandle();

		auto latch = std::make_shared<std::latch>(parallelCount);

		for (uint32_t parallelIndex = 0; parallelIndex < parallelCount; parallelIndex++)
		{
			//�v�f��������؂�Ȃ��ꍇ�͐؂�グ�āA�Ō�̃X���b�h���]��𖳎�����
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
		
		//���O��
		SetThreadDescription(GetCurrentThread(), std::format(L"JobWorker[{}]", threadID).c_str());
		
		while (true)
		{
			//�W���u���L���[������o��
			PackagedTask job;
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_condition.wait(lock, [this] { return !m_jobQueue.empty() || m_isDestroy; });

				//�f�X�g���N�^���Ă΂ꂽ��W���u�L���[��S�ďI��炵�Ă���I��
				if (m_isDestroy && m_jobQueue.empty())
					break;

				job = std::move(m_jobQueue.front());
				m_jobQueue.pop();
			}

			//�W���u�����s
			{
				//�ˑ��W���u���I���܂őҋ@
				for (auto& dependJob : job.dependJobs)
					dependJob.Complate();

				//�W���u�����s
				job.exexute();
			}

			//�W���u�I��
			if (m_jobCount.fetch_sub(1) == 1)
			{
				m_isComplete.store(true);
				m_isComplete.notify_all();
			}

				job.latch->count_down();
		}
	}

}