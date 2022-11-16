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

			/*
			std::cout <<
				", [point: " << collisionData.point << "]" <<
				", [predictedPoint: " << collisionData.point + vel * deltaTime << "]" <<
				", [normal: " << collisionData.normal << "]" <<
				", [POI: " << collisionData.pointOfIntersection << "]" <<
				", [distance: " << collisionData.distance << "]" <<
				", [timeOfImpact=" << collisionData.timeOfImpact << "]" <<
				", [direction=" << collisionData.direction << "]" <<
				std::endl;
				*/
		}

		// No more collisions detected
		if (collisions.size() == 0)
			break;

		// Sort
		std::sort(collisions.begin(), collisions.end(), Collision::compareCollisionData);

		std::vector<Collision> collisionsBackup = collisions;

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

		std::cout << collisions.size() << std::endl;

		// Find minimum distance
		Vector2 minDistance = collisions[0].distance;
		for (auto& collision : collisions)
		{
			if (collision.distance.size() < minDistance.size())
				minDistance = collision.distance;
		}

		// Move soft box based on min distance
		pos = pos + minDistance;

		// Redirect soft object velocity
		for (auto& collision : collisions)
			vel = (vel - minDistance).projectOntoAxis(collision.normal.normal());

		// Trigger OnCollide callbacks
		for (auto& collision : collisions)
		{
			collision.object->OnCollide(collision.otherObject, collision);
			collision.otherObject->OnCollide(collision.object, collision);
		}

		
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



		/*
		// Get raycast hit
		RaycastHit hit = Physics::shapecast(collidables, *this, vel.normalized(), vel.size() * deltaTime);

		// No more collisions detected
		if (hit.hitbox == nullptr)
			break;

		// Check for collision between a soft object and a hard object
		bool ignore = true;
		if (hit.hitbox->getInertia() != inertia)
			ignore = false;

		// Solve collision
		if (inertia == Hitbox::Inertia::SOFT)
		{
			pos = pos + hit.separation;
			vel = vel.projectOntoAxis(hit.normal.normal());
		}
		//Physics::solveCollision(Collision(this, hit.hitbox, hit.normal, hit.point, hit.separation, hit.fraction, ignore), deltaTime);

		
		std::cout <<
			"[centroid=" << hit.centroid << "], " <<
			"[point=" << hit.point << "], " <<
			"[normal=" << hit.normal << "], " <<
			"[separation=" << hit.separation << "], " <<
			"[fraction=" << hit.fraction << "], " <<
			"[vel=" << vel << "], " <<
			"[hitbox pos=" << hit.hitbox->getPos() << "]" <<
			std::endl;
			

		// Remove collider from collidables vector
		for (unsigned int i = 0; i < collidables.size(); ++i)
		{
			if (collidables[i]->getHitbox() == hit.hitbox)
			{
				collidables.erase(collidables.begin() + i);
				break;
			}
		}

		// Stop loop if not moving anymore
		if (vel == Vector2())
			break;
			*/
	}
}
