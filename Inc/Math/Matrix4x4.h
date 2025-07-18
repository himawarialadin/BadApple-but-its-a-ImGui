#pragma once
#pragma warning(push)
#pragma warning(disable:4324)

#include "MathFwd.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace Math
{

	/// <summary>
	/// DirectX::XMMATRIX �S�s�S�� float �s��Ŋg���N���X
	/// <para>���̃N���X��64�o�C�g����܂��B</para>
	/// </summary>
	class alignas(16) Matrix4x4
	{
	public:
	#pragma warning(push)
	#pragma warning(disable:4201)
		union
		{
			DirectX::XMMATRIX		_m;
			DirectX::XMVECTOR		_v[4];
			Vector4							_vector4[4];
			DirectX::XMFLOAT4X4A	_float4x4;

			struct
			{
				float _m00, _m01, _m02, _m03;
				float _m10, _m11, _m12, _m13;
				float _m20, _m21, _m22, _m23;
				float _m30, _m31, _m32, _m33;
			};
	
			float _array[4][4];
			float _oneArray[16];
		};
	#pragma warning(pop)
	
	public:
		Matrix4x4() : _vector4() {};
	
		constexpr Matrix4x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) : 
		_m00(m00), _m01(m01), _m02(m02), _m03(m03),
		_m10(m10), _m11(m11), _m12(m12), _m13(m13),
		_m20(m20), _m21(m21), _m22(m22), _m23(m23),
		_m30(m30), _m31(m31), _m32(m32), _m33(m33)
		{}
	
		explicit constexpr Matrix4x4(const DirectX::XMFLOAT4X4& m) :
			_float4x4(
				m._11, m._12, m._13, m._14,
				m._21, m._22, m._23, m._24,
				m._31, m._32, m._33, m._34,
				m._41, m._42, m._43, m._44) 
		{}

					constexpr	Matrix4x4(const DirectX::XMMATRIX& m)					: _m(m) {}
		explicit	constexpr	Matrix4x4(_In_reads_(4) DirectX::XMVECTOR* rows)	: _v{rows[0],rows[1],rows[2],rows[3]} {}
	
	public://Operator
		float					operator()(size_t row, size_t column) noexcept(false);
		Math::Vector4&	operator[](size_t row) { return _vector4[row]; }
	
	public://Cast
					operator DirectX::XMMATRIX()		const { return _m; }
					operator DirectX::XMFLOAT4X4()	const { return _float4x4; }
		explicit	operator DirectX::XMFLOAT4X4A()	const { return _float4x4; }
	
	public://Operatoer
		friend	Matrix4x4		__vectorcall operator+	(const Matrix4x4& left, const Matrix4x4& right);
		friend	Matrix4x4		__vectorcall operator-	(const Matrix4x4& left, const Matrix4x4& right);
		friend	Matrix4x4		__vectorcall operator*	(const Matrix4x4& left, const Matrix4x4& right);
		friend	Matrix4x4		__vectorcall operator*	(const Matrix4x4& left, float scaler);
		friend	Matrix4x4		__vectorcall operator/	(const Matrix4x4& left, float scaler);
	
		Matrix4x4&	__vectorcall operator+=(const Matrix4x4& right)		& { return *this = *this + right;		}
		Matrix4x4&	__vectorcall operator-=	(const Matrix4x4& right)		& { return *this = *this - right;		}
		Matrix4x4&	__vectorcall operator*=	(const Matrix4x4& right)		& { return *this = *this * right;		}
		Matrix4x4&	__vectorcall operator*=	(float scaler)						& { return *this = *this * scaler;	}
		Matrix4x4&	__vectorcall operator/=	(float scaler)						& { return *this = *this / scaler;		}
		Matrix4x4		__vectorcall operator+()											{ return *this;}
		Matrix4x4		__vectorcall operator-();
	
	public://Functions
		
		/// <summary>
		/// �t�s������߂�
		/// </summary>
		/// <returns></returns>
		Matrix4x4 __vectorcall	Inverse()	const
		{
			return DirectX::XMMatrixInverse(nullptr, _m);
		}

		/// <summary>
		/// �]�u�s������߂�
		/// </summary>
		Matrix4x4 __vectorcall	Transpose() const
		{
			return DirectX::XMMatrixTranspose(_m);
		}

		//Vector4	__vectorcall GetCoulmn(int /*index*/) const
		//{
		//	throw std::exception("������");
		//}

		Vector4	__vectorcall GetRow		(int index) const
		{
			return _vector4[index];
		}

	public: //Transform Matrix

		/// <summary>
		/// �g�����X�t�H�[���s��Ɖ��肵�A�X�P�[���A��]�A�ʒu���擾���܂��B
		/// </summary>
		void __vectorcall Decompose(_Out_ Math::Vector3& scale, _Out_  Math::Quaternion& rotation, _Out_ Math::Vector3& translation) const
		{
			DirectX::XMMatrixDecompose(&scale._v, &rotation._v, &translation._v, _m);
		}

		/// <summary>
		/// �g�����X�t�H�[���s��Ɖ��肵�A�E�����x�N�g�����擾���܂��B
		/// </summary>
		Vector3 GetRight() const
		{
			return Vector3(_m.r[0]).Normalize();
		}
		Vector3 GetRight_Unnormal() const
		{
			return Vector3(_m.r[0]);
		}

		/// <summary>
		/// �g�����X�t�H�[���s��Ɖ��肵�A������x�N�g�����擾���܂��B
		/// </summary>
		Vector3 GetUp() const
		{
			return Vector3(_m.r[1]).Normalize();
		}
		Vector3 GetUp_Unnormal() const
		{
			return Vector3(_m.r[1]);
		}

		/// <summary>
		/// �g�����X�t�H�[���s��Ɖ��肵�A������x�N�g�����擾���܂��B
		/// </summary>
		Vector3 GetForward() const
		{
			return Vector3(_m.r[2]).Normalize();
		}
		Vector3 GetForward_Unnormal() const
		{
			return Vector3(_m.r[2]);
		}

		/// <summary>
		/// �g�����X�t�H�[���s��Ɖ��肵�A���s�ړ��x�N�g�����擾���܂��B
		/// </summary>
		Vector3 GetTranslation() const
		{
			return Vector3(_m.r[3]);
		}

	public:
		/// <summary>
		/// �X�P�[���s��~��]�s��~���s�ړ��s��̃��[���h�s����쐬
		/// </summary>
		/// <param name="scale">�X�P�[��</param>
		/// <param name="rotation">��]�iyaw pitch row�̏��A�S�ă��W�A���p�x�j</param>
		/// <param name="transrate">���s�ړ���</param>
		static Matrix4x4 __vectorcall SetSRT(Vector3 const& scale, Vector3 const& rotation /*radian vector*/, Vector3 const& transrate);

		/// <summary>
		/// �X�P�[���s��~��]�s��~���s�ړ��s��̃��[���h�s����쐬
		/// </summary>
		/// <param name="scale">�X�P�[��</param>
		/// <param name="rotation">��]�N�H�[�^�j�I��</param>
		/// <param name="transrate">���s�ړ���</param>
		static Matrix4x4 __vectorcall SetSRT(Vector3 const& scale, Quaternion const& rotation, Vector3 const& transrate);

		/// <summary>
		/// �r���[�s����쐬
		/// eyePosition��focusPosition�������l�̏ꍇ�̓G���[���������܂��B
		/// </summary>
		/// <param name="eyePosition">���_���W</param>
		/// <param name="focusPosition">�t�H�[�J�X�ʒu���W</param>
		/// <param name="upDirection">��x�N�g��</param>
		static Matrix4x4 __vectorcall LookAt			(Vector3 const& eyePosition, Vector3 const& focusPosition, Vector3 const& upDirection);

		/// <summary>
		/// �r���[�s����쐬(���_�A�����̌������w�肷��o�[�W����)
		/// </summary>
		/// <param name="eyePosition">���_���W</param>
		/// <param name="focusPosition">�����̌���</param>
		/// <param name="upDirection">��x�N�g��</param>
		static Matrix4x4 __vectorcall LookTo			(Vector3 const& eyePosition, Vector3 const& eyeDirection, Vector3 const& upDirection);

		/// <summary>
		/// ���s�ˉe�s����쐬
		/// �����ꂩ�̒l���O�̏ꍇ�̓G���[���������܂��B
		/// </summary>
		/// <param name="viewWidth">�r���[����</param>
		/// <param name="viewHeight">�r���[�c��</param>
		/// <param name="nearZ">�j�A�[�x</param>
		/// <param name="farZ">�t�@�[�[�x</param>
		static Matrix4x4 __vectorcall Orthographic	(float viewWidth, float viewHeight, float nearZ, float farZ);

		/// <summary>
		/// 2D�A�t�B���ϊ��s����쐬���܂��B
		/// </summary>
		/// <param name="scale">�X�P�[�����O�W����\��2�����x�N�g���B</param>
		/// <param name="rotationOrigin">��]�̒��S�ƂȂ�2�����x�N�g���B</param>
		/// <param name="rotateAngleScaler">��]�p�x�i���W�A���P�ʁj�B</param>
		/// <param name="translation">���s�ړ���\��2�����x�N�g���B</param>
		/// <returns>�w�肳�ꂽ�X�P�[���A��]�A���s�ړ���K�p����2D�A�t�B���ϊ���4x4�s��B</returns>
		static Matrix4x4 __vectorcall AffineTransformation2D(Vector2 const& scale, Vector2 const& rotationOrigin, float rotateAngleScaler, Vector2 const& translation);

		/// <summary>
		/// �ˉe�s����쐬
		/// </summary>
		/// <param name="fov">����p(�x���@)</param>
		/// <param name="aspect">�A�X�y�N�g��</param>
		/// <param name="nearZ">�j�A�[�x</param>
		/// <param name="farZ">�t�@�[�[�x</param>
		static Matrix4x4 __vectorcall Parspective		(float fov, float aspect, float nearZ, float farZ);

		/// <summary>
		/// �X�P�[���s����쐬
		/// </summary>
		static Matrix4x4 __vectorcall Scale				(Vector3		const& scale);

		/// <summary>
		/// �ړ��s����쐬
		/// </summary>
		static Matrix4x4 __vectorcall Translate		(Vector3		const& translate);

		/// <summary>
		/// ��]�s����쐬
		/// </summary>
		static Matrix4x4 __vectorcall Rotation		(Vector3		const& angle);

		/// <summary>
		/// ��]�s����쐬
		/// </summary>
		static Matrix4x4 __vectorcall Rotation		(Quaternion	const& rotation);
		
	public://Constants
		static constinit const Matrix4x4 Zero;
		static constinit const Matrix4x4 Identity;
	};
}

#pragma warning(pop)