#include "Block.h"

WLUW::SampleGame::Block::Block()
{
	this->hitbox.setLinkedObj(this);
}

WLUW::SampleGame::Block::Block(Hitbox& hitbox)
{
	this->hitbox = hitbox;
	this->hitbox.setLinkedObj(this);
}

WLUW::SampleGame::Block::Block(Hitbox& hitbox, WTexture* texture)
{
	this->hitbox = hitbox;
	this->hitbox.setLinkedObj(this);
	this->texture = texture;
}

void WLUW::SampleGame::Block::render(SDL_Renderer* renderer)
{
	std::vector<Vector2> points = hitbox.getPoints();
	for (int i = 0; i < points.size(); i++)
	{
		Vector2 p1 = points[i] + hitbox.getPos();
		Vector2 p2 = points[(i + 1) % points.size()] + hitbox.getPos();

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
	}
}
