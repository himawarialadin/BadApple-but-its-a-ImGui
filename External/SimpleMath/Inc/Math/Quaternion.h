#pragma once
#include "MathFwd.h"
#include "Vector3.h"

namespace Math
{
	/// <summary>
	/// DirectX::XMVECTOR クォータニオン版拡張クラス
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

		//アライメント強制New
		void* operator new(size_t size);
		//アライメント強制New[]
		void* operator new[](size_t size);

	public:
		friend Quaternion	__vectorcall operator*		(const Quaternion& Q1,	const Quaternion& Q2);
		friend Vector3		__vectorcall operator*		(const Quaternion& Q,	const Vector3& vector);
		friend bool				__vectorcall operator==	(const Quaternion& Q1,	const Quaternion& Q2);

		float&					__vectorcall operator[]	(size_t index) &				noexcept(false);
		float						__vectorcall operator[]	(size_t index) const&&	noexcept(false);

	public:
		//回転行列からクォータニオンを生成
		static Quaternion	__vectorcall FromRotationMatrix(const Matrix4x4& rotation);
		
		//オイラー角(度数法)からクォータニオンに
		static Quaternion	__vectorcall Euler				(float x, float y, float z);
	
		//オイラー角(度数法)からクォータニオンに(角度指定がベクトルバージョン)
		static Quaternion	__vectorcall Euler				(const Vector3& eulerAngle);
	
		//逆クォータニオン
		static Quaternion	__vectorcall Inverse			(const Quaternion& Q);
	
		/// <summary>
		/// 指定された前方方向に向くクォータニオンを求めます。
		/// </summary>
		/// <param name="forward">前方ベクトル</param>
		/// <param name="upward">上ベクトル(Y軸ベクトル)</param>
		/// <remarks>
		/// Y軸ベクトルを定義する意味は、二つのベクトルを定義して初めて回転軸を作ることができるからです。
		/// 回転軸を求めたうえで、クォータニオンを作成します。
		/// </remarks>
		static Quaternion	__vectorcall LookRotation(const Vector3& forward, const Vector3& upward = { 0, 1.0f, 0 });
	
		/// <summary>
		/// fromからtoに向くクォータニオンを求めます。
		/// ２つのベクトルは正規化されている必要があります。
		/// </summary>
		static Quaternion	__vectorcall FromToRotation(const Vector3& from, const Vector3& to);

		//クォータニオン球面線形補間
		static Quaternion	__vectorcall Slerp				(const Quaternion& start, const Quaternion& end, float t);
	
		//軸と角度からクォータニオンを作成
		static Quaternion	__vectorcall AngleAxis		(const Vector3& axis, float angle);

		//二つの回転の内積
		static float				__vectorcall Dot				(const Quaternion& Q1, const Quaternion& Q2);

	public:
		//クォータニオンから軸と角度を抽出
		void						__vectorcall GetAxisAngle	(_Out_ Vector3& outAxis, _Out_ float& outAngle) const;

		//クォータニオンからオイラー角に変換
		Vector3					ToEuler()		const;

	public://Constants
		static constinit const Quaternion Identity;

	};
}