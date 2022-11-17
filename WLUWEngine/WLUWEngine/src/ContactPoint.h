#pragma once

#include "Edge.h"
#include "Hitbox.h"
#include "Vector2.h"

namespace WLUW
{
	class ContactPoint
	{
	public:
		enum ContactType
		{
			NO_CONTACT = 0,
			POINT,
			EDGE
		};

		Vector2 point;
		Vector2 normal;
		Vector2 separation;
		double fraction;
		ContactType contactType;

		/*
		Vector2 relativeVel;
		Hitbox* box;
		Hitbox* otherBox;
		*/

		static bool compare(ContactPoint& a, ContactPoint& b)
		{
			if (a.contactType == NO_CONTACT) return false;
			if (b.contactType == NO_CONTACT) return true;

			return a.fraction < b.fraction;
		}
	};
}
