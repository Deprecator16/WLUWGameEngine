/*
WARNING: the code that follows will make you cry;
a safety pig has been provided below for your benefit
						 _
 _._ _..._ .-',     _.._(`))
'-. `     '  /-._.-'    ',/
  )         \            '.
 / _    _    |             \
|  a    a    /              |
 \   .-.                     ;
  '-('' ).-'       ,'       ;
	 '-;           |      .'
		\           \    /
		| 7  .__  _.-\   \
		| |  |  ``/  /`  /
	   /,_|  |   /,_/   /
		  /,_/      '`-'
*/

#include "WWindow.h"
#include "WRendering.h"
#include <cstdlib>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "InputManager.h"
#include "Game.h"
#include "WTimer.h"

// Initialize SDL
bool init()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Initialize SDL_image
	int imageFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imageFlags) & imageFlags))
	{
		printf("SDL_image could not initialize! SDL Error: %s\n", IMG_GetError());
		return false;
	}

	// Initialize SDL_ttf
	if (TTF_Init() < 0)
	{
		printf("SDL_TTF could not initialize!SDL Error : % s\n", TTF_GetError());
		return false;
	}

	// Initialize SDL_mixer	
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	return true;
}

/* Loads texture at the given path
* path: the image location relative to the solution
* renderer: the renderer which will be rendering the texture
*/
SDL_Texture* loadTexture(std::string path, SDL_Renderer* renderer)
{
	// Final texture
	SDL_Texture* newText = NULL;

	// Load image to surface
	SDL_Surface* loadedImg = IMG_Load(path.c_str());
	// Convert surface to texture
	newText = SDL_CreateTextureFromSurface(renderer, loadedImg);

	// TODO: Idiotproof
	// TODO: Drink some water
	// TODO: Fix your posture

	SDL_FreeSurface(loadedImg);
	return newText;
}


int main(int argc, char** argv)
{
	WLUW::WWindow win((char*)"Hello World");
	//WLUW::WRenderer testRenderer(win);
	SDL_Renderer* renderer = SDL_CreateRenderer(win.getWindow(), -1, 0);

	// Initialize timer
	WTimer timer;
	double deltaTime = 0.0;
	double timeElapsed = 0.0;
	unsigned int framerate = 0;

	// Initialize input manager
	WLUW::InputManager inputManager;

	// Initialize game
	WLUW::SampleGame::Game::GameData gameData(&win, &inputManager);
	WLUW::SampleGame::Game game(gameData);

	// Test code
	bool shouldQuit = false;
	SDL_Event event;
	while (!shouldQuit)
	{
		// Reset key states at the start of the frame
		inputManager.resetStates();

		while (SDL_PollEvent(&event) != 0)
		{
			// Handle window events
			switch (event.type)
			{
			case SDL_QUIT:
				shouldQuit = true;
				break;
			}

			// Handle key events
			inputManager.handleEvent(event);
		}
		
		// Clear screen
		/*
		testRenderer.clear(SDL_Color{204, 187, 100, 255});
		testRenderer.present();
		*/
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		// Update game
		game.update(deltaTime);

		// Render game
		game.render(renderer);

		// Update the screen
		SDL_RenderPresent(renderer);

		// Restart timer at the end of frame
		// Cap framerate
		do
		{
			deltaTime = timer.elapsed();
		} while (framerate != 0 && deltaTime <= (1.0f / framerate));

		timeElapsed += timer.elapsed();
		timer.reset();
	}
	win.setWindowSize(1920, 1080);

	return 0;
}