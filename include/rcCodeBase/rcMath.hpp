#ifndef RC_MATH_HPP
#define RC_MATH_HPP

#include <cmath>
#include <vector>

namespace rc{

	struct AABB
	{
		float min[3];
		float max[3];
	};

	template <typename T>
	inline AABB get_aabb(const std::vector<T>& points)
	{
		AABB aabb;
		aabb.max[0] = aabb.max[1] = aabb.max[2] = -std::numeric_limits<float>::max();
		aabb.min[0] = aabb.min[1] = aabb.min[2] = std::numeric_limits<float>::max();

		for (const T& point : points)
		{
			if (point.x < aabb.min[0]) aabb.min[0] = point.x;
			if (point.y < aabb.min[1]) aabb.min[1] = point.y;
			if (point.z < aabb.min[2]) aabb.min[2] = point.z;

			if (point.x > aabb.max[0]) aabb.max[0] = point.x;
			if (point.y > aabb.max[1]) aabb.max[1] = point.y;
			if (point.z > aabb.max[2]) aabb.max[2] = point.z;
		}

		return aabb;
	}
}

#endif