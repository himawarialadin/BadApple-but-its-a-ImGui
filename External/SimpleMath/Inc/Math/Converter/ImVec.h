#pragma once
/*
* ImGui <-> SimpleMath Converter
* 使用するにはこのファイルをインクルードする前に、ImGuiのヘッダーファイルをインクルードしてください。
*/

#ifdef IMGUI_VERSION

#include "ConverterFunc.h"
#include "../Vector2.h"
#include "../Vector2Int.h"
#include "../Color.h"

namespace Math
{
	template<>
	inline ImVec2 Cast(Vector2 const& v)
	{
		return ImVec2(v.x, v.y);
	}

	template<>
	inline Vector2 Cast(ImVec2 const& v)
	{
		return Vector2(v.x, v.y);
	}

	template<>
	inline Vector2Int Cast(ImVec2 const& v)
	{
		return Vector2Int(static_cast<Vector2Int::value_type>(v.x), static_cast<Vector2Int::value_type>(v.y));
	}

	template<>
	inline ImVec2 Cast(Vector2Int const& v)
	{
		return ImVec2(static_cast<float>(v.x), static_cast<float>(v.y));
	}

	template<>
	inline ImVec4 Cast(Color const& c)
	{
		return ImVec4(c.R, c.G, c.B, c.A);
	}

	template<>
	inline Color Cast(ImVec4 const& c)
	{
		return Color(c.x, c.y, c.z, c.w);
	}
}

#endif