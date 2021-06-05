#pragma once

#include "Header.h"
#include "Vector2.h"
#include "WTexture.h"

class WObject
{
public:

	WObject();

	void virtual update();
	void draw();

	WTexture* texture;
	Vector2 pos;
};

