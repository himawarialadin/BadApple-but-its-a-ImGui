#pragma once
#include <cstdint>

namespace Utility
{
	/*!
	@brief 別スレッドで実行するジョブのインターフェース
	*/
	struct IJob
	{
		virtual ~IJob() = default;

		/*!
		@brief ジョブの開始時に呼ばれる関数
		*/
		virtual void OnBegin() {};

		/*!
		@brief ジョブの実行時に呼ばれる関数
		*/
		virtual void Execute() = 0;

		/*!
		@brief ジョブの終了時に呼ばれる関数
		*/
		virtual void OnEnd() {};
	};

	/*!
	@brief 多くの要素を並列で処理するジョブのインターフェース
	*/
	struct IJobParallelFor
	{
		virtual ~IJobParallelFor() = default;

		/*!
		@brief 
		ジョブの開始時に呼ばれる関数。並列単位ごとに呼ばれます。
		たとえば、100個の要素を10個ごとに処理する場合、10回呼ばれます。
		*/
		virtual void OnBegin() {};

		/*!
		@brief 並列処理時に要素ごとに呼ばれる関数。
		@param index 処理する要素のインデックス
		*/
		virtual void Execute(uint32_t index) = 0;

		/*!
		@brief
		ジョブの終了時に呼ばれる関数。並列単位ごとに呼ばれます。
		たとえば、100個の要素を10個ごとに処理する場合、10回呼ばれます。
		*/
		virtual void OnEnd() {};
	};

	/*!
	@brief 多くの要素を並列で処理するジョブのインターフェース。
	IJobParallelForと違い、呼ばれる関数がインデックスの範囲になっています。
	*/
	struct IJobParallelForEX
	{
		virtual ~IJobParallelForEX() = default;

		/*!
		@brief この並列処理が開始される前に呼ばれる関数
		*/
		virtual void OnBegin() {};


		/*!
		@brief この並列処理が終了した後に呼ばれる関数
		*/
		virtual void OnEnd() {};

		/*!
		@brief 並列処理時に要素ごとに呼ばれる関数。IJobParallelForと違い、for文を自前で回す必要があります。
		@param beginIndex 処理する要素の開始インデックス
		@param endIndex 処理する要素の終了インデックス
		*/
		virtual void Execute(uint32_t beginIndex, uint32_t endIndex) = 0;
	};

}