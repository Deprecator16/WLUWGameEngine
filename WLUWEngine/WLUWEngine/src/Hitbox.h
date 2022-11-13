#pragma once

#include "SDL.h"
#include "Shape.h"
#include "Vector2.h"

namespace WLUW
{
    class Hitbox :
        public Shape
    {
    public:
        enum Inertia
        {
            SOFT = 0,
            HARD
        };

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

        class CollisionData
        {
        public:
            unsigned int hardBoxId;
            Vector2 point;
            std::pair<Vector2, Vector2> edge;
            Vector2 pointOfIntersection;
            Vector2 distance;
            double timeOfImpact;
            double totalDistanceFromEdgeToShape;
            Direction direction;
            CollisionType collisionType;
        };

        // Constructors
        Hitbox();
        Hitbox(Shape box, Inertia inertia);
        Hitbox(const Hitbox& other);
        Hitbox(Hitbox&& other) noexcept;

        // Assignment operators
        Hitbox& operator=(const Hitbox& other);
        Hitbox& operator=(Hitbox&& other) noexcept;

        // Update functions
        void move(double deltaTime);
        void updatePredict(double deltaTime);

        // Checks if self overlaps with target
        std::pair<Vector2, double> checkCollision(Hitbox* target);
        std::pair<Vector2, double> predictCollision(Hitbox* target);

        // Getters
        Shape* getBox() { return &box; }
        Shape* getPredict() { return &predict; }
        Vector2 getPos() { return box.getPos(); }
        Vector2 getPredictPos() { return predict.getPos(); }
        Vector2 getVel() { return vel; }
        SDL_Rect getAABB();
        Inertia getInertia() { return inertia; }

        // Setters
        void setBox(Shape box) { this->box = box; this->predict = box; }
        void setPos(Vector2 pos) { box.setPos(pos); }
        void setPredictPos(Vector2 pos) { predict.setPos(pos); }
        void setVel(Vector2 vel) { this->vel = vel; }

    private:
        Shape box;
        Shape predict;

        // Physics vectors
        Vector2 vel;

        Inertia inertia;
    };
}

