#include "WObject.h"

WObject::WObject() :
	hitbox(Vector2(0, 0), Vector2(0,0))
{
	texture = new WTexture();
	shouldDraw = true;
}

WObject::WObject(WTexture* texture) :
	hitbox(Vector2(0, 0), Vector2(0, 0))
{
	this->texture = texture;
	shouldDraw = true;
}

void WObject::update()
{
}
