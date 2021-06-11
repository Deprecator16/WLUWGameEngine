#include "Vector2.h"
#include "WTexture.h"
#include "WWindow.h"
#include "WTimer.h"
#include "WAnimation.h"
#include "Hitbox.h"
#include "Block.h"
#include "Player.h"
#include "Header.h"

// Struct for resolving collisions
struct Collision
{
	Hitbox* hitbox1;
	Hitbox* hitbox2;
	Vector2 intersection;
};

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
	// TODO: Drink some water
	// TODO: Fix your posture

	SDL_FreeSurface(loadedImg);
	return newText;
}

void detectCollisions(Player* player, vector<Block*>* blocks, float deltaTime, SDL_Renderer* renderer)
{
	// Player hitbox
	Hitbox* playerHitbox = player->getHitbox();
	playerHitbox->updatePredict(deltaTime);

	// Loop through blocks
	int blocksSize = blocks->size();
	for (int i = 0; i < blocksSize; i++)
	{
		// Update player hitbox
		playerHitbox->updatePredict(deltaTime);

		// Block hitbox
		Hitbox* blockHitbox = blocks->at(i)->getHitbox();
		blockHitbox->updatePredict(deltaTime);

		// Detect collision
		if (playerHitbox->predictCollision(blockHitbox))
		{
			// Information variables
			Vector2 playerPos = playerHitbox->getPos();
			Vector2 playerSize = playerHitbox->getSize();
			Vector2 playerPredictPos = playerHitbox->getPredictPos();
			Vector2 vel = playerHitbox->getVel();
			Vector2 blockPos = blockHitbox->getPos();
			Vector2 blockSize = blockHitbox->getSize();
			Vector2 dist = playerHitbox->getDistanceTo(blockHitbox);

			// Enum to store axis of collision
			axis collisionAxis;

			// Get time of overlap on either axis
			float xOverlapTime = dist.x / (vel.x * deltaTime);
			float yOverlapTime = dist.y / (vel.y * deltaTime);

			// Check for single axis collision
			bool singleAxis = false;
			// Check if any side of the player is eclipsed by the block
			// Eclipsed on X axis, collision must occur on Y axis
			if ((playerPos.x                  > blockPos.x) && (playerPos.x                  < (blockPos.x + blockSize.x)) || // Left side
				((playerPos.x + playerSize.x) > blockPos.x) && ((playerPos.x + playerSize.x) < (blockPos.x + blockSize.x)))   // Right side
			{
				singleAxis = true;
				collisionAxis = Y;
			}
			// Eclipsed on Y axis, collision must occur on X axis
			if ((playerPos.y                  > blockPos.y) && (playerPos.y                  < (blockPos.y + blockSize.y)) || // Top side
				((playerPos.y + playerSize.y) > blockPos.y) && ((playerPos.y + playerSize.y) < (blockPos.y + blockSize.y)))   // Bottom side
			{
				singleAxis = true;
				collisionAxis = X;
			}

			// Check if any side of the block is eclipsed by the player
			// Eclipsed on X axis, collision must occur on Y axis
			if ((blockPos.x                 > playerPos.x) && (blockPos.x                 < (playerPos.x + playerSize.x)) || // Left side
				((blockPos.x + blockSize.x) > playerPos.x) && ((blockPos.x + blockSize.x) < (playerPos.x + playerSize.x)))   // Right side
			{
				singleAxis = true;
				collisionAxis = Y;
			}
			// Eclipsed on Y axis, collision must occur on X axis
			if ((blockPos.y                 > playerPos.y) && (blockPos.y                 < (playerPos.y + playerSize.y)) || // Top side
				((blockPos.y + blockSize.y) > playerPos.y) && ((blockPos.y + blockSize.y) < (playerPos.y + playerSize.y)))   // Bottom side
			{
				singleAxis = true;
				collisionAxis = X;
			}

			// Resolve collision
			// Single axis collision
			if (singleAxis)
			{
				// X
				if (collisionAxis == X)
				{
					player->setPos(Vector2((int)(playerPos.x + dist.x), playerPos.y));
					player->setVel(Vector2(0.0, vel.y));
				}
				// Y
				else
				{
					player->setPos(Vector2(playerPos.x, (int)(playerPos.y + dist.y)));
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
				// Check for overlap at the average time
				float avgTime = (xOverlapTime + yOverlapTime) / 2;
				float collisionTime = 0.0;
				// Collision occurs at the shorter time
				if ((playerPos.x + (avgTime * vel.x * deltaTime)) + playerSize.x >= blockPos.x &&
					blockPos.x + blockSize.x >= (playerPos.x + (avgTime * vel.x * deltaTime)) &&
					(playerPos.y + (avgTime * vel.y * deltaTime)) + playerSize.y >= blockPos.y &&
					blockPos.y + blockSize.y >= (playerPos.y + (avgTime * vel.y * deltaTime)))
				{
					collisionTime = min(xOverlapTime, yOverlapTime);

					// X
					if (xOverlapTime < yOverlapTime)
					{
						collisionAxis = X;
					}
					// Y
					else
					{
						collisionAxis = Y;
					}
				}
				// Collision occurs at the longer time
				else
				{
					collisionTime = max(xOverlapTime, yOverlapTime);

					// X
					if (xOverlapTime > yOverlapTime)
					{
						collisionAxis = X;
					}
					// Y
					else
					{
						collisionAxis = Y;
					}
				}

				// Resolve collision
				// X
				if (collisionAxis == X)
				{
					player->setPos(Vector2((int)(playerPos.x + dist.x), playerPos.y));
					player->setVel(Vector2(0.0, vel.y));
				}
				// Y
				else
				{
					player->setPos(Vector2(playerPos.x, (int)(playerPos.y + dist.y)));
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

	if (!init())
	{
		cout << "Initialization failed!" << endl;
		return 1;
	}

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
	Player player(Vector2(64.0, 0.0), Vector2(32.0, 32.0), &texPlayer, texPlayerMap, &window, renderer);

	// Initialize blocks
	vector<Block*> blocks;

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
				}
			}

			player.handleEvent(e);
		}

		// Update player
		player.update(deltaTime);

		// Do collisions
		detectCollisions(&player, &blocks, deltaTime, renderer);

		// Move hitboxes
		player.getHitbox()->move(deltaTime);

		/*
		if (player.getHitbox()->isColliding(ground.getHitbox()))
		{
			player.getHitbox()->move(Vector2(-deltaTime, -deltaTime) * player.getVel());

			// Reset player velocity
			player.setVel(Vector2(player.getVel().x, 0.0));

			player.setCanJump(true);
		}
		*/
		
		// Render player hitbox for testing
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0x00);
		SDL_RenderFillRect(renderer, player.getHitbox()->getBox());

		// Render player prediction hitbox for testing
		/*
		fillRect.x = player.getHitbox()->getPredictPos().x;
		fillRect.y = player.getHitbox()->getPredictPos().y;
		fillRect.w = player.getHitbox()->getPredict()->w;
		fillRect.h = player.getHitbox()->getPredict()->h;
		*/
		//SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
		//SDL_RenderFillRect(renderer, player.getHitbox()->getPredict());

		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00);
		// Render block hitboxes for testing
		for (int i = 0; i < blocks.size(); i++)
		{
			SDL_RenderFillRect(renderer, blocks.at(i)->getHitbox()->getBox());
		}

		// Render player
		//player.render();

		// Update the screen
		SDL_RenderPresent(renderer);

		// Restart timer at the end of frame
		deltaTime = timer.elapsed();
		timeElapsed += timer.elapsed();
		timer.reset();
	}

	// Cleanup
	SDL_DestroyRenderer(renderer);
	window.~WWindow();
	IMG_Quit();
	SDL_Quit();

	return 0;
}