#pragma once
#include <latch>
#include <memory>

namespace Utility
{
	/*!
	@brief �W���u�̊�����҂��߂̃n���h��
	*/
	class JobHandle final
	{
	public:
		JobHandle()		= default;
		JobHandle(std::shared_ptr<std::latch> const& latch) : m_latch(latch) {}
		~JobHandle()		= default;

	public:

		/*!
		@brief 
		�W���u���󂩂ǂ������擾���܂��B
		�����ȃn���h���Ƃ͈قȂ�܂��B
		*/
		bool IsEmpty()		const { return m_latch == nullptr; }

		/*!
		@brief �W���u�������������ǂ������擾���܂��B
		*/
		bool IsComplate()	const { return m_latch->try_wait(); }

		/*!
		@brief �W���u����������܂őҋ@���܂��B
		*/
		void Complate()		const 
		{
			if (m_latch == nullptr) return;
			m_latch->wait();
		}

	private:
		std::shared_ptr<std::latch> m_latch;
	};
}