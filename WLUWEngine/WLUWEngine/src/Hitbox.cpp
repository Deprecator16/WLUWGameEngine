#include "Hitbox.h"
#include <algorithm>
#include <iomanip>
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

void WLUW::Hitbox::handleCollisions(std::vector<WObject*> objects, float deltaTime)
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
		//for (auto& obj : collidables)
		{
			Collision collisionData = Physics::getCollisionData(linkedObj, obj, deltaTime);
			if (collisionData.collisionType == Collision::CollisionType::NO_COLLISION)
				continue;
			if (collisionData.separation.size() > (vel * deltaTime).size())
				continue;
			collisions.push_back(collisionData);
		}

		// No more collisions detected
		if (collisions.size() == 0)
			break;

		// Sort
		std::sort(collisions.begin(), collisions.end(), Collision::compare);

		if (debugOutput)
		{
			std::cout << "COLLISIONS ==================================================" << std::endl;
			for (auto& collision : collisions)
			{
				std::cout <<
					//"[numObjectsHit=" << objectsHit.size() << "], " <<
					"[numCollisions=" << collisions.size() << "], " <<
					"[collidierPos=" << collision.otherObject->getHitbox()->getPos() << "], " <<
					"[point=" << collision.point << "], " <<
					"[normal=" << collision.normal << "], " <<
					"[separation=" << collision.separation << "], " <<
					"[separationSize=" << collision.separation.size() << "], " <<
					"[fraction=" << collision.fraction << "], " <<
					"[direction=" << collision.direction << "], " <<
					"[collisionType=" << collision.collisionType << "], " <<
					"[vel=" << vel << "]" <<
					std::endl;
			}
		}

		// Redirect soft object velocity
		vel = vel.projectOntoAxis(collisions[0].normal.normal());
		//vel = (vel - (collisions[0].separation / deltaTime)).projectOntoAxis(collisions[0].normal.normal());

		// Trigger OnCollide callbacks
		collisions[0].object->OnCollide(collisions[0].otherObject, collisions[0]);
		collisions[0].otherObject->OnCollide(collisions[0].object, collisions[0]);

		// Move soft box based on min distance
		pos = pos + collisions[0].separation;



		if (debugOutput)
		{
			if (Physics::clips(this, collidables))
				std::cout << "Object clips during solving loop" << std::endl;
		}
		
		/*
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
		*/
		
		// Stop loop if the soft box isn't moving anymore
		if (vel == Vector2())
			break;
	}

	if (debugOutput)
	{
		if (Physics::clips(this, collidables))
			std::cout << "Object clips after solving loop" << std::endl;
	}

	if (debugOutput)
		std::cout << "AFTER SOLVING LOOP: pos=" << pos << ", vel=" << vel <<std::endl;
}

void WLUW::Hitbox::move(std::vector<WObject*> objects, float deltaTime)
{
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

	if (debugOutput)
	{
		if (Physics::clips(this, collidables))
			std::cout << "Object clips before movement" << std::endl;
	}


	pos = pos + (vel * deltaTime);


	if (debugOutput)
	{
		if (Physics::clips(this, collidables))
			std::cout << "Object clips after movement" << std::endl;
	}
}
