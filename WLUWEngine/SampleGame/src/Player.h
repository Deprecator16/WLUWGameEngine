#pragma once

#include "Collision.h"
#include "Hitbox.h"
#include "SDL.h"
#include "Shape.h"
#include "WObject.h"
#include "InputManager.h"

class WWindow;

namespace WLUW::SampleGame
{
	class Player : public WObject
	{
	public:
		// Constructors
		Player(InputManager* inputManager);

		// Updaters
		void update(float deltaTime);
		void handleInput(float deltaTime);
		void doPhysics(float deltaTime);
		void render(SDL_Renderer* renderer);

		// Event callbacks
		void OnCollide(WObject* other, Collision collision);

		// Getters
		Vector2 getPos() { return hitbox.getPos(); }
		Vector2 getTmpVel() { return tmpVel; }

		// Setters
		void setPos(Vector2 pos) { hitbox.setPos(pos); }
		void setTmpVel(Vector2 tmpVel) { this->tmpVel = tmpVel; }
		void setGrounded(bool collideBottom) { this->collideBottom = collideBottom; }

	private:
		// Movement
		float speed;
		float jumpHeight;
		float gravity;
		Vector2 tmpVel;
		bool collideBottom;
		bool collideTop;
		bool collideLeft;
		bool collideRight;

		// Input manager
		InputManager* inputManager;

		// Graphics
	};
}

