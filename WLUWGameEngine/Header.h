#pragma once

#include <cmath>
#include <iostream>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

using namespace std;

enum playerState
{
	DEAD = 0,
	IDLE = 1,
	WALK = 2,
	RUN = 3,
	JUMP = 4,
	CROUCH = 5
};