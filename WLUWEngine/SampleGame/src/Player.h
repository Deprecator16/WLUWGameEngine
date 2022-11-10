#pragma once

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
		void update(double deltaTime);
		void handleInput(double deltaTime);
		void doPhysics(double deltaTime);
		void render(SDL_Renderer* renderer);

		// Event callbacks
		void OnCollide(WObject* target, Hitbox::CollisionData collisionData);

		// Getters
		Vector2 getPos() { return hitbox.getPos(); }
		Vector2 getTmpVel() { return tmpVel; }

		// Setters
		void setPos(Vector2 pos) { hitbox.setPos(pos); }
		void setTmpVel(Vector2 tmpVel) { this->tmpVel = tmpVel; }
		void setGrounded(bool grounded) { this->grounded = grounded; }

	private:
		// Movement
		float speed;
		float jumpHeight;
		Vector2 tmpVel;
		bool grounded;

		// Input manager
		InputManager* inputManager;

		// Graphics
	};
}
