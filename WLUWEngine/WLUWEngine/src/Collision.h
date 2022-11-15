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

        Hitbox* box;
        Hitbox* otherBox;
        Vector2 normal;
        Vector2 point;
        Vector2 separation;
        double fraction;
        bool ignore;
        // CONTACTS

        /*
        double timeOfImpact;
        double totalDistanceFromEdgeToShape;
        Direction direction;
        CollisionType collisionType;
        */

        friend bool operator==(const Collision collision1, const Collision collision2)
        {
            return collision1.box == collision2.box &&
                collision1.otherBox == collision2.otherBox &&
                collision1.normal == collision2.normal &&
                collision1.point == collision2.point &&
                collision1.separation == collision2.separation; /* &&
                collision1.timeOfImpact == collision2.timeOfImpact &&
                collision1.totalDistanceFromEdgeToShape == collision2.totalDistanceFromEdgeToShape &&
                collision1.direction == collision2.direction &&
                collision1.collisionType == collision2.collisionType;*/
        }

        static bool compare(Collision collision1, Collision collision2)
        {
            if (collision1.box == nullptr) return false;
            if (collision2.box == nullptr) return true;

            return collision1.fraction < collision2.fraction;
        }
    };
}

