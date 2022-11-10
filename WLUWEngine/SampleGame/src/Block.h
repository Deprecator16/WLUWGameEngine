#pragma once

#include "WObject.h"

namespace WLUW::SampleGame
{
	class Block : public WObject
	{
	public:
		// Constructors
		Block();
		Block(Shape shape);
		Block(Shape shape, WTexture* texture);
		Block(Hitbox hitbox);
		Block(Hitbox hitbox, WTexture* texture);

		void update(double deltaTime) {};
		void render(SDL_Renderer* renderer);

		// Event callbacks
		virtual void OnCollide(WObject* target, Hitbox::CollisionData collisionData) {};

	private:
		// Graphics

	};
}

