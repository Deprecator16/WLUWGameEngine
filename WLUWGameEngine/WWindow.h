#pragma once
#include "Header.h"

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

class WWindow
{
public:
	// Constructor/destructor
	WWindow();
	~WWindow();

	// Information functions
	SDL_Window* getWindow();

private:
	// Core window
	SDL_Window* window;
};

