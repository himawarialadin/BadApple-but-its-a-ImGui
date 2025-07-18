#pragma once
#include "Math.h"

namespace Math
{
	/*!
	@brief �O�p�g�𐶐����܂��B
	@param t				����
	@param period	����
	@param min		�ŏ��l
	@param max		�ő�l
	*/
	inline float TriangleWave(float t, float period, float min, float max)
	{
		// �O�p�g�� [0, period / 2] �͈̔͂Ő���
		float baseWave = std::fabsf(std::fmodf(t, period) - period / 2.0f);

		// �X�P�[�����O���� [min, max] �͈̔͂Ƀ}�b�s���O
		float amplitude = max - min;       // �g�̐U��
		return (baseWave / (period / 2.0f)) * amplitude + min;
	}

	/*!
	@brief ��`�g�𐶐����܂��B
	@param t				����
	@param period	����
	@param min		�ŏ��l
	@param max		�ő�l
	*/
	inline float SquareWave(float t, float period, float min, float max)
	{
		return std::sinf(t / period * 2.0f * Pi) >= 0.0f ? max : min;
	}

}