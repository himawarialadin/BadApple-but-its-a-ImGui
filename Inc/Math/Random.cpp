#include "Random.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Math.h"
#include "Bounds.h"

namespace Math 
{
	Vector3 Random::InsideUnitSphere(Math::Vector3 const& radius /*= Math::Vector3::One*/)
	{
		auto rangeX = std::uniform_real_distribution<float>(-radius.x, radius.x);
		auto rangeY = std::uniform_real_distribution<float>(-radius.y, radius.y);
		auto rangeZ = std::uniform_real_distribution<float>(-radius.z, radius.z);

		auto& engine = Engine();
		return Vector3(rangeX(engine), rangeY(engine), rangeZ(engine)).Normalize();
	}

	Vector3 Random::InsideUnitBox(Math::Vector3 const& harfExtents)
	{
		auto rangeX = std::uniform_real_distribution<float>(-harfExtents.x, harfExtents.x);
		auto rangeY = std::uniform_real_distribution<float>(-harfExtents.y, harfExtents.y);
		auto rangeZ = std::uniform_real_distribution<float>(-harfExtents.z, harfExtents.z);

		auto& engine = Engine();
		return Vector3(rangeX(engine), rangeY(engine), rangeZ(engine));
	}

	Quaternion Random::Rotation()
	{
		auto axis	= InsideUnitSphere();
		auto angle = Random::Value() * 360.0f;

		return Quaternion::AngleAxis(axis, angle);
	}

	Vector3 Random::InsideBounds(Bounds const& bounds)
	{
		auto range = std::uniform_real_distribution<float>(0.0f, 1.0f);
		auto& engine = Engine();

		return bounds.min + (Vector3::Scale(bounds.Size(), Vector3(range(engine), range(engine), range(engine))));
	}

}