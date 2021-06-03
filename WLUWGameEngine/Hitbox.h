#pragma once

#include "Vector2.h"
#include "Header.h"

class Hitbox
{
public:
	// Constructor
	Hitbox(Vector2 pos, Vector2 size);

	// Update functions
	void move(Vector2 vel);

	// Checks if self overlaps with target
	bool checkOverlap(Hitbox* target);

	// Information functions
	SDL_Rect* getBox();
	Vector2 getPos();
	Vector2 getSize();

	// Management functions
	void setPos(Vector2 pos);

	// Enabling triggers
	bool topEnabled;
	bool botEnabled;
	bool leftEnabled;
	bool rightEnabled;

private:
	// Hitbox rectangle
	SDL_Rect box;
	
	// Position vector (kept as float)
	Vector2 pos;
};

