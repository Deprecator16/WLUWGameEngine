#include "Hitbox.h"

Hitbox::Hitbox(Vector2 pos, Vector2 size) :
	pos(pos)
{
	box.x = pos.x;
	box.y = pos.y;
	box.w = size.x;
	box.h = size.y;

	predict = box;
}

void Hitbox::move(float deltaTime)
{
	pos = pos + (vel * deltaTime);
	box.x = pos.x;
	box.y = pos.y;
}

void Hitbox::updatePredict(float deltaTime)
{
	predictPos = pos;
	predictPos.x += (vel.x * deltaTime);
	predictPos.y += (vel.y * deltaTime);

	predict = box;
	predict.x = predictPos.x;
	predict.y = predictPos.y;
}

bool Hitbox::isColliding(Hitbox* target)
{
	Vector2 targetPos = target->getPos();
	Vector2 targetSize = target->getSize();

	return pos.x       + box.w        >= targetPos.x &&
		   targetPos.x + targetSize.x >= pos.x       &&
		   pos.y       + box.h        >= targetPos.y &&
		   targetPos.y + targetSize.y >= pos.y;
}

bool Hitbox::predictCollision(Hitbox* target)
{
	Vector2 targetPos = target->getPos();
	Vector2 targetSize = target->getSize();

	return predictPos.x + predict.w    >= targetPos.x  &&
		   targetPos.x  + targetSize.x >= predictPos.x &&
		   predictPos.y + predict.h    >= targetPos.y  &&
	       targetPos.y  + targetSize.y >= predictPos.y;
}

Vector2 Hitbox::getDistanceTo(Hitbox* target)
{
	Vector2 distance;

	Vector2 targetPos = target->getPos();
	Vector2 targetSize = target->getSize();

	// Horizontal
	// Player is to the left of block
	if (pos.x + box.w < targetPos.x)
	{
		distance.x = targetPos.x - (pos.x + box.w);
	}
	// Player is to the right of block
	else if (pos.x > targetPos.x + targetSize.x)
	{
		distance.x = (targetPos.x + targetSize.x) - pos.x;
	}
	// Block eclipses player on X axis
	else
	{
		distance.x = 0.0;
	}

	// Vertical
	// Player is above block
	if (pos.y + box.h < targetPos.y)
	{
		distance.y = targetPos.y - (pos.y + box.h);
	}
	// Player is below block
	else if (pos.y > targetPos.y + targetSize.y)
	{
		distance.y = (targetPos.y + targetSize.y) - pos.y;
	}
	// Block eclipses player on Y axis
	else
	{
		distance.y = 0.0;
	}

	return distance;
}

SDL_Rect* Hitbox::getBox()
{
	return &box;
}

SDL_Rect* Hitbox::getPredict()
{
	// Change position of prediction box
	return &predict;
}

Vector2 Hitbox::getPos()
{
	return pos;
}

Vector2 Hitbox::getSize()
{
	return Vector2(box.w, box.h);
}

Vector2 Hitbox::getVel()
{
	return vel;
}

Vector2 Hitbox::getPredictPos()
{
	return Vector2(predictPos.x, predictPos.y);
}

void Hitbox::setPos(Vector2 pos)
{
	this->pos = pos;
	box.x = pos.x;
	box.y = pos.y;
}

void Hitbox::setVel(Vector2 vel)
{
	this->vel = vel;
}