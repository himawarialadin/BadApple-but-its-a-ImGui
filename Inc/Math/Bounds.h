#pragma once
#pragma push_macro("min")
#pragma push_macro("max")

#undef min
#undef max

#include <DirectXMath.h>
#include "Vector3.h"
#include "Vector2.h"

#pragma warning(push)
#pragma warning(disable:4201) //C4201: 非標準の拡張機能が使用されています: 無名の構造体または共用体です。
#pragma warning(disable:4324) //C4324: アラインメント指定子のために構造体がパッドされました


namespace Math
{
	//3次元の境界ボックスを表すクラスです。
	class alignas(DirectX::XMVECTOR) Bounds
	{
	public:
		union
		{
			struct alignas(DirectX::XMVECTOR)
			{
				DirectX::XMVECTOR _vMin;
				DirectX::XMVECTOR _vMax;
			};

			//struct alignas(DirectX::XMVECTOR)
			//{
			//	DirectX::XMFLOAT3A _floatMin;
			//	DirectX::XMFLOAT3A _floatMax;
			//};

			struct alignas(DirectX::XMVECTOR)
			{
				Vector3 min;		//ボックスの最小値
				Vector3 max;		//ボックスの最大値
			};

			struct alignas(DirectX::XMVECTOR)
			{
				float left,		top,			back,			_padding;
				float right,		bottom,		front,		_padding2;
			};
		};

	public:
		constexpr Bounds() : _vMin(), _vMax()		{}
		constexpr Bounds(Vector3 const& _begin, Vector3 const& _end) : min(_begin), max(_end)	{}
		//constexpr Bounds(DirectX::XMFLOAT3A const& _begin, DirectX::XMFLOAT3A const& _end) : _floatMin(_begin), _floatMax(_end) {}
		constexpr Bounds(DirectX::XMVECTOR const& _begin, DirectX::XMVECTOR const& _end) : _vMin(_begin), _vMax(_end) {}
		constexpr Bounds(float _left, float _bottom, float _back, float _right, float _top, float _front) : left(_left), bottom(_bottom), back(_back), right(_right), top(_top), front(_front) {}

	public:
		static Bounds __vectorcall SetElements		(float left, float bottom, float back, float right, float top, float front) { return Bounds(left, bottom, back, right, top, front); }
		static Bounds __vectorcall SetMinMax		(Vector3 const& min, Vector3 const& max) { return Bounds(min, max); }
		static Bounds __vectorcall SetMinMax		(Vector2 const& min, Vector2 const& max) { return Bounds(min._v, max._v); }

		static Bounds __vectorcall SetCenterSize	(Vector3 const& center, Vector3 const& size) { return Bounds(center - size / 2, center + size / 2); }
		
	public:
		Vector3 __vectorcall Center()	const { return (min + max) / 2; }
		Vector3 __vectorcall Size()		const { return max - min; }

		void __vectorcall SetSize(Vector3 const& size) { max = min + size; }
		void __vectorcall SetCenter(Vector3 const& center) { auto size = Size(); min = center - size / 2; max = center + size / 2; }
	};
}

#pragma warning(pop)
#pragma pop_macro("min")
#pragma pop_macro("max")