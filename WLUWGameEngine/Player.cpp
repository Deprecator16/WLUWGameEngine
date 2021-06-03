#include "Player.h"

Player::Player(Vector2 hitboxPos, Vector2 hitboxSize, WTexture* sprite, Vector2 imageCount, WWindow* window, SDL_Renderer* renderer) :
	hitbox(hitboxPos, hitboxSize),
	animation(sprite, imageCount, 0)
{
	// Initialize graphics variables
	this->sprite = sprite;
	row = 1;
	colBegin = 0;
	colEnd = 1;
	switchTime = 1.0;
	faceRight = false;

	// Initialize movement variables
	speed = 1.0;
	vel.x = 0;
	vel.y = 0;

	// Initialize state machine
	state = IDLE;
}

// State machine functions
void Player::deadState(Uint32 deltaTime)
{

}

void Player::idleState(Uint32 deltaTime)
{
	// Do physics
	if (keyA || keyD)
	{
		vel.x *= 0.998;
	}
	else
	{
		vel.x *= 0.9;
	}

	// Gravity
	// Limit gravity
	if (vel.y > 1.0)
	{
		vel.y = 1.0;
	}
	else
	{
		vel.y += (1.0 / 256.0);
	}

	// Restrict maximum movement speed
	if (vel.x > 1.0)
	{
		vel.x = 1.0;
	}
	if (vel.x < -1.0)
	{
		vel.x = -1.0;
	}

	// Set velocity to 0 when small enough
	if (abs(vel.x) < (1.0 / 256.0))
	{
		vel.x = 0.0;
	}
	
	// Detect input
	// A, D
	if (keyA)
	{
		vel.x -= (speed / 32.0);
	}
	if (keyD)
	{
		vel.x += (speed / 32.0);
	}

	// W
	if (keyW && canJump)
	{
		vel.y = -1.0;
		canJump = false;
	}

	// Do collision

	// Move hitbox
	hitbox.move(vel);

	// Switch states if required

}

void Player::walkState(Uint32 deltaTime)
{
	// Do physics

	// Gravity

	// Restrict maximum movement speed

	// Set velocity to 0 when small enough

	// Detect input

	// Do collision

	// Move hitbox

	// Switch states if required
}

void Player::runState(Uint32 deltaTime)
{

}

void Player::jumpState(Uint32 deltaTime)
{

}

void Player::crouchState(Uint32 deltaTime)
{

}

void Player::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_w:
			keyW = true;
			break;

		case SDLK_s:
			keyS = true;
			break;

		case SDLK_a:
			keyA = true;
			break;

		case SDLK_d:
			keyD = true;
			break;
		}
	}

	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_w:
			keyW = false;
			break;

		case SDLK_s:
			keyS = false;
			break;

		case SDLK_a:
			keyA = false;
			break;

		case SDLK_d:
			keyD = false;
			break;
		}
	}
}

void Player::update(Uint32 deltaTime)
{
	// State machine
	switch (state)
	{
	case DEAD:
		deadState(deltaTime);
		break;

	case IDLE:
		idleState(deltaTime);
		break;

	case WALK:
		walkState(deltaTime);
		break;

	case RUN:
		runState(deltaTime);
		break;

	case JUMP:
		jumpState(deltaTime);
		break;

	case CROUCH:
		crouchState(deltaTime);
		break;
	}

	// Update animation
	updateAnimation(deltaTime);
}

void Player::updateAnimation(Uint32 deltaTime)
{
	// Update direction based on velocity
	if (vel.x > 0.0)
	{
		faceRight = true;
	}
	else if (vel.x < 0.0)
	{
		faceRight = false;
	}

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
	//cout << animation.getClip()->w << " " << animation.getClip()->h << endl;

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

void Player::setPos(Vector2 pos)
{
	hitbox.setPos(pos);
}

void Player::setVel(Vector2 vel)
{
	this->vel.x = vel.x;
	this->vel.y = vel.y;
}

void Player::setCanJump(bool canJump)
{
	this->canJump = canJump;
}