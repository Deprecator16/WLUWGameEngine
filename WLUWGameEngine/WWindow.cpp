#include "WWindow.h"

WWindow::WWindow()
{
	window = SDL_CreateWindow("WLUW Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN & SDL_WINDOW_MOUSE_FOCUS);
}

WWindow::~WWindow()
{
	SDL_DestroyWindow(window);
}

/*
* Get SDL window pointer
*/
SDL_Window* WWindow::getWindow()
{
	return window;
}
