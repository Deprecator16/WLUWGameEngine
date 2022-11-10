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

		enum Orientation
		{
			COLLINEAR = 0,
			CLOCKWISE,
			COUNTERCLOCKWISE
		};

		class LoadFailException {};

		// Constructors
		Game(GameData gameData);
		~Game();
		void freeObjects();

		// Updaters
		void update(double deltaTime);
		void render(SDL_Renderer* renderer);

		// Collision handling
		Orientation orientation(Vector2 point1, Vector2 point2, Vector2 point3);
		bool onSegment(Vector2 point, std::pair<Vector2, Vector2> edge);
		bool areIntersecting(std::pair<Vector2, Vector2> edge1, std::pair<Vector2, Vector2> edge2);
		Vector2 getPointOfIntersection(std::pair<Vector2, Vector2> edge1, std::pair<Vector2, Vector2> edge2);
		Hitbox::CollisionData getCollisionData(Hitbox* box1, Hitbox* box2, double deltaTime);
		void handleCollisions(double deltaTime);

		// Loaders
		bool loadGame();

	private:
		WWindow* window;
		InputManager* inputManager;

		SDL_Rect* camera;
		std::vector<WObject*> objects;
	};
}

