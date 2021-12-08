#pragma once

class SDL_Window;

namespace WLUW
{
	constexpr int SCREEN_WIDTH = 640;
	constexpr int SCREEN_HEIGHT = 480;

	class WWindow
	{
	public:
		// Constructor/destructor
		WWindow();
		WWindow(char* name, bool isFullscreen = false, int width = 640, int height = 480);
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