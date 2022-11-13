#include "Hitbox.h"
#include <iostream>

WLUW::Hitbox::Hitbox() :
	inertia{ Inertia::HARD },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox::Hitbox(Shape box, Inertia inertia) :
	box{ box },
	predict{ box },
	inertia{ inertia },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox::Hitbox(const Hitbox& other) :
	box{ other.box },
	predict{ other.box },
	inertia{ other.inertia },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox::Hitbox(Hitbox&& other) noexcept :
	box{ other.box },
	predict{ other.box },
	inertia{ other.inertia },
	vel{ 0.0, 0.0 }
{

}

WLUW::Hitbox& WLUW::Hitbox::operator=(const Hitbox& other)
{
	this->box = other.box;
	this->predict = other.predict;
	this->inertia = other.inertia;
	this->vel = other.vel;

	return *this;
}

WLUW::Hitbox& WLUW::Hitbox::operator=(Hitbox&& other) noexcept
{
	this->box = other.box;
	this->predict = other.predict;
	this->inertia = other.inertia;
	this->vel = other.vel;

	return *this;
}

void WLUW::Hitbox::move(double deltaTime)
{
	//std::cout << "moving hitbox with vel: x = " << vel.x << ", y = " << vel.y << std::endl;
	box.setPos(box.getPos() + (vel * deltaTime));
}

void WLUW::Hitbox::updatePredict(double deltaTime)
{
	predict = box;
	predict.setPos(box.getPos() + (vel * deltaTime));
}

std::pair<WLUW::Vector2, double> WLUW::Hitbox::checkCollision(Hitbox* target)
{
	return box.checkCollision(box, *target->getBox());
}

std::pair<WLUW::Vector2, double> WLUW::Hitbox::predictCollision(Hitbox* target)
{
	return box.checkCollision(predict, *target->getPredict());
}

std::pair<WLUW::Vector2, WLUW::Vector2> WLUW::Hitbox::getAABB()
{
	if (box.getPoints().size() == 0)
		return std::make_pair(Vector2(), Vector2());

	Vector2 min;
	Vector2 max;
	for (auto& point : box.getPoints())
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

