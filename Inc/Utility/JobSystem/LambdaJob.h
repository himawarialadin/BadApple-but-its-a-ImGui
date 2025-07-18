#pragma once
#include "JobInterface.h"
#include <functional>

namespace Utility
{
	/*!
	@brief ����ۂ̃W���u�B�O���[�v�������W���u�̍Ō�ɒǉ����邱�ƂŁA�W���u�̏I�������m�ł��܂��B
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
	�����_���ł̕���For�W���u
	���̃W���u��OnBegin�AOnEnd�͌Ă΂�܂���B
	*/
	struct LambdaParallelForJob : public IJobParallelFor
	{
	public:
		//! @brief ���s����֐�
		std::function<void(uint32_t)> execute;

	private:
		void Execute(uint32_t index) override
		{ execute(index); }

	};

	struct LambdaParallelForEXJob : public IJobParallelForEX
	{
	public:
		//! @brief ���s����֐�
		std::function<void(uint32_t, uint32_t)> execute;

	private:
		void Execute(uint32_t beginIndex, uint32_t endIndex) override
		{
			execute(beginIndex, endIndex);
		}
	};

}