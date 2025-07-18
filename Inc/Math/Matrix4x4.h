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
	/// DirectX::XMMATRIX ４行４列 float 行列版拡張クラス
	/// <para>このクラスは64バイトあります。</para>
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
		/// 逆行列を求める
		/// </summary>
		/// <returns></returns>
		Matrix4x4 __vectorcall	Inverse()	const
		{
			return DirectX::XMMatrixInverse(nullptr, _m);
		}

		/// <summary>
		/// 転置行列を求める
		/// </summary>
		Matrix4x4 __vectorcall	Transpose() const
		{
			return DirectX::XMMatrixTranspose(_m);
		}

		//Vector4	__vectorcall GetCoulmn(int /*index*/) const
		//{
		//	throw std::exception("未実装");
		//}

		Vector4	__vectorcall GetRow		(int index) const
		{
			return _vector4[index];
		}

	public: //Transform Matrix

		/// <summary>
		/// トランスフォーム行列と仮定し、スケール、回転、位置を取得します。
		/// </summary>
		void __vectorcall Decompose(_Out_ Math::Vector3& scale, _Out_  Math::Quaternion& rotation, _Out_ Math::Vector3& translation) const
		{
			DirectX::XMMatrixDecompose(&scale._v, &rotation._v, &translation._v, _m);
		}

		/// <summary>
		/// トランスフォーム行列と仮定し、右方向ベクトルを取得します。
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
		/// トランスフォーム行列と仮定し、上方向ベクトルを取得します。
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
		/// トランスフォーム行列と仮定し、上方向ベクトルを取得します。
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
		/// トランスフォーム行列と仮定し、平行移動ベクトルを取得します。
		/// </summary>
		Vector3 GetTranslation() const
		{
			return Vector3(_m.r[3]);
		}

	public:
		/// <summary>
		/// スケール行列×回転行列×平行移動行列のワールド行列を作成
		/// </summary>
		/// <param name="scale">スケール</param>
		/// <param name="rotation">回転（yaw pitch rowの順、全てラジアン角度）</param>
		/// <param name="transrate">平行移動量</param>
		static Matrix4x4 __vectorcall SetSRT(Vector3 const& scale, Vector3 const& rotation /*radian vector*/, Vector3 const& transrate);

		/// <summary>
		/// スケール行列×回転行列×平行移動行列のワールド行列を作成
		/// </summary>
		/// <param name="scale">スケール</param>
		/// <param name="rotation">回転クォータニオン</param>
		/// <param name="transrate">平行移動量</param>
		static Matrix4x4 __vectorcall SetSRT(Vector3 const& scale, Quaternion const& rotation, Vector3 const& transrate);

		/// <summary>
		/// ビュー行列を作成
		/// eyePositionとfocusPositionが同じ値の場合はエラーが発生します。
		/// </summary>
		/// <param name="eyePosition">視点座標</param>
		/// <param name="focusPosition">フォーカス位置座標</param>
		/// <param name="upDirection">上ベクトル</param>
		static Matrix4x4 __vectorcall LookAt			(Vector3 const& eyePosition, Vector3 const& focusPosition, Vector3 const& upDirection);

		/// <summary>
		/// ビュー行列を作成(視点、視線の向きを指定するバージョン)
		/// </summary>
		/// <param name="eyePosition">視点座標</param>
		/// <param name="focusPosition">視線の向き</param>
		/// <param name="upDirection">上ベクトル</param>
		static Matrix4x4 __vectorcall LookTo			(Vector3 const& eyePosition, Vector3 const& eyeDirection, Vector3 const& upDirection);

		/// <summary>
		/// 平行射影行列を作成
		/// いずれかの値が０の場合はエラーが発生します。
		/// </summary>
		/// <param name="viewWidth">ビュー横幅</param>
		/// <param name="viewHeight">ビュー縦幅</param>
		/// <param name="nearZ">ニア深度</param>
		/// <param name="farZ">ファー深度</param>
		static Matrix4x4 __vectorcall Orthographic	(float viewWidth, float viewHeight, float nearZ, float farZ);

		/// <summary>
		/// 2Dアフィン変換行列を作成します。
		/// </summary>
		/// <param name="scale">スケーリング係数を表す2次元ベクトル。</param>
		/// <param name="rotationOrigin">回転の中心となる2次元ベクトル。</param>
		/// <param name="rotateAngleScaler">回転角度（ラジアン単位）。</param>
		/// <param name="translation">平行移動を表す2次元ベクトル。</param>
		/// <returns>指定されたスケール、回転、平行移動を適用した2Dアフィン変換の4x4行列。</returns>
		static Matrix4x4 __vectorcall AffineTransformation2D(Vector2 const& scale, Vector2 const& rotationOrigin, float rotateAngleScaler, Vector2 const& translation);

		/// <summary>
		/// 射影行列を作成
		/// </summary>
		/// <param name="fov">視野角(度数法)</param>
		/// <param name="aspect">アスペクト比</param>
		/// <param name="nearZ">ニア深度</param>
		/// <param name="farZ">ファー深度</param>
		static Matrix4x4 __vectorcall Parspective		(float fov, float aspect, float nearZ, float farZ);

		/// <summary>
		/// スケール行列を作成
		/// </summary>
		static Matrix4x4 __vectorcall Scale				(Vector3		const& scale);

		/// <summary>
		/// 移動行列を作成
		/// </summary>
		static Matrix4x4 __vectorcall Translate		(Vector3		const& translate);

		/// <summary>
		/// 回転行列を作成
		/// </summary>
		static Matrix4x4 __vectorcall Rotation		(Vector3		const& angle);

		/// <summary>
		/// 回転行列を作成
		/// </summary>
		static Matrix4x4 __vectorcall Rotation		(Quaternion	const& rotation);
		
	public://Constants
		static constinit const Matrix4x4 Zero;
		static constinit const Matrix4x4 Identity;
	};
}

#pragma warning(pop)