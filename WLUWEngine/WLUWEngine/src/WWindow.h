#pragma once

struct SDL_Window;

namespace WLUW
{
	constexpr int SCREEN_WIDTH = 1280;
	constexpr int SCREEN_HEIGHT = 720;

	class WWindow
	{
	public:
		// Constructor/destructor
		WWindow();
		WWindow(char* name, bool isFullscreen = false, int width = 1280, int height = 720);
		~WWindow();

		// Change Window Settings
		bool setWindowSize(int width, int height);

		// Information functions
		SDL_Window* getWindow();

	private:
		// Core window
		SDL_Window* window;
	};
}