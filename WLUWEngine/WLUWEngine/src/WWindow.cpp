#include "WWindow.h"
#include "SDL.h"

namespace WLUW
{
	WWindow::WWindow()
	{
		window = SDL_CreateWindow("WLUW Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN & SDL_WINDOW_MOUSE_FOCUS);
	}

	/**
	* Creates a window with the given values
	* @param name The name displayed at the top of the window
	* @param isFullscreen Controls if the window opens in fullscreen
	* @param width The width of the window (not necessary if isFullscreen is true)
	* @param height The height of the window (not necessary if isFullscreen is true)
	*/
	WWindow::WWindow(char* name, bool isFullscreen, int width, int height)
	{
		if (isFullscreen)
			window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN & SDL_WINDOW_MOUSE_FOCUS);
		else
			window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN & SDL_WINDOW_MOUSE_FOCUS & SDL_WINDOW_FULLSCREEN_DESKTOP);
	}

	WWindow::~WWindow()
	{
		SDL_DestroyWindow(window);
	}

	/**
	* Sets the size of the window
	* @param width The new width of the window
	* @param height The new height of the window
	*/
	bool WWindow::setWindowSize(int width, int height)
	{
		SDL_SetWindowSize(window, width, height);
		return true;
	}

	/*
	* Returns a reference to the SDL_Window
	* @return a reference to the SDL_Window object
	*/
	SDL_Window* WWindow::getWindow()
	{
		return window;
	}
}