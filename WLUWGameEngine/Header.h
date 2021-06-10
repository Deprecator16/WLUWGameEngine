#pragma once

#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "Vector2.h"

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

enum direction
{
	TOP = 0,
	BOT = 1,
	LEFT = 2,
	RIGHT = 3
};

enum axis
{
	X = 0,
	Y = 1
};