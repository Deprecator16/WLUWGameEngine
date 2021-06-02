#pragma once

#include "Vector2.h"
#include "WTexture.h"
#include "Hitbox.h"
#include "Header.h"

class Block
{
public:
	Block(Vector2 hBoxPos, Vector2 hBoxSize);

	Hitbox* getHBox();

private:
	Hitbox hBox;

};

