#pragma once

#include "Edge.h"
#include "Hitbox.h"
#include "Vector2.h"

namespace WLUW
{
    class Collision
    {
    public:
        enum Direction
        {
            NO_DIRECTION = 0,
            TOP,
            BOTTOM,
            LEFT,
            RIGHT
        };

        enum CollisionType
        {
            NO_COLLISION = 0,
            EDGE_EDGE,
            POINT_EDGE,
            EDGE_POINT
        };

        /*
        Hitbox* box;
        Hitbox* otherBox;
        Vector2 normal;
        Vector2 point;
        Vector2 separation;
        double fraction;
        bool ignore;
        // CONTACTS
        */

        /*
        WObject* object;
        WObject* otherObject;
        Vector2 point;
        Vector2 normal;
        Vector2 pointOfIntersection;
        Vector2 distance;
        double timeOfImpact;
        double totalDistanceFromEdgeToShape;
        Direction direction;
        CollisionType collisionType;
        */

        WObject* object;
        WObject* otherObject;
        Vector2 pointOfIntersection;
        Vector2 normal;
        Vector2 separation;
        double fraction;

        /*
        double totalDistanceFromEdgeToShape;
        Direction direction;
        CollisionType collisionType;
        */
        
        /*
        friend bool operator==(const Collision collision1, const Collision collision2)
        {
            return collision1.box == collision2.box &&
                collision1.otherBox == collision2.otherBox &&
                collision1.normal == collision2.normal &&
                collision1.point == collision2.point &&
                collision1.separation == collision2.separation &&

                collision1.timeOfImpact == collision2.timeOfImpact &&
                collision1.totalDistanceFromEdgeToShape == collision2.totalDistanceFromEdgeToShape &&
                collision1.direction == collision2.direction &&
                collision1.collisionType == collision2.collisionType;
        }
        */

        
        static bool compare(Collision collision1, Collision collision2)
        {
            if (collision1.object == nullptr) return false;
            if (collision2.object == nullptr) return true;

            return collision1.fraction < collision2.fraction;
        }
        

        /*
        static bool compareCollisionData(const Collision& a, const Collision& b)
        {
            if (a.timeOfImpact < b.timeOfImpact) return true;
            if (a.timeOfImpact > b.timeOfImpact) return false;

            if (a.totalDistanceFromEdgeToShape < b.totalDistanceFromEdgeToShape) return true;
            if (a.totalDistanceFromEdgeToShape > b.totalDistanceFromEdgeToShape) return false;

            return false;
        }
        */
    };
}

