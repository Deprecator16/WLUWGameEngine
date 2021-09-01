#pragma once

#include "Vector2.h"
#include "WTexture.h"
#include "Hitbox.h"
#include "Header.h"
#include "WObject.h"

class Block : public WObject
{
public:
	// Constructor
	Block(Vector2 hBoxPos, Vector2 hBoxSize);

	// Information functions
	Hitbox* getHitbox();

private:
	// Associated hitbox
	Hitbox hitbox;

};

