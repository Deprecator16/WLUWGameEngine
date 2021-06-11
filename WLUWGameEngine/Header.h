#pragma once

#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Vector2.h"

using namespace std;

enum class PlayerState
{
	DEAD = 0,
	IDLE = 1,
	WALK = 2,
	RUN = 3,
	JUMP = 4,
	CROUCH = 5
};

enum class Axis
{
	X = 0,
	Y = 1
};