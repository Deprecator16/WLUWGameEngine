#include "Block.h"

Block::Block(Vector2 hBoxPos, Vector2 hBoxSize) :
	hitbox(hBoxPos, hBoxSize)
{

}

Hitbox* Block::getHitbox()
{
	return &hitbox;
}