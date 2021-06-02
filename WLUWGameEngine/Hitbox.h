#pragma once

#include "Vector2.h"
#include "Header.h"

class Hitbox
{
public:
	Hitbox(Vector2 pos, Vector2 size);

	bool checkOverlap(Hitbox target);

	SDL_Rect* getBox();
	Vector2 getPos();
	Vector2 getSize();

	bool topEnabled;
	bool botEnabled;
	bool leftEnabled;
	bool rightEnabled;

private:
	SDL_Rect box;
};

