#pragma once

#include "Header.h"
#include "Vector2.h"
#include "WTexture.h"

class WObject
{
	WTexture* texture;
	Vector2* pos;
	
	void virtual update();
	void draw();
};

