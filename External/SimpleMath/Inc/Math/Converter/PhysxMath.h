#pragma once
/*
* PhysX <-> SimpleMath Converter
* 使用するにはこのファイルをインクルードする前に、PhysXのヘッダーファイルをインクルードしてください。
*/

#ifdef PHYSX_VERSION

#include "ConverterFunc.h"
#include "../Vector2.h"
#include "../Vector3.h"
#include "../Vector4.h"
#include "../Quaternion.h"
#include "../Matrix4x4.h"

namespace Math
{
	template<>
	inline Vector2 Cast(physx::PxVec2 const& v)
	{
		return Vector2(v.x, v.y);
	}

	template<>
	inline Vector3 Cast(physx::PxVec3 const& v)
	{
		return Vector3(v.x, v.y, v.z);
	}

	template<>
	inline Vector4 Cast(physx::PxVec4 const& v)
	{
		return Vector4(v.x, v.y, v.z, v.w);
	}

	template<>
	inline Quaternion Cast(physx::PxQuat const& v)
	{
		return Quaternion(v.x, v.y, v.z, v.w);
	}

	template<>
	inline Matrix4x4 Cast(physx::PxMat44 const& v)
	{
		return Matrix4x4(
			v.column0.x, v.column0.y, v.column0.z, v.column0.w,
			v.column1.x, v.column1.y, v.column1.z, v.column1.w,
			v.column2.x, v.column2.y, v.column2.z, v.column2.w,
			v.column3.x, v.column3.y, v.column3.z, v.column3.w
		);

	}

	//--------------------------------------------------------------//
	template<>
	inline physx::PxVec2 Cast(Vector2 const& v)
	{
		return physx::PxVec2(v.x, v.y);
	}

	template<>
	inline physx::PxVec3 Cast(Vector3 const& v)
	{
		return physx::PxVec3(v.x, v.y, v.z);
	}

	template<>
	inline physx::PxVec4 Cast(Vector4 const& v)
	{
		return physx::PxVec4(v.x, v.y, v.z, v.w);
	}
	
	template<>
	inline physx::PxQuat Cast(Quaternion const& v)
	{
		return physx::PxQuat(v.x, v.y, v.z, v.w);
	}

	template<>
	inline physx::PxMat44 Cast(Matrix4x4 const& v)
	{
		return physx::PxMat44(
			Cast<physx::PxVec4>(v._vector4[0]),
			Cast<physx::PxVec4>(v._vector4[1]),
			Cast<physx::PxVec4>(v._vector4[2]),
			Cast<physx::PxVec4>(v._vector4[3]));
	}

}

#endif