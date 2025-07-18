#include "Curve.h"
#include <algorithm>

namespace Math
{

	float Curve::Evaluate(float t)
	{
		const int num = static_cast<int>(points.size());
		constexpr float coefs[16] = 
		{
			-1.0f,  2.0f, -1.0f,  0.0f,
			 3.0f, -5.0f,  0.0f,  2.0f,
			-3.0f,  4.0f,  1.0f,  0.0f,
			 1.0f, -1.0f,  0.0f,  0.0f
		};

		if (points.empty())
			return 0.0f;
		if(t > GetMaxX())
			return points.back().y;
		if (t < GetMinX())
			return points.front().y;

		float v = 0;

		// Find the segment containing t
		int k = 0;
		while (k < num && points[k].x < t)
			k++;

		// Ensure k is within valid range
		k = std::clamp(k, 1, num - 1);

		// Time values for interpolation
		float key0 = points[static_cast<size_t>(k - 1)].x;
		float key1 = points[k].x;

		// Interpolant
		float h = (t - key0) / (key1 - key0);

		// Initialize result
		v = 0.0f;

		// Add basis functions
		for (int i = 0; i < 4; ++i)
		{
			const float* co = &coefs[4 * i];
			float b = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);

			// Calculate the index of the key point to use
			int kn = std::clamp(k + i - 2, 0, num - 1);

			// Accumulate the result for the Y component
			v += b * points[kn].y;
		}

		return v;
	}
}