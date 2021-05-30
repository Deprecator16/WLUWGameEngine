#include "Header.h"

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

// Initialize SDL
bool init()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Initialize SDL_image
	int imageFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imageFlags) & imageFlags))
	{
		printf("SDL_image could not initialize! SDL Error: %s\n", SDL_GetError());
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

	SDL_FreeSurface(loadedImg);
	return newText;
}

int main(int argc, char* argv[])
{
	if (!init())
	{
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("WLUW Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN & SDL_WINDOW_MOUSE_FOCUS);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_Texture* texture = loadTexture("assets/loaded.png", renderer);

	// Allows the program to exit
	bool quit = false;

	// For event handling
	SDL_Event e;

	// Main game loop
	while (!quit)
	{
		// Fill the screen with a solid colour (white)
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		// Poll any user events
		while (SDL_PollEvent(&e) != 0)
		{
			// User clicks 'X' button
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
			}
		}

		SDL_RenderCopy(renderer, texture, NULL, NULL);

		// Update the screen
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}