#pragma once
/*
* FMOD_VECTOR <-> SimpleMath Converter
* 使用するにはこのファイルをインクルードする前に、FMODのヘッダーファイルをインクルードしてください。
*/

#ifdef FMOD_VERSION

#include "ConverterFunc.h"
#include "../Vector3.h"

namespace Math
{
	template<>
	inline Vector3 Cast(FMOD_VECTOR const& v)
	{
		return Vector3(v.x, v.y, v.z);
	}

	template<>
	inline FMOD_VECTOR Cast(Vector3 const& v)
	{
		return FMOD_VECTOR{ v.x, v.y, v.z };
	}
}

#endif