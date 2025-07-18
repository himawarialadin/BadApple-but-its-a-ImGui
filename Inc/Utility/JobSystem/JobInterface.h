#pragma once
#include <cstdint>

namespace Utility
{
	/*!
	@brief �ʃX���b�h�Ŏ��s����W���u�̃C���^�[�t�F�[�X
	*/
	struct IJob
	{
		virtual ~IJob() = default;

		/*!
		@brief �W���u�̊J�n���ɌĂ΂��֐�
		*/
		virtual void OnBegin() {};

		/*!
		@brief �W���u�̎��s���ɌĂ΂��֐�
		*/
		virtual void Execute() = 0;

		/*!
		@brief �W���u�̏I�����ɌĂ΂��֐�
		*/
		virtual void OnEnd() {};
	};

	/*!
	@brief �����̗v�f�����ŏ�������W���u�̃C���^�[�t�F�[�X
	*/
	struct IJobParallelFor
	{
		virtual ~IJobParallelFor() = default;

		/*!
		@brief 
		�W���u�̊J�n���ɌĂ΂��֐��B����P�ʂ��ƂɌĂ΂�܂��B
		���Ƃ��΁A100�̗v�f��10���Ƃɏ�������ꍇ�A10��Ă΂�܂��B
		*/
		virtual void OnBegin() {};

		/*!
		@brief ���񏈗����ɗv�f���ƂɌĂ΂��֐��B
		@param index ��������v�f�̃C���f�b�N�X
		*/
		virtual void Execute(uint32_t index) = 0;

		/*!
		@brief
		�W���u�̏I�����ɌĂ΂��֐��B����P�ʂ��ƂɌĂ΂�܂��B
		���Ƃ��΁A100�̗v�f��10���Ƃɏ�������ꍇ�A10��Ă΂�܂��B
		*/
		virtual void OnEnd() {};
	};

	/*!
	@brief �����̗v�f�����ŏ�������W���u�̃C���^�[�t�F�[�X�B
	IJobParallelFor�ƈႢ�A�Ă΂��֐����C���f�b�N�X�͈̔͂ɂȂ��Ă��܂��B
	*/
	struct IJobParallelForEX
	{
		virtual ~IJobParallelForEX() = default;

		/*!
		@brief ���̕��񏈗����J�n�����O�ɌĂ΂��֐�
		*/
		virtual void OnBegin() {};


		/*!
		@brief ���̕��񏈗����I��������ɌĂ΂��֐�
		*/
		virtual void OnEnd() {};

		/*!
		@brief ���񏈗����ɗv�f���ƂɌĂ΂��֐��BIJobParallelFor�ƈႢ�Afor�������O�ŉ񂷕K�v������܂��B
		@param beginIndex ��������v�f�̊J�n�C���f�b�N�X
		@param endIndex ��������v�f�̏I���C���f�b�N�X
		*/
		virtual void Execute(uint32_t beginIndex, uint32_t endIndex) = 0;
	};

}