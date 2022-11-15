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
	predict{ box },
	inertia{ inertia },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox::Hitbox(const Hitbox& other) :
	Shape{ other },
	predict{ other.predict },
	inertia{ other.inertia },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox::Hitbox(Hitbox&& other) noexcept :
	Shape{ other },
	predict{ other.predict },
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

	this->predict = other.predict;
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

	this->predict = other.predict;
	this->inertia = other.inertia;
	this->vel = other.vel;

	return *this;
}

void WLUW::Hitbox::updatePredict(double deltaTime)
{
	predict = *this;
	predict.setPos(pos + (vel * deltaTime));
}

void WLUW::Hitbox::move(std::vector<WObject*> objects, double deltaTime)
{
	updatePredict(deltaTime);

	// Check for potential collisions and find the first one to occur
	bool collisionDetected = false;
	for (auto& object : objects)
	{
		if (object->getHitbox() == this)
			continue;

		if (this->isEqual(predict) && object->getHitbox()->isEqual(*object->getHitbox()->getPredict()))
			continue;

		// Check if the boxes' AABBs overlap
		std::pair<Vector2, Vector2> AABB1 = getAABB();
		std::pair<Vector2, Vector2> AABB2 = object->getHitbox()->getAABB();
		if (!(AABB1.first.x + AABB1.second.x >= AABB2.first.x &&
			AABB2.first.x + AABB2.second.x >= AABB1.first.x &&
			AABB1.first.y + AABB1.second.y >= AABB2.first.y &&
			AABB2.first.y + AABB2.second.y >= AABB1.first.y))
			continue;

		// Check if a collision occurs
		std::pair<Vector2, double> mtv = predictCollision(object->getHitbox());
		if (mtv.second == 0) // No collision
			continue;

		// A collision is guaranteed to occur
		collisionDetected = true;




	  /*
		if (collisionData.timeOfImpact < minTimeOfImpact)
		{
			// New min time of impact found, reset colliders storage and add new box
			minTimeOfImpact = collisionData.timeOfImpact;
			colliders.clear();
			colliders.push_back(collisionData);
		}
		else if (collisionData.timeOfImpact == minTimeOfImpact)
		{
			// Same time of impact as current min, add box to colliders storage
			colliders.push_back(collisionData);
		}
		*/
	}


	// Get collision data and create collision object
	//Collision collisionData = Physics::getCollisionData(this, object->getHitbox(), deltaTime);

	//std::cout << "moving hitbox with vel: x = " << vel.x << ", y = " << vel.y << std::endl;
	if (!collisionDetected)
		pos = pos + (vel * deltaTime);
}

std::pair<WLUW::Vector2, WLUW::Vector2> WLUW::Hitbox::getAABB()
{
	if (points.size() == 0)
		return std::make_pair(Vector2(), Vector2());

	Vector2 min;
	Vector2 max;
	for (auto& point : points)
	{
		if (point.x < min.x)
			min.x = point.x;
		if (point.x > max.x)
			max.x = point.x;
		if (point.y < min.y)
			min.y = point.y;
		if (point.y > max.y)
			max.y = point.y;
	}
	for (auto& point : predict.getPoints())
	{
		if (point.x < min.x)
			min.x = point.x;
		if (point.x > max.x)
			max.x = point.x;
		if (point.y < min.y)
			min.y = point.y;
		if (point.y > max.y)
			max.y = point.y;
	}

	return std::make_pair(min, max - min);
}

