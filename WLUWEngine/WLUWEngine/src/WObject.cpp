#include "WObject.h"

WLUW::WObject::WObject() :
	shouldDraw{ true },
	isUIElement{ false },
	texture{ nullptr }
{
}

WLUW::WObject::WObject(const WObject& obj) :
	shouldDraw{ obj.shouldDraw },
	isUIElement{ obj.isUIElement },
	texture{ obj.texture },
	hitbox{ obj.hitbox }
{
}

WLUW::WObject::WObject(WObject&& obj) noexcept :
	shouldDraw{ obj.shouldDraw },
	isUIElement{ obj.isUIElement },
	texture{ obj.texture },
	hitbox{ obj.hitbox }
{
}

WLUW::WObject& WLUW::WObject::operator=(const WObject& other)
{
	this->shouldDraw = other.shouldDraw;
	this->isUIElement = other.isUIElement;
	this->texture = other.texture;
	this->hitbox = other.hitbox;

	return *this;
}

WLUW::WObject& WLUW::WObject::operator=(WObject&& other) noexcept
{
	this->shouldDraw = other.shouldDraw;
	this->isUIElement = other.isUIElement;
	this->texture = other.texture;
	std::swap(this->hitbox, other.hitbox);

	return *this;
}
