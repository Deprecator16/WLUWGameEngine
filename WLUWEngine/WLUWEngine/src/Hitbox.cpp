#include "Hitbox.h"
#include <iostream>

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

void WLUW::Hitbox::move(double deltaTime)
{
	//std::cout << "moving hitbox with vel: x = " << vel.x << ", y = " << vel.y << std::endl;
	pos = pos +(vel * deltaTime);
}

void WLUW::Hitbox::updatePredict(double deltaTime)
{
	predict = *this;
	predict.setPos(pos + (vel * deltaTime));
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

