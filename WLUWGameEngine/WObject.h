#pragma once

#include "Header.h"
#include "Vector2.h"
#include "WTexture.h"

class WObject
{
public:
	void virtual update();
	void draw();

protected:
	WTexture* texture;
	Vector2* pos;
};
