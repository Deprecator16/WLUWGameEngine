#pragma once

#include "Edge.h"
#include "Physics.h"
#include "RaycastHit.h"
#include "SDL.h"
#include "Shape.h"
#include "Vector2.h"

namespace WLUW
{
    class WObject;

    class Hitbox : public Shape
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

        // Helpers
        friend bool operator==(const Hitbox& lhs, const Hitbox& rhs) { return lhs.isEqual(rhs); }
        bool isEqual(Hitbox other) const
        {
            return this->type == other.type
                && this->pos == other.pos
                && this->radius == other.radius
                && this->points == other.points
                && this->normals == other.normals
                && this->vel == other.vel
                && this->inertia == other.inertia;
        }

        // Updaters
        void move(std::vector<WObject*> objects, double deltaTime);

        // Getters
        Vector2 getVel() { return vel; }
        Inertia getInertia() { return inertia; }

        // Setters
        void setVel(Vector2 vel) { this->vel = vel; }

    private:
        Vector2 vel;
        Inertia inertia;
    };
}

