#pragma once
#include "Header.h"

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

class WWindow
{
public:
	WWindow();
	~WWindow();

	SDL_Window* getWindow();

private:
	SDL_Window* window;
};

