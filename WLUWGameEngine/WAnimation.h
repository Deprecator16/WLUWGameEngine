#pragma once

#include "Vector2.h"
#include "WTexture.h"
#include "Header.h"

class WAnimation
{
public:
	WAnimation(WTexture* tex, Vector2 imageCount, float switchTime);

	void update(int row, int colBegin, int colEnd, float deltaTime, bool faceRight);

	void setTexture(WTexture* tex);
	void setSwitchTime(float switchTime);

private:
	SDL_Rect uvRect;

	Vector2 imageCount;
	Vector2 currentImage;
	
	float switchTime;
	float totalTime;

	int row;
	int col;
	int colBegin;
	int colEnd;
};

