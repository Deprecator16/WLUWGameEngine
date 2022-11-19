#pragma once

#include "Header.h"

#include <vector>
#include "ContactPoint.h"
#include "Edge.h"
#include "RaycastHit.h"
#include "Shape.h"
#include "Vector2.h"

namespace WLUW
{
    class Collision;
    class Hitbox;
    class WObject;

	class Physics
	{
	public:
        static bool checkCollision(Shape& a, Shape& b);

        static std::vector<WObject*> shapecastAll(std::vector<WObject*> objects, Shape shape, Vector2 direction, float distance);
        static std::vector<ContactPoint> getContactPoints(Shape* softBox, Shape* hardBox, Vector2 direction, float distance);
        static Collision getCollisionData(WObject* softObject, WObject* hardObject, float deltaTime);

        static bool clips(Hitbox* softBox, std::vector<WObject*> objects);

	private:

	};
}

