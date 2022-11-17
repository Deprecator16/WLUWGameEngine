#include "Hitbox.h"
#include <algorithm>
#include <iostream>
#include "WObject.h"

WLUW::Hitbox::Hitbox() :
	inertia{ Inertia::HARD },
	vel{ 0.0, 0.0 },
	linkedObj{ nullptr }
{

}

WLUW::Hitbox::Hitbox(WObject* linkedObj) :
	inertia{ Inertia::HARD },
	vel{ 0.0, 0.0 },
	linkedObj{ linkedObj }
{

}

WLUW::Hitbox::Hitbox(WObject* linkedObj, Shape box, Inertia inertia) :
	Shape{ box },
	inertia{ inertia },
	vel{ 0.0, 0.0 },
	linkedObj{ linkedObj }
{

}

WLUW::Hitbox::Hitbox(const Hitbox& other) :
	Shape{ other },
	inertia{ other.inertia },
	vel{ 0.0, 0.0 },
	linkedObj{ other.linkedObj }
{

}

WLUW::Hitbox::Hitbox(Hitbox&& other) noexcept :
	Shape{ other },
	inertia{ other.inertia },
	vel{ 0.0, 0.0 },
	linkedObj{ nullptr }
{
	std::swap(this->linkedObj, other.linkedObj);
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
	this->linkedObj = other.linkedObj;

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
	std::swap(this->linkedObj, other.linkedObj);

	return *this;
}

void WLUW::Hitbox::handleCollisions(std::vector<WObject*> objects, double deltaTime)
{
	/*
	// Only handle collisions for soft objects
	if (inertia == Inertia::HARD)
		return;

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

	// Find and resolve collisions until no more can be found
	while (true)
	{
		// Get a list of objects that we will collide with
		std::vector<WObject*> objectsHit = Physics::shapecastAll(collidables, *this, vel.normalized(), vel.size() * deltaTime);
		
		// Get collisions
		std::vector<Collision> collisions;
		for (auto& obj : objectsHit)
		{
			Collision collisionData = Physics::getCollisionData(linkedObj, obj, deltaTime);
			if (collisionData.direction == Collision::Direction::NO_DIRECTION)
				continue;
			collisions.push_back(collisionData);
		}

		// No more collisions detected
		if (collisions.size() == 0)
			break;

		// Sort
		std::sort(collisions.begin(), collisions.end(), Collision::compareCollisionData);

		// Remove unnecessary collisions
		for (unsigned int i = 0; i < collisions.size(); ++i)
		{
			if (collisions[i].timeOfImpact != collisions[0].timeOfImpact)
			{
				collisions.erase(collisions.begin() + i);
				--i;
			}
		}
		double bestTotalDistance = collisions[0].totalDistanceFromEdgeToShape;
		for (unsigned int i = 0; i < collisions.size(); ++i)
		{
			if (collisions[i].totalDistanceFromEdgeToShape != bestTotalDistance)
			{
				collisions.erase(collisions.begin() + i);
				--i;
			}
		}

		// Loop through each collision
		Vector2 minDistance = collisions[0].distance;
		for (auto& collision : collisions)
		{
			// Redirect soft object velocity
			vel = (vel - minDistance).projectOntoAxis(collision.normal.normal());

			// Find minimum distance
			if (collision.distance.size() < minDistance.size())
				minDistance = collision.distance;

			// Trigger OnCollide callbacks
			collision.object->OnCollide(collision.otherObject, collision);
			collision.otherObject->OnCollide(collision.object, collision);
		}

		// Move soft box based on min distance
		pos = pos + minDistance;
		
		// Remove all colliders from the next loop
		for (unsigned int i = 0; i < collidables.size(); ++i)
		{
			for (auto& collision : collisions)
			{
				if (collidables[i]->getHitbox() == collision.otherObject->getHitbox())
				{
					collidables.erase(collidables.begin() + i);
					--i;
				}
			}
		}
		
		// Stop loop if the soft box isn't moving anymore
		if (vel == Vector2())
			break;
	}
	*/
}
