#include "WWindow.h"
#include "WRendering.h"
#include <cstdlib>

#include "SDL.h"

int main(int argc, char** argv)
{
	WLUW::WWindow win((char*)"Hello World");
	WLUW::WRenderer testRenderer(win);

	// Test code
	bool shouldQuit = false;
	SDL_Event event;
	while (!shouldQuit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
			case SDL_QUIT:
				shouldQuit = true;
				break;
			}
		}
		
		testRenderer.clear(SDL_Color{204, 187, 100, 255});
		testRenderer.present();
	}
	//win.setWindowSize(790, 500);

	return 0;
}