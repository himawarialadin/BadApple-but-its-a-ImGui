#pragma once
#include "MathFwd.h"
#include "Vector3.h"

namespace Math
{
	/// <summary>
	/// DirectX::XMVECTOR �N�H�[�^�j�I���Ŋg���N���X
	/// </summary>
	__declspec(align(16)) class Quaternion final
	{
	public:
	#pragma warning(push)
	#pragma warning(disable:4201)
		union
		{
			DirectX::XMVECTOR	_v;
			DirectX::XMFLOAT4A _float;
			struct { float x, y, z, w; };
			mutable float _array[4];
		};
	#pragma warning(pop)
	
	public://Constructor
					constexpr	Quaternion()																: x(0), y(0), z(0), w(0)																	{}
					constexpr	Quaternion(float x, float y, float z, float w)					: x(x), y(y), z(z), w(w)																	{}
		explicit	constexpr	Quaternion(_In_reads_(4) float* array)						: _array{ array[0],array[1],array[2],array[3]}									{}
		explicit					Quaternion(const DirectX::XMVECTOR&	quaternion)	: _v(quaternion)																				{}
		explicit					Quaternion(const DirectX::XMFLOAT4&	quaternion)	: x(quaternion.x),y(quaternion.y),z(quaternion.z),w(quaternion.w)	{}
	
	public://Casts
		explicit	__vectorcall operator DirectX::XMVECTOR()	const { return _v;			}
		explicit	__vectorcall operator DirectX::XMFLOAT4()		const { return _float;		}
		explicit	__vectorcall operator float*()							const { return _array;	}

		//�A���C�����g����New
		void* operator new(size_t size);
		//�A���C�����g����New[]
		void* operator new[](size_t size);

	public:
		friend Quaternion	__vectorcall operator*		(const Quaternion& Q1,	const Quaternion& Q2);
		friend Vector3		__vectorcall operator*		(const Quaternion& Q,	const Vector3& vector);
		friend bool				__vectorcall operator==	(const Quaternion& Q1,	const Quaternion& Q2);

		float&					__vectorcall operator[]	(size_t index) &				noexcept(false);
		float						__vectorcall operator[]	(size_t index) const&&	noexcept(false);

	public:
		//��]�s�񂩂�N�H�[�^�j�I���𐶐�
		static Quaternion	__vectorcall FromRotationMatrix(const Matrix4x4& rotation);
		
		//�I�C���[�p(�x���@)����N�H�[�^�j�I����
		static Quaternion	__vectorcall Euler				(float x, float y, float z);
	
		//�I�C���[�p(�x���@)����N�H�[�^�j�I����(�p�x�w�肪�x�N�g���o�[�W����)
		static Quaternion	__vectorcall Euler				(const Vector3& eulerAngle);
	
		//�t�N�H�[�^�j�I��
		static Quaternion	__vectorcall Inverse			(const Quaternion& Q);
	
		/// <summary>
		/// �w�肳�ꂽ�O�������Ɍ����N�H�[�^�j�I�������߂܂��B
		/// </summary>
		/// <param name="forward">�O���x�N�g��</param>
		/// <param name="upward">��x�N�g��(Y���x�N�g��)</param>
		/// <remarks>
		/// Y���x�N�g�����`����Ӗ��́A��̃x�N�g�����`���ď��߂ĉ�]������邱�Ƃ��ł��邩��ł��B
		/// ��]�������߂������ŁA�N�H�[�^�j�I�����쐬���܂��B
		/// </remarks>
		static Quaternion	__vectorcall LookRotation(const Vector3& forward, const Vector3& upward = { 0, 1.0f, 0 });
	
		/// <summary>
		/// from����to�Ɍ����N�H�[�^�j�I�������߂܂��B
		/// �Q�̃x�N�g���͐��K������Ă���K�v������܂��B
		/// </summary>
		static Quaternion	__vectorcall FromToRotation(const Vector3& from, const Vector3& to);

		//�N�H�[�^�j�I�����ʐ��`���
		static Quaternion	__vectorcall Slerp				(const Quaternion& start, const Quaternion& end, float t);
	
		//���Ɗp�x����N�H�[�^�j�I�����쐬
		static Quaternion	__vectorcall AngleAxis		(const Vector3& axis, float angle);

		//��̉�]�̓���
		static float				__vectorcall Dot				(const Quaternion& Q1, const Quaternion& Q2);

	public:
		//�N�H�[�^�j�I�����玲�Ɗp�x�𒊏o
		void						__vectorcall GetAxisAngle	(_Out_ Vector3& outAxis, _Out_ float& outAngle) const;

		//�N�H�[�^�j�I������I�C���[�p�ɕϊ�
		Vector3					ToEuler()		const;

	public://Constants
		static constinit const Quaternion Identity;

	};
}