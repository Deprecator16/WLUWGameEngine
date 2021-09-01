#include "Vector2.h"
#include "WTexture.h"
#include "WWindow.h"
#include "WTimer.h"
#include "WAnimation.h"
#include "Hitbox.h"
#include "Block.h"
#include "Player.h"
#include "WRenderer.h"
#include "WLoader.h"
#include "Header.h"
#include "WAudioController.h"


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

void handleCollisions(Player* player, vector<Block*>* blocks, float deltaTime, SDL_Renderer* renderer)
{
	// Get player hitbox
	Hitbox* playerHitbox = player->getHitbox();

	// Loop through blocks
	size_t blocksSize = blocks->size();
	for (int i = 0; i < blocksSize; i++)
	{
		// Update player prediction box
		playerHitbox->updatePredict(deltaTime);

		// Get block hitbox and update its prediction box
		Hitbox* blockHitbox = blocks->at(i)->getHitbox();
		blockHitbox->updatePredict(deltaTime);

		// Detect incoming collision
		if (playerHitbox->predictCollision(blockHitbox))
		{
			// Information variables
			// Player variables
			Vector2 playerPos = playerHitbox->getPos();
			Vector2 playerSize = playerHitbox->getSize();
			Vector2 vel = playerHitbox->getVel();

			// Block variables
			Vector2 blockPos = blockHitbox->getPos();
			Vector2 blockSize = blockHitbox->getSize();

			// Get distance between player and block
			Vector2 dist = playerHitbox->getDistanceTo(blockHitbox);

			// Enum to store axis of collision
			Axis collisionAxis;

			// Check for single axis collision
			bool singleAxis = false;
			// Check if any side of the player is eclipsed by the block
			// Eclipsed on X axis, collision must occur on Y axis
			if ((playerPos.x                  > blockPos.x) && (playerPos.x                  < (blockPos.x + blockSize.x)) || // Left side
				((playerPos.x + playerSize.x) > blockPos.x) && ((playerPos.x + playerSize.x) < (blockPos.x + blockSize.x)))   // Right side
			{
				singleAxis = true;
				collisionAxis = Axis::Y;
			}
			// Eclipsed on Y axis, collision must occur on X axis
			if ((playerPos.y                  > blockPos.y) && (playerPos.y                  < (blockPos.y + blockSize.y)) || // Top side
				((playerPos.y + playerSize.y) > blockPos.y) && ((playerPos.y + playerSize.y) < (blockPos.y + blockSize.y)))   // Bottom side
			{
				singleAxis = true;
				collisionAxis = Axis::X;
			}

			// Check if any side of the block is eclipsed by the player
			// Eclipsed on X axis, collision must occur on Y axis
			if ((blockPos.x                 > playerPos.x) && (blockPos.x                 < (playerPos.x + playerSize.x)) || // Left side
				((blockPos.x + blockSize.x) > playerPos.x) && ((blockPos.x + blockSize.x) < (playerPos.x + playerSize.x)))   // Right side
			{
				singleAxis = true;
				collisionAxis = Axis::Y;
			}
			// Eclipsed on Y axis, collision must occur on X axis
			if ((blockPos.y                 > playerPos.y) && (blockPos.y                 < (playerPos.y + playerSize.y)) || // Top side
				((blockPos.y + blockSize.y) > playerPos.y) && ((blockPos.y + blockSize.y) < (playerPos.y + playerSize.y)))   // Bottom side
			{
				singleAxis = true;
				collisionAxis = Axis::X;
			}

			// Resolve single axis collision
			if (singleAxis)
			{
				// X
				if (collisionAxis == Axis::X)
				{
					player->setPos(Vector2(round(playerPos.x + dist.x), playerPos.y));
					player->setVel(Vector2(0.0, vel.y));
				}
				// Y
				else
				{
					player->setPos(Vector2(playerPos.x, round(playerPos.y + dist.y)));
					player->setVel(Vector2(vel.x, 0.0));

					// Reset canJump
					if ((playerHitbox->getBox()->y + playerSize.y) == blockHitbox->getBox()->y) // If touching ground
					{
						player->setCanJump(true);
					}
				}
			}
			// Collision on both axes
			else
			{
				// Get time of overlap on either axis
				float xOverlapTime = dist.x / (vel.x * deltaTime);
				float yOverlapTime = dist.y / (vel.y * deltaTime);

				// Check for overlap at the average time
				float avgTime = (xOverlapTime + yOverlapTime) / 2;

				// Collision occurs at the shorter time
				if ((playerPos.x + (avgTime * vel.x * deltaTime)) + playerSize.x >= blockPos.x &&
					blockPos.x + blockSize.x >= (playerPos.x + (avgTime * vel.x * deltaTime)) &&
					(playerPos.y + (avgTime * vel.y * deltaTime)) + playerSize.y >= blockPos.y &&
					blockPos.y + blockSize.y >= (playerPos.y + (avgTime * vel.y * deltaTime)))
				{
					// X
					if (xOverlapTime < yOverlapTime)
					{
						collisionAxis = Axis::X;
					}
					// Y
					else
					{
						collisionAxis = Axis::Y;
					}
				}
				// Collision occurs at the longer time
				else
				{
					// X
					if (xOverlapTime > yOverlapTime)
					{
						collisionAxis = Axis::X;
					}
					// Y
					else
					{
						collisionAxis = Axis::Y;
					}
				}

				// Resolve double axis collision
				// X
				if (collisionAxis == Axis::X)
				{
					player->setPos(Vector2(round(playerPos.x + dist.x), playerPos.y));
					player->setVel(Vector2(0.0, vel.y));
				}
				// Y
				else
				{
					player->setPos(Vector2(playerPos.x, round(playerPos.y + dist.y)));
					player->setVel(Vector2(vel.x, 0.0));

					// Reset canJump
					if ((playerHitbox->getBox()->y + playerSize.y) == blockHitbox->getBox()->y) // If touching ground
					{
						player->setCanJump(true);
					}
				}
			}
		}
	}
}

int main(int argc, char* argv[])
{
	//Replace with WWindow
	//SDL_Window* window = SDL_CreateWindow("WLUW Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN & SDL_WINDOW_MOUSE_FOCUS);
	WWindow window;
	SDL_Renderer* renderer = SDL_CreateRenderer(window.getWindow(), -1, 0);

	WAudioController audio;

	if (!init())
	{
		cout << "Initialization failed!" << endl;
		return 1;
	}

	//Load Music and SFX and play
	audio.loadMusic((char*)"music/Soundgarden-Smokestack-Lightning.wav", "smokestack");
	audio.playMusic("smokestack");
	audio.loadChunk((char*)"music/wilhelm.wav", "wilhelm");

	// Load media
	// Player spritesheet
	WTexture texPlayer(&window, renderer);
	if (!texPlayer.loadFromFile("assets/texPlayer.png"))
	{
		cout << "Could not load player texture!" << endl;
		return 1;
	}
	Vector2 texPlayerMap(2, 6);

	// Allows the program to exit
	bool quit = false;

	// For event handling
	SDL_Event e;


	// Initialize player object
	Player player(Vector2(64.0, 0.0), Vector2(16.0, 32.0), &texPlayer, texPlayerMap, &window, renderer);

	// Initialize blocks
	vector<Block*> blocks;

	// Testing WRenderer
	WRenderer rend(renderer);
	WObject obj[2];
	WLoader::init(&rend);
	WLoader::loadTextureFromFile(obj[0].texture, "assets/character.png");
	obj[0].hitbox = Hitbox(Vector2(100, 0), Vector2(100, 100));
	obj[0].texture->layer = 5;
	WLoader::loadTextureFromFile(obj[1].texture, "assets/character2.png");
	obj[1].texture->layer = 7;

	// Ground block
	Block ground(Vector2(0.0, 320.0), Vector2(640.0, 16.0));
	blocks.push_back(&ground);

	// Wall blocks
	Block leftWall(Vector2(0.0, 0.0), Vector2(16.0, 480.0));
	blocks.push_back(&leftWall);
	Block rightWall(Vector2(624.0, 0.0), Vector2(16.0, 480.0));
	blocks.push_back(&rightWall);

	// Ceiling blocks
	Block leftCeiling(Vector2(0.0, 272.0), Vector2(160.0, 16.0));
	blocks.push_back(&leftCeiling);
	Block rightCeiling(Vector2(480.0, 272.0), Vector2(160.0, 16.0));
	blocks.push_back(&rightCeiling);
	Block midCeiling(Vector2(280.0, 256.0), Vector2(80.0, 16.0));
	blocks.push_back(&midCeiling);

	// Initialize internal timer
	WTimer timer;
	float deltaTime = 0.0;
	float timeElapsed = 0.0;

	// Testing camera
	rend.setCameraPos(Vector2(100, 0));

	// Main game loop
	while (!quit)
	{
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
				case SDLK_0:
					audio.getPlayingMusic() ? audio.getPausedMusic() ? audio.resumeMusic() : audio.pauseMusic() : audio.playMusic("smokestack");
					break;
				case SDLK_9:
					audio.getPlayingMusic() ? audio.getPausedMusic() ? audio.resumeMusic() : audio.haltMusic(2000) : audio.playMusic("smokestack", 5000);
					break;
				case SDLK_UP:
					audio.setMusicVolume(audio.musicVolume + 0.1f);
					break;
				case SDLK_DOWN:
					audio.setMusicVolume(audio.musicVolume - 0.1f);
					break;
				case SDLK_8:
					audio.playSfx("wilhelm");
					break;
				case SDLK_7:
					audio.playSfxLooped("wilhelm", -1);
				}
			}

			player.handleEvent(e);
		}

		// Update all objects
		// Update player
		player.update(deltaTime);

		// Do collisions
		handleCollisions(&player, &blocks, deltaTime, renderer);

		// Move all objects
		// Move player hitbox
		player.getHitbox()->move(deltaTime);

		
		// Render player hitbox for testing
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0x00);
		SDL_RenderFillRect(renderer, player.getHitbox()->getBox());

		// Render block hitboxes for testing
		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00);
		// Render block hitboxes for testing
		for (int i = 0; i < blocks.size(); i++)
		{
			SDL_RenderFillRect(renderer, blocks.at(i)->getHitbox()->getBox());
		}

		// Render all objects
		// Render player
		player.render();

		rend.addObj(&obj[0]);
		rend.addObj(&obj[1]);
		rend.renderAll();

		// Update the screen
		SDL_RenderPresent(renderer);

		// Restart timer at the end of frame
		deltaTime = timer.elapsed();
		timeElapsed += timer.elapsed();
		timer.reset();
	}

	//Halt music
	audio.haltMusic();

	// Cleanup
	SDL_DestroyRenderer(renderer);
	window.~WWindow();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}