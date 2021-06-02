#include "Block.h"

Block::Block(Vector2 hBoxPos, Vector2 hBoxSize) :
	hBox(hBoxPos, hBoxSize)
{

}

Hitbox* Block::getHBox()
{
	return &hBox;
}