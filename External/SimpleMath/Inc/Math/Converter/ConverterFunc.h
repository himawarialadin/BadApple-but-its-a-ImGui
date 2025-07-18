#pragma once
/*!
* @brief
* 各ライブラリのベクトル・行列型をSimpleMathの型に変換するための関数テンプレート
* 独自で実装した型をSimpleMathの型に変換する場合は、これらの関数テンプレートをオーバーロードしてください。
* 
* @code{.cpp}
* //例: 独自のVector2Int型をSimpleMathのVector2Int型に変換する関数テンプレート
* template<>
* Math::Vector2Int Cast(YourVector2IntType const& v)
* {
* 	return Math::Vector2Int(static_cast<Math::Vector2Int::value_type>(v.x), static_cast<Math::Vector2Int::value_type>(v.y));
* }
* 
* //例: ベクトル要素数が異なる場合、追加引数を指定して変換することもできます。(テンプレートの仕様上、追加引数にデフォルト値を指定することはできません。)
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
