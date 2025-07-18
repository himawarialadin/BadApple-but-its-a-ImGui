#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Curve.h"

namespace Math
{
	template<class Archive>
	void serialize(Archive& archive, Math::Vector2& v)
	{
		archive(cereal::make_size_tag(static_cast<cereal::size_type>(Math::Vector2::Element)));
		archive(v.x, v.y);
	}

	template<class Archive>
	void serialize(Archive& archive, Math::Vector3& v)
	{
		archive(cereal::make_size_tag(static_cast<cereal::size_type>(Math::Vector3::Element)));
		archive(v.x, v.y, v.z);
	}

	template<class Archive>
	void serialize(Archive& archive, Math::Vector4& v)
	{
		archive(cereal::make_size_tag(static_cast<cereal::size_type>(Math::Vector4::Element)));
		archive(v.x, v.y, v.z, v.w);
	}

	template<class Archive>
	void serialize(Archive& archive, Math::Quaternion& v)
	{
		archive(cereal::make_size_tag(static_cast<cereal::size_type>(4)));
		archive(v.x, v.y, v.z, v.w);
	}

	template<class Archive>
	void serialize(Archive& archive, Math::Matrix4x4& m)
	{
		archive(cereal::make_size_tag(static_cast<cereal::size_type>(16)));
		archive(
			m._m00, m._m01, m._m02, m._m03,
			m._m10, m._m11, m._m12, m._m13,
			m._m20, m._m11, m._m22, m._m23,
			m._m30, m._m31, m._m32, m._m33
		);
	}

	template<class Archive>
	void load(Archive& archive, Math::Curve& curve)
	{
		cereal::size_type size;
		archive(cereal::make_size_tag(size));
		curve.points.resize(static_cast<size_t>(size));
		
		for (auto& point : curve.points)
			archive(point);
	}

	template<class Archive>
	void save(Archive& archive, Math::Curve const& curve)
	{
		archive(cereal::make_size_tag(static_cast<cereal::size_type>(curve.points.size())));
		for (auto& point : curve.points)
			archive(point);
	}
}