#pragma once

#include <vector>
#include "Player.h"
#include "SDL.h"
#include "tinyxml2.h"
#include "WObject.h"
#include "Block.h"

namespace WLUW::SampleGame
{
	class Game
	{
	public:
		class GameData
		{
		public:
			WWindow* window;
			InputManager* inputManager;
		};

		class LoadFailException {};

		// Constructors
		Game(GameData gameData);
		~Game();
		void freeObjects();

		// Updaters
		void handleCollisions(double deltaTime);
		void update(double deltaTime);
		void render(SDL_Renderer* renderer);

		// Loaders
		bool loadGame();

	private:
		WWindow* window;
		InputManager* inputManager;

		SDL_Rect* camera;
		std::vector<WObject*> objects;
	};
}

