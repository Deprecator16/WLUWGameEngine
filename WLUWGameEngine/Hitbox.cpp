#include "Hitbox.h"

Hitbox::Hitbox(Vector2 pos, Vector2 size)
{
	box.x = pos.x;
	box.y = pos.y;
	box.w = size.x;
	box.h = size.y;
}

bool Hitbox::checkOverlap(Hitbox target)
{
	// Calculate target boundaries
	float tTop = target.getPos().y;
	float tBot = target.getPos().y + target.getSize().y;
	float tLeft = target.getPos().x;
	float tRight = target.getPos().x + target.getSize().x;

	// Calculate self boundaries
	float mTop = box.y;
	float mBot = box.y + box.h;
	float mLeft = box.x;
	float mRight = box.x + box.w;

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
	Vector2 pos;
	pos.x = box.x;
	pos.y = box.y;
	return pos;
}

Vector2 Hitbox::getSize()
{
	Vector2 size;
	size.x = box.w;
	size.y = box.h;
	return size;
}