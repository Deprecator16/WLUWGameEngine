#pragma once

#include "Edge.h"
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
        std::pair<Vector2, double> predictCollision(Hitbox* target) { return checkCollision(predict, *target->getPredict()); }

        // Getters
        Shape* getPredict() { return &predict; }
        Vector2 getPredictPos() { return predict.getPos(); }
        Vector2 getVel() { return vel; }
        std::pair<Vector2, Vector2> getAABB();
        Inertia getInertia() { return inertia; }

        // Setters
        void setPredictPos(Vector2 pos) { predict.setPos(pos); }
        void setVel(Vector2 vel) { this->vel = vel; }

    private:
        Shape predict;

        // Physics vectors
        Vector2 vel;

        Inertia inertia;
    };
}

