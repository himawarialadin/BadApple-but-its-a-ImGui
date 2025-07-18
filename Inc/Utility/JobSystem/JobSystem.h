#pragma once
#include "JobHandle.h"
#include "JobInterface.h"

#include <queue>
#include <thread>
#include <functional>
#include <array>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <span>

#include "Macros.h"

namespace Utility
{
	class JobSystem final
	{
		ENABLE_SINGLETON(JobSystem);
		DELETE_COPY_MOVE_COUNSTRACTORS(JobSystem);

	public:
		static inline constexpr size_t MaxWorkers = 12;
		using LunchCallBackFunc = void(*)(uint32_t threadID);

	public:
		JobSystem() = default;
		~JobSystem() { m_isDestroy = true; m_condition.notify_all(); }

		void Initialize();

	public:
		/*!
		@brief �X���b�h���N�������ۂɌĂ΂��R�[���o�b�N�֐���ݒ肵�܂��B
		*/
		void SetThreadOnLunchCallBack(LunchCallBackFunc func) { m_lunchCallBackFunc.emplace_back(func); }

		/*!
		@brief �W���u��V�K�ǉ����܂��B�ǉ���A�����Ɏ��s����܂��B
		@param job �W���u
		@param dependJobs �ˑ�����W���u�B�ˑ��W���u���I���܂őҋ@���܂��B
		@return �W���u�n���h���B�W���u�̊�����҂��Ƃ��ł��܂��B
		@warning job�͎��s�����܂ŕێ������K�v������܂��B
		*/
		JobHandle	Schedule(IJob* job, std::span<JobHandle> dependJobs = {});

		/*!
		@brief �����̃W���u�������s���܂��B
		@param parallelCount ����
		@param elementCount ��������v�f��
		@param job �W���u
		@param dependJobs �ˑ�����W���u�B�ˑ��W���u���I���܂őҋ@���܂��B
		@return �W���u�n���h���B�W���u�̊�����҂��Ƃ��ł��܂��B
		@warning job�͎��s�����܂ŕێ������K�v������܂��B
		*/
		JobHandle ParallelFor(uint32_t elementCount, uint32_t parallelCount, IJobParallelFor*		job, std::span<JobHandle> dependJobs = {});
		JobHandle ParallelFor(uint32_t elementCount, uint32_t parallelCount, IJobParallelForEX*	job, std::span<JobHandle> dependJobs = {});

		void WaitForAll() const { m_isComplete.wait(false); }

	private:
		struct PackagedTask
		{
			std::function<void()>	exexute;
	
			std::shared_ptr<std::latch>			latch;
			std::vector<JobHandle>				dependJobs;
		};

	private:
		JobHandle ImplParallelFor(uint32_t elementCount, uint32_t parallelCount, std::function<void()>&& job, std::span<JobHandle> dependJobs);
		JobHandle ImplSchedule(PackagedTask&& task);
		void WokerThread(uint32_t threadID);

	private:
		std::vector<LunchCallBackFunc>			m_lunchCallBackFunc;

		std::queue<PackagedTask>					m_jobQueue;
		std::array<std::jthread, MaxWorkers>	m_workers;
		std::mutex											m_mutex;
		std::condition_variable							m_condition;

		std::atomic<bool>								m_isDestroy		= false;
		std::atomic<bool>								m_isComplete	= false;
		std::atomic<uint32_t>							m_jobCount		= 0;
	};


}

