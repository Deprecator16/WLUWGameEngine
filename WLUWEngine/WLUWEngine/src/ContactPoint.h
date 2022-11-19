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
			EDGE,
			POINT
		};

		Vector2 point;
		Vector2 normal;
		Vector2 separation;
		float fraction;
		ContactType contactType;

		static bool compare(ContactPoint& a, ContactPoint& b)
		{
			if (a.contactType == NO_CONTACT) return false;
			if (b.contactType == NO_CONTACT) return true;

			if (a.fraction < b.fraction) return true;
			if (a.fraction > b.fraction) return false;

			if (a.contactType == ContactType::EDGE && b.contactType == ContactType::POINT) return true;
			if (a.contactType == ContactType::POINT && b.contactType == ContactType::EDGE) return false;

			return false;
		}
	};
}
