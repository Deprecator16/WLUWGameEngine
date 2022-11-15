#include "Hitbox.h"
#include <iostream>
#include "WObject.h"

WLUW::Hitbox::Hitbox() :
	inertia{ Inertia::HARD },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox::Hitbox(Shape box, Inertia inertia) :
	Shape{ box },
	inertia{ inertia },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox::Hitbox(const Hitbox& other) :
	Shape{ other },
	inertia{ other.inertia },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox::Hitbox(Hitbox&& other) noexcept :
	Shape{ other },
	inertia{ other.inertia },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox& WLUW::Hitbox::operator=(const Hitbox& other)
{
	this->type = other.type;
	this->pos = other.pos;
	this->radius = other.radius;
	this->points = other.points;
	this->normals = other.normals;

	this->inertia = other.inertia;
	this->vel = other.vel;

	return *this;
}

WLUW::Hitbox& WLUW::Hitbox::operator=(Hitbox&& other) noexcept
{
	this->type = other.type;
	this->pos = other.pos;
	this->radius = other.radius;
	std::swap(this->points, other.points);
	std::swap(this->normals, other.normals);

	this->inertia = other.inertia;
	this->vel = other.vel;

	return *this;
}

void WLUW::Hitbox::move(std::vector<WObject*> objects, double deltaTime)
{
	// Remove self from collidable objects
	std::vector<WObject*> collidables = objects;
	for (unsigned int i = 0; i < objects.size(); ++i)
	{
		if (objects[i]->getHitbox() == this)
		{
			collidables.erase(collidables.begin() + i);
			break;
		}
	}

	RaycastHit hit = Physics::shapecast(collidables, *this, vel.normalized(), vel.size() * deltaTime);

	if (hit.hitbox != nullptr) // Collision detected
	{
		// Remove collider from collidables vector
		for (unsigned int i = 0; i < collidables.size(); ++i)
		{
			if (collidables[i]->getHitbox() == hit.hitbox)
			{
				collidables.erase(collidables.begin() + i);
				break;
			}
		}

		// Check for collision between a soft object and a hard object
		bool ignore = true;
		if (hit.hitbox->getInertia() != inertia)
			ignore = false;

		Physics::solveCollision(Collision(this, hit.hitbox, hit.normal, hit.point, hit.separation, ignore), collidables, deltaTime);

		if (!ignore)
			return;
	}

	// No collision detected
	pos = pos + (vel * deltaTime);
}
