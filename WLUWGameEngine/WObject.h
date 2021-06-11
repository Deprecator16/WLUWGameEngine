#pragma once

#include "Header.h"
#include "Vector2.h"
#include "WTexture.h"
#include "Hitbox.h"

class WObject
{
public:

	WObject();
	WObject(WTexture* texture);

	void virtual update();

	WTexture* texture;
	Hitbox hitbox;
	bool shouldDraw;
};
