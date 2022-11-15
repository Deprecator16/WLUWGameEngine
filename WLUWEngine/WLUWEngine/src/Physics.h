#pragma once

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
        // Casts a line segment and returns all hits
        static std::vector<RaycastHit> linecastAll(std::vector<WObject*> objects, Vector2 start, Vector2 end);
        // Casts a line segment and returns the first hit
        static RaycastHit linecast(std::vector<WObject*> objects, Vector2 start, Vector2 end);
        // Casts a ray and returns all hits
        static std::vector<RaycastHit> raycastAll(std::vector<WObject*> objects, Vector2 origin, Vector2 direction, double distance);
        // Casts a ray and returns the first hit
        static RaycastHit raycast(std::vector<WObject*> objects, Vector2 origin, Vector2 direction, double distance);
        // Casts an edge and returns all hits
        static std::vector<RaycastHit> edgecastAll(std::vector<WObject*> objects, Edge edge, Vector2 direction, double distance);
        // Casts an edge and returns the first hit
        static RaycastHit edgecast(std::vector<WObject*> objects, Edge edge, Vector2 direction, double distance);
        // Casts a shape and returns all hits
        static std::vector<RaycastHit> shapecastAll(std::vector<WObject*> objects, Shape shape, Vector2 direction, double distance);
        // Casts a shape and returns the first hit
        static RaycastHit shapecast(std::vector<WObject*> objects, Shape shape, Vector2 direction, double distance);

        //static Collision getCollisionData(Hitbox* softBox, Hitbox* hardBox, double deltaTime);
		static void solveCollision(Collision collision, double deltaTime);

	private:

	};
}

