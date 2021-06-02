#pragma once

#include "Vector2.h"
#include "Header.h"

class Hitbox
{
public:
	// Constructor
	Hitbox(Vector2 pos, Vector2 size);

	// Checks if self overlaps with target
	bool checkOverlap(Hitbox target);

	// Information functions
	SDL_Rect* getBox();
	Vector2 getPos();
	Vector2 getSize();

	// Enabling triggers
	bool topEnabled;
	bool botEnabled;
	bool leftEnabled;
	bool rightEnabled;

private:
	// Hitbox rectangle
	SDL_Rect box;
};

