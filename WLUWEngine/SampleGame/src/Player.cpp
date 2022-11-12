#include "Player.h"

#include <iostream>
#include <cmath>
#include <algorithm>

WLUW::SampleGame::Player::Player(InputManager* inputManager) :
	inputManager{inputManager}
{
	// Initialize hitbox
	Shape newShape;
	newShape.addPoint(Vector2(0.0, 0.0));
	newShape.addPoint(Vector2(32.0, 0.0));
	newShape.addPoint(Vector2(32.0, 32.0));
	newShape.addPoint(Vector2(0.0, 32.0));
	this->hitbox = Hitbox(newShape, Hitbox::SOFT);

	// Initialize movement variables
	this->speed = 1.0;
	this->jumpHeight = 1.0;
	this->gravity = 1.0;
	this->tmpVel = Vector2(0.0, 0.0);
	this->collideBottom = false;
}

void WLUW::SampleGame::Player::update(double deltaTime)
{
	// Get hitbox vel
	tmpVel = hitbox.getVel();

	// Handle input
	handleInput(deltaTime);

	// Do physics
	doPhysics(deltaTime);

	// State machine?

	// Prime hitbox for movement
	hitbox.setVel(tmpVel);

	// Reset collision states
	collideBottom = collideTop = collideLeft = collideRight = false;

	hitbox.updatePredict(deltaTime);
	//std::cout << "Player pos: " << hitbox.getPos() << ", Player vel: " << hitbox.getVel() << ", predict pos: " << hitbox.getPredictPos() << std::endl;
	//std::cout << "x = " << tmpVel.x << ", y = " << tmpVel.y << std::endl;
	//std::cout << deltaTime << std::endl;
}

void WLUW::SampleGame::Player::handleInput(double deltaTime)
{
	// A, D
	if (inputManager->getKey(SDLK_a) && !collideLeft)
		tmpVel.x -= speed * deltaTime * 2048.0f;
	if (inputManager->getKey(SDLK_d) && !collideRight)
		tmpVel.x += speed * deltaTime * 2048.0f;

	// W
	//if (inputManager->getKeyDown(SDLK_w) && collideBottom && !collideTop)
	if (inputManager->getKey(SDLK_w))
		tmpVel.y = -1024.0f * jumpHeight;
}

void WLUW::SampleGame::Player::doPhysics(double deltaTime)
{
	// Decrease X velocity
	if ((!inputManager->getKey(SDLK_a) && !inputManager->getKey(SDLK_d)) ||
		(tmpVel.x > 0.0 && !inputManager->getKey(SDLK_d)) ||
		(tmpVel.x < 0.0 && !inputManager->getKey(SDLK_a)))
	{
		//tmpVel.x *= 0.9;
		tmpVel.x *= pow(0.03125, deltaTime);
	}
	else
	{
		//tmpVel.x *= 0.998;
		tmpVel.x *= pow(0.0625, deltaTime);
	}

	// Gravity
	// Limit gravity
	tmpVel.y = std::min(1024.0, tmpVel.y);

	// Not on ground, increase y velocity based on gravity
	if (!collideBottom)
		tmpVel.y += gravity * 4096.0f * deltaTime;

	// Restrict maximum movement speed
	tmpVel.x = std::clamp(tmpVel.x, -512.0, 512.0);
}

void WLUW::SampleGame::Player::render(SDL_Renderer* renderer)
{
	std::vector<Vector2> points = hitbox.getBox()->getPoints();
	for (int i = 0; i < points.size(); i++)
	{
		Vector2 p1 = points[i] + hitbox.getPos();
		Vector2 p2 = points[(i + 1) % points.size()] + hitbox.getPos();

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
	}
}

void WLUW::SampleGame::Player::OnCollide(WObject* target, Hitbox::CollisionData collisionData)
{
	// Get direction of collision
	if (target->getHitbox()->getInertia() == Hitbox::HARD)
	{
		if (collisionData.direction == Hitbox::Direction::TOP)
			collideTop = true;
		else if (collisionData.direction == Hitbox::Direction::BOTTOM)
			collideBottom = true;
		else if (collisionData.direction == Hitbox::Direction::LEFT)
			collideLeft = true;
		else if (collisionData.direction == Hitbox::Direction::RIGHT)
			collideRight = true;

		//std::cout << "bottom=" << collideBottom << ", top=" << collideTop << ", left=" << collideLeft << ", right=" << collideRight << std::endl;
	}
}
