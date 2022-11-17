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
            EDGE,
            POINT
        };

        WObject* object;
        WObject* otherObject;
        Vector2 point;
        Vector2 normal;
        Vector2 separation;
        double fraction;
        Direction direction;
        CollisionType collisionType;
        
        static bool compare(Collision collision1, Collision collision2)
        {
            if (collision1.collisionType == CollisionType::NO_COLLISION) return false;
            if (collision1.collisionType == CollisionType::NO_COLLISION) return true;

            if (collision1.fraction < collision2.fraction) return true;
            if (collision1.fraction > collision2.fraction) return false;

            if (collision1.collisionType == CollisionType::EDGE && collision2.collisionType == CollisionType::POINT) return true;
            if (collision1.collisionType == CollisionType::POINT && collision2.collisionType == CollisionType::EDGE) return false;

            return false;
        }
    };
}

