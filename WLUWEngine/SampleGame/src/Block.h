#pragma once

#include "WObject.h"

namespace WLUW::SampleGame
{
	class Block : public WObject
	{
	public:
		// Constructors
		Block();
		Block(Hitbox* hitbox);
		Block(Hitbox* hitbox, WTexture* texture);

		void update(double deltaTime) {};
		void render(SDL_Renderer* renderer);

		// Event callbacks
		virtual void OnCollide(WObject* target, Collision collisionData) {};

	private:
		// Graphics

	};
}

