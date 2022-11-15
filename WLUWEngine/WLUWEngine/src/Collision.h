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

        Hitbox* box;
        Hitbox* otherBox;
        Vector2 point;
        Edge edge;
        Vector2 pointOfIntersection;
        Vector2 distance;
        double timeOfImpact;
        double totalDistanceFromEdgeToShape;
        Direction direction;
        CollisionType collisionType;

        friend bool operator==(const Collision collision1, const Collision collision2)
        {
            return collision1.box == collision2.box &&
                collision1.otherBox == collision2.otherBox &&
                collision1.point == collision2.point &&
                collision1.edge == collision2.edge &&
                collision1.pointOfIntersection == collision2.pointOfIntersection &&
                collision1.distance == collision2.distance &&
                collision1.timeOfImpact == collision2.timeOfImpact &&
                collision1.totalDistanceFromEdgeToShape == collision2.totalDistanceFromEdgeToShape &&
                collision1.direction == collision2.direction &&
                collision1.collisionType == collision2.collisionType;
        }
    };
}

