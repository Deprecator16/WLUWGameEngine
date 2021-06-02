#include "Player.h"

Player::Player(Vector2 hBoxPos, Vector2 hBoxSize, SDL_Window* window, SDL_Renderer* renderer) :
	hitbox(hBoxPos, hBoxSize),
	animation(sprite, hBoxPos, 0)
{
	// Initialize graphics variables
	row = 1;
	colBegin = 0;
	colEnd = 1;
	switchTime = 1.0;
	faceRight = false;

	// Initialize movement variables
	vel.x = 0;
	vel.y = 0;
}

void Player::update(float deltaTime)
{
	// Update animation

}

void Player::updateAnimation(float deltaTime)
{
	// Update animation
	switch (state)
	{
	case DEAD:
		row = 0;
		colBegin = 0;
		colEnd = 0;
		switchTime = 0.0;
		break;

	case IDLE:
		row = 1;
		colBegin = 0;
		colEnd = 1;
		switchTime = 1.0;
		break;

	case WALK:
		row = 2;
		colBegin = 0;
		colEnd = 1;
		switchTime = 0.5;
		break;

	case RUN:
		row = 3;
		colBegin = 0;
		colEnd = 1;
		switchTime = 0.3;
		break;

	case JUMP:
		row = 4;
		colBegin = 0;
		colEnd = 0;
		switchTime = 0.0;
		break;

	case CROUCH:
		row = 5;
		colBegin = 0;
		colEnd = 0;
		switchTime = 0.0;
		break;
	}

	animation.setSwitchTime(switchTime);
	animation.update(row, colBegin, colEnd, deltaTime, faceRight);
}

void Player::render()
{
	SDL_Point center;
	center.x = hitbox.getPos().x;
	center.y = hitbox.getPos().y;
	if (faceRight) {
		sprite->render(hitbox.getPos().x, hitbox.getPos().y, animation.getClip(), 0, &center, SDL_FLIP_NONE);
	}
	else
	{
		sprite->render(hitbox.getPos().x, hitbox.getPos().y, animation.getClip(), 0, &center, SDL_FLIP_HORIZONTAL);
	}
}

Vector2 Player::getPos()
{
	return hitbox.getPos();
}

Vector2 Player::getSize()
{
	return hitbox.getSize();
}

Vector2 Player::getVel()
{
	return vel;
}

Hitbox* Player::getHitbox()
{
	return &hitbox;
}

playerState Player::getState()
{
	return state;
}