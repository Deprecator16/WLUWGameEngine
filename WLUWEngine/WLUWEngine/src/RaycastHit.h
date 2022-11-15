#pragma once

#include "Vector2.h"

namespace WLUW
{
	class Hitbox;

	class RaycastHit
	{
	public:
		Vector2 centroid;
		Vector2 point;
		Vector2 normal;
		Vector2 separation;
		double fraction; // The fraction along the ray where the hit occurred
		Hitbox* hitbox;

		static bool compare(RaycastHit hit1, RaycastHit hit2)
		{
			if (hit1.hitbox == nullptr)
				return false;
			if (hit2.hitbox == nullptr)
				return true;
			return hit1.fraction < hit2.fraction;
		}
	};
}

