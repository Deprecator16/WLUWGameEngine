#pragma once

#include "Vector2.h"
#include "WTexture.h"
#include "Header.h"

class WAnimation
{
public:
	// Constructor
	WAnimation(WTexture* tex, Vector2 imageCount, Uint32 switchTime);

	// Update animation
	void update(int row, int colBegin, int colEnd, Uint32 deltaTime, bool faceRight);

	// Manage information
	void setTexture(WTexture* tex);
	void setSwitchTime(Uint32 switchTime);

	// Fetch texture rectangle
	SDL_Rect* getClip();

private:
	SDL_Rect uvRect;

	Vector2 imageCount;
	Vector2 currentImage;
	
	Uint32 switchTime;
	Uint32 totalTime;

	int row;
	int col;
	int colBegin;
	int colEnd;
};

