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
			// Check if the player is only moving on one axis
			// X
			if (vel.y == 0.0 && vel.x != 0.0)
			{
				singleAxis = true;
				collisionAxis = X;
			}
			// Y
			if (vel.x == 0.0 && vel.y != 0.0)
			{
				singleAxis = true;
				collisionAxis = Y;
			}

			if (!singleAxis)
			{
				// Check if any side is eclipsed
				// X
				if ((playerPos.x > blockPos.x) && (playerPos.x < (blockPos.x + blockSize.x)) || // Left side
					((playerPos.x + playerSize.x) > blockPos.x) && ((playerPos.x + playerSize.x) < (blockPos.x + blockSize.x)))   // Right side
				{
					singleAxis = true;
					collisionAxis = X;
				}
				// Y
				if ((playerPos.y > blockPos.y) && (playerPos.y < (blockPos.y + blockSize.y)) || // Top side
					((playerPos.y + playerSize.y) > blockPos.y) && ((playerPos.y + playerSize.y) < (blockPos.y + blockSize.y)))   // Bottom side
				{
					singleAxis = true;
					collisionAxis = Y;
				}
			}


			// Resolve collision
			// Single axis collision
			if (singleAxis)
			{
				// X
				if (collisionAxis == X)
				{
					if (dist.x != 0.0)
					{
						cout << "X" << " " << dist.x << endl;
					}
					player->setPos(Vector2(playerPos.x + dist.x, playerPos.y));
					player->setVel(Vector2(0.0, vel.y));
				}
				// Y
				else
				{
					if (dist.x != 0.0)
					{
						cout << "Y" << " " << dist.y << endl;
					}
					player->setPos(Vector2(playerPos.x, playerPos.y + dist.y));
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
					player->setPos(Vector2(playerPos.x + dist.x, playerPos.y));
					player->setVel(Vector2(0.0, vel.y));
				}
				// Y
				else
				{
					player->setPos(Vector2(playerPos.x, playerPos.y + dist.y));
					player->setVel(Vector2(vel.x, 0.0));

					// Reset canJump
					if ((playerHitbox->getBox()->y + playerSize.y) == blockHitbox->getBox()->y) // If touching ground
					{
						player->setCanJump(true);
					}
				}
			}








			/*
			// Determine direction of collision
			direction dir = LEFT;

			// Get time of collision on either axis
			Vector2 dist = playerHitbox->getDistanceTo(blockHitbox);
			Vector2 vel = playerHitbox->getVel();
			Vector2 playerPos = playerHitbox->getPos();
			Vector2 predictPos = playerHitbox->getPredictPos();
			Vector2 blockPos = blockHitbox->getPos();
			Vector2 axisCollisionTime = dist / (vel.x * deltaTime);

			// Check for actual collision at the average collision time to see if there is actually a collision
			float avgTime = (axisCollisionTime.x + axisCollisionTime.y) / 2;

			// Check if already touching
			bool touching = false;
			// Already touching on Y axis
			if (dist.y == 0.0)
			{
				touching = true;
			}
			// Already touching on X axis
			if (dist.x == 0.0)
			{
				touching = true;
			}

			// Check if there is only collision on one axis
			bool oneAxis = false;
			// Only X axis
			if (axisCollisionTime.y < 0.0)
			{
				oneAxis = true;
				avgTime = axisCollisionTime.x;
				axisCollisionTime.y = 0.0;
			}
			// Only Y axis
			if (axisCollisionTime.x < 0.0)
			{
				oneAxis = true;
				avgTime = axisCollisionTime.y;
				axisCollisionTime.x = 0.0;
			}

			// Check for eclipse
			bool xEclipse = false;
			bool yEclipse = false;
			// X
			if (playerPos.x > blockPos.x &&
				playerPos.x + playerHitbox->getSize().x < blockPos.x + blockHitbox->getSize().x)
			{
				xEclipse = true;
				oneAxis = true;
				avgTime = axisCollisionTime.y;
				axisCollisionTime.x = 0.0;
			}
			// Y
			if (playerPos.y > blockPos.y &&
				playerPos.y + playerHitbox->getSize().y < blockPos.y + blockHitbox->getSize().y)
			{
				yEclipse = true;
				oneAxis = true;
				avgTime = axisCollisionTime.y;
				axisCollisionTime.x = 0.0;
			}

			// Create rect to simulate player hitbox
			SDL_Rect avgBox = *(playerHitbox->getBox());
			avgBox.x += (vel.x * avgTime * deltaTime);
			avgBox.y += (vel.y * avgTime * deltaTime);

			// DRAW FOR TESTING
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
			SDL_RenderFillRect(renderer, &avgBox);

			float trueCollisionTime = 0.0;
			axis trueCollisionAxis = X;

			// Already touching
			if (touching)
			{
				// Touching on X axis
				if (dist.x == 0.0 && !xEclipse)
				{
					// Left
					if (predictPos.x < playerPos.x)
					{
						dir = LEFT;
						cout << "touching left" << endl;
					}
					// Right
					else if (predictPos.x > playerPos.x)
					{
						dir = RIGHT;
						cout << "touching right" << endl;
					}
				}
				// Touching on Y axis
				else if (dist.y == 0.0 && !yEclipse)
				{
					// Top
					if (predictPos.y < playerPos.y)
					{
						dir = TOP;
						cout << "touching top" << endl;
					}
					// Bottom
					else if (predictPos.y > playerPos.y)
					{
						dir = BOT;
						cout << "touching bot" << endl;
					}
				}
			}
			// Collision on only one axis
			else if (oneAxis)
			{
				// X
				if (axisCollisionTime.x != 0.0)
				{
					// Left
					if (predictPos.x < playerPos.x)
					{
						dir = LEFT;
						cout << "oneAxis left" << endl;
					}
					// Right
					else if (predictPos.x > playerPos.x)
					{
						dir = RIGHT;
						cout << "oneAxis right" << endl;
					}
				}
				// Y
				else if (axisCollisionTime.y != 0.0)
				{
					// Top
					if (predictPos.y < playerPos.y)
					{
						dir = TOP;
						cout << "oneAxis top" << endl;
					}
					// Bottom
					else if (predictPos.y > playerPos.y)
					{
						dir = BOT;
						cout << "oneAxis bot" << endl;
					}
				}

			}
			// Collision on both axes
			else
			{
				// Check for intersection
				// Collision at smaller time value
				if (blockHitbox->getPos().x + blockHitbox->getSize().x > avgBox.x                &&
					avgBox.x                + avgBox.w                 > blockHitbox->getPos().x &&
					blockHitbox->getPos().y + blockHitbox->getSize().y > avgBox.y                &&
					avgBox.y                + avgBox.h                 > blockHitbox->getPos().y)
				{
					trueCollisionTime = min(axisCollisionTime.x, axisCollisionTime.y);

					// Determine direction
					// X
					if (axisCollisionTime.x < axisCollisionTime.y)
					{
						// Left
						if (dist.x < 0.0)
						{
							dir = LEFT;
							cout << "smaller left" << endl;
						}
						// Right
						else if (dist.x > 0.0)
						{
							dir = RIGHT;
							cout << "smaller right" << endl;
						}
					}
					// Y
					else if (axisCollisionTime.x > axisCollisionTime.y)
					{
						// Top
						if (dist.y < 0.0)
						{
							dir = TOP;
							cout << "smaller top" << endl;
						}
						// Bottom
						else if (dist.y > 0.0)
						{
							dir = BOT;
							cout << "smaller bot" << endl;
						}
					}
				}
				// Collision at larger time value
				else
				{
					trueCollisionTime = max(axisCollisionTime.x, axisCollisionTime.y);

					// Determine direction
					// X
					if (axisCollisionTime.x > axisCollisionTime.y)
					{
						// Left
						if (dist.x < 0.0)
						{
							dir = LEFT;
							cout << "larger left " << trueCollisionTime << " " << axisCollisionTime.x << " " << axisCollisionTime.y << endl;
						}
						// Right
						else if (dist.x > 0.0)
						{
							dir = RIGHT;
							cout << "larger right" << endl;
						}
					}
					// Y
					else if (axisCollisionTime.x < axisCollisionTime.y)
					{
						// Top
						if (dist.y < 0.0)
						{
							dir = TOP;
							cout << "larger top" << endl;
						}
						// Bottom
						else if (dist.y > 0.0)
						{
							dir = BOT;
							cout << "larger bot" << endl;
						}
					}
				}
			}

			// Resolve collision
			if (dir == TOP)
			{
				// Set y vel to 0 if negative
				if (vel.y < 0.0)
				{
					player->setVel(Vector2(vel.x, 1.0));
				}

				if (!touching)
				{
					if (abs(dist.y) > 0.0)
					{
						player->setPos(Vector2(player->getPos().x, player->getPos().y - abs(dist.y)));

						dist = playerHitbox->getDistanceTo(blockHitbox);
						cout << setw(6) << "TOP" << setw(3) << i << setw(12) << dist.y << endl;
					}
				}
			}
			else if (dir == BOT)
			{
				// Set y vel to 0 if positive
				if (vel.y > 0.0)
				{
					player->setVel(Vector2(vel.x, 0.0));
				}

				if (!touching)
				{
					if (abs(dist.y) > 0.0)
					{
						player->setPos(Vector2(player->getPos().x, player->getPos().y + abs(dist.y)));

						dist = playerHitbox->getDistanceTo(blockHitbox);
						cout << setw(6) << "BOT" << setw(3) << i << setw(12) << dist.y << endl;
					}
				}

				// Reset canJump
				player->setCanJump(true);
			}
			else if (dir == LEFT)
			{
				// Set x vel to 0 if negative
				if (vel.x < 0.0)
				{
					player->setVel(Vector2(0.0, vel.y));
				}

				if (!touching)
				{
					if (abs(dist.x) > 0.0)
					{
						player->setPos(Vector2(player->getPos().x - abs(dist.x), player->getPos().y));

						dist = playerHitbox->getDistanceTo(blockHitbox);
						cout << setw(6) << "LEFT" << setw(3) << i << setw(12) << dist.x << endl;
					}
				}
			}
			else if (dir == RIGHT)
			{
				// Set x vel to 0 if positive
				if (vel.x > 0.0)
				{
					player->setVel(Vector2(0.0, vel.y));
				}

				if (!touching)
				{
					if (abs(dist.x) > 0.0)
					{
						player->setPos(Vector2(player->getPos().x + abs(dist.x), player->getPos().y));

						dist = playerHitbox->getDistanceTo(blockHitbox);
						cout << setw(6) << "RIGHT" << setw(3) << i << setw(12) << dist.x << endl;
					}
				}
			}
			*/


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