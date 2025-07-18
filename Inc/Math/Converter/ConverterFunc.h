#pragma once
/*!
* @brief
* �e���C�u�����̃x�N�g���E�s��^��SimpleMath�̌^�ɕϊ����邽�߂̊֐��e���v���[�g
* �Ǝ��Ŏ��������^��SimpleMath�̌^�ɕϊ�����ꍇ�́A�����̊֐��e���v���[�g���I�[�o�[���[�h���Ă��������B
* 
* @code{.cpp}
* //��: �Ǝ���Vector2Int�^��SimpleMath��Vector2Int�^�ɕϊ�����֐��e���v���[�g
* template<>
* Math::Vector2Int Cast(YourVector2IntType const& v)
* {
* 	return Math::Vector2Int(static_cast<Math::Vector2Int::value_type>(v.x), static_cast<Math::Vector2Int::value_type>(v.y));
* }
* 
* //��: �x�N�g���v�f�����قȂ�ꍇ�A�ǉ��������w�肵�ĕϊ����邱�Ƃ��ł��܂��B(�e���v���[�g�̎d�l��A�ǉ������Ƀf�t�H���g�l���w�肷�邱�Ƃ͂ł��܂���B)
* template<>
* Math::Vector3 Cast(YourVector2Type const& v, float z)
* {
* 	return Math::Vector3(v.x, v.y, z);
* }
* 
*/

namespace Math
{
	template<typename Dest, typename Src>
	Dest Cast(Src const&) = delete;

	template<typename Dest, typename Src>
	Dest Cast(Src const&, auto) = delete;

	template<typename Dest, typename Src>
	Dest Cast(Src const&, auto, auto) = delete;

}
