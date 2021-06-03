#include "Vector2.h"
#include "WTexture.h"
#include "WWindow.h"
#include "WTimer.h"
#include "WAnimation.h"
#include "Hitbox.h"
#include "Block.h"
#include "Player.h"
#include "Header.h"


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
	//Replace with WWindow
	//SDL_Window* window = SDL_CreateWindow("WLUW Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN & SDL_WINDOW_MOUSE_FOCUS);
	WWindow window;
	SDL_Renderer* renderer = SDL_CreateRenderer(window.getWindow(), -1, 0);

	if (!init())
	{
		cout << "Initialization failed!" << endl;
		return 1;
	}

	// Load media
	SDL_Texture* texture = loadTexture("assets/loaded.png", renderer);
	WTexture texPlayer(&window, renderer);
	if (!texPlayer.loadFromFile("assets/texPlayer.png"))
	{
		cout << "Could not load player texture!" << endl;
		return 1;
	}

	// Allows the program to exit
	bool quit = false;

	// For event handling
	SDL_Event e;

	// Player object
	Vector2 hitboxPos;
	hitboxPos.x = 0.0;
	hitboxPos.y = 0.0;
	Vector2 hitboxSize;
	hitboxSize.x = 32.0;
	hitboxSize.y = 32.0;
	Vector2 imageCount;
	imageCount.x = 2;
	imageCount.y = 6;

	// Initialize player object
	Player player(hitboxPos, hitboxSize, &texPlayer, imageCount, &window, renderer);

	// Ground block
	hitboxPos.x = 0.0;
	hitboxPos.y = 320.0;
	hitboxSize.x = 640.0;
	hitboxSize.y = 16.0;

	Block ground(hitboxPos, hitboxSize);

	// Initialize internal timer
	WTimer timer;
	Uint32 timeElapsed = 0;

	// Main game loop
	while (!quit)
	{
		// Restart timer at the beginning of frame
		timeElapsed += timer.getTicks();
		timer.stop();
		timer.start();

		// Fill the screen with a solid colour (white)
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		// Poll any user events
		while (SDL_PollEvent(&e) != 0)
		{
			// User clicks 'X' exit button
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

			player.handleEvent(e);
		}

		// Update player
		player.update(timer.getTicks());

		// Do collisions
		if (player.getHitbox()->checkOverlap(ground.getHitbox()))
		{
			Vector2 correct;
			correct.x = -player.getVel().x;
			correct.y = -player.getVel().y;
			player.getHitbox()->move(correct);

			// Reset player velocity
			correct.x = player.getVel().x;
			correct.y = 0;
			player.setVel(correct);

			player.setCanJump(true);
		}

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		
		// Draw ground block hitbox for testing
		SDL_Rect fillRect =
		{
			ground.getHitbox()->getPos().x,
			ground.getHitbox()->getPos().y,
			ground.getHitbox()->getSize().x,
			ground.getHitbox()->getSize().y
		};
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0x00);
		SDL_RenderFillRect(renderer, &fillRect);

		// Render player
		player.render();

		// Update the screen
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	SDL_DestroyRenderer(renderer);
	window.~WWindow();
	IMG_Quit();
	SDL_Quit();

	return 0;
}