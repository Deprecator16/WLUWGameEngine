#include "Hitbox.h"

Hitbox::Hitbox(Vector2 pos, Vector2 size)
{
	this->pos.x = pos.x;
	this->pos.y = pos.y;
	box.x = pos.x;
	box.y = pos.y;
	box.w = size.x;
	box.h = size.y;

}

void Hitbox::move(Vector2 vel)
{
	// CHECK COLLISION

	pos.x += vel.x;
	pos.y += vel.y;

	box.x = pos.x;
	box.y = pos.y;
}

bool Hitbox::checkOverlap(Hitbox* target)
{
	// Calculate target boundaries
	float tTop = target->getPos().y;
	float tBot = target->getPos().y + target->getSize().y;
	float tLeft = target->getPos().x;
	float tRight = target->getPos().x + target->getSize().x;

	// Calculate self boundaries
	float mTop = pos.y;
	float mBot = pos.y + box.h;
	float mLeft = pos.x;
	float mRight = pos.x + box.w;

	// Check if sides of self are outside target
	if (mBot <= tTop)
	{
		return false;
	}

	if (mTop >= tBot)
	{
		return false;
	}

	if (mRight <= tLeft)
	{
		return false;
	}

	if (mLeft >= tRight)
	{
		return false;
	}

	// If none of the sides from self are outside target
	return true;
}

SDL_Rect* Hitbox::getBox()
{
	return &box;
}

Vector2 Hitbox::getPos()
{
	return pos;
}

Vector2 Hitbox::getSize()
{
	Vector2 size;
	size.x = box.w;
	size.y = box.h;
	return size;
}

void Hitbox::setPos(Vector2 pos)
{
	this->pos.x = pos.x;
	this->pos.y = pos.y;
	box.x = pos.x;
	box.y = pos.y;
}