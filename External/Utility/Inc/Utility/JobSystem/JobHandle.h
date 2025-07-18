#pragma once
#include <latch>
#include <memory>

namespace Utility
{
	/*!
	@brief ジョブの完了を待つためのハンドル
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
		ジョブが空かどうかを取得します。
		無効なハンドルとは異なります。
		*/
		bool IsEmpty()		const { return m_latch == nullptr; }

		/*!
		@brief ジョブが完了したかどうかを取得します。
		*/
		bool IsComplate()	const { return m_latch->try_wait(); }

		/*!
		@brief ジョブが完了するまで待機します。
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