#pragma once
#include "Math.h"

namespace Math
{
	/*!
	@brief 三角波を生成します。
	@param t				時間
	@param period	周期
	@param min		最小値
	@param max		最大値
	*/
	inline float TriangleWave(float t, float period, float min, float max)
	{
		// 三角波を [0, period / 2] の範囲で生成
		float baseWave = std::fabsf(std::fmodf(t, period) - period / 2.0f);

		// スケーリングして [min, max] の範囲にマッピング
		float amplitude = max - min;       // 波の振幅
		return (baseWave / (period / 2.0f)) * amplitude + min;
	}

	/*!
	@brief 矩形波を生成します。
	@param t				時間
	@param period	周期
	@param min		最小値
	@param max		最大値
	*/
	inline float SquareWave(float t, float period, float min, float max)
	{
		return std::sinf(t / period * 2.0f * Pi) >= 0.0f ? max : min;
	}

}