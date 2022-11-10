#include "Player.h"

#include <iostream>
#include <cmath>

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
	this->tmpVel = Vector2(0.0, 0.0);
	this->grounded = false;
}

void WLUW::SampleGame::Player::update(double deltaTime)
{
	// Get hitbox vel
	tmpVel = hitbox.getVel();

	// Handle input
	handleInput(deltaTime);

	// Do physics
	doPhysics(deltaTime);

	//std::cout << "x = " << hitbox.getPos().x << ", y = " << hitbox.getPos().y << std::endl;
	//std::cout << "x = " << hitbox.getVel().x << ", y = " << hitbox.getVel().y << std::endl;
	//std::cout << "x = " << tmpVel.x << ", y = " << tmpVel.y << std::endl;
	//std::cout << deltaTime << std::endl;

	// State machine?

	// Prime hitbox for movement
	hitbox.setVel(tmpVel);

	// Reset grounded state
	// Grounded will be set to true in OnCollide() later if player is still grounded
	grounded = false;
}

void WLUW::SampleGame::Player::handleInput(double deltaTime)
{
	// A, D
	if (inputManager->getKey(SDLK_a))
	{
		tmpVel.x -= speed * deltaTime * 2048.0f;
	}
	if (inputManager->getKey(SDLK_d))
	{
		tmpVel.x += speed * deltaTime * 2048.0f;
	}

	// W
	//if (inputManager->getKeyDown(SDLK_w) && grounded)
	if (inputManager->getKey(SDLK_w))
	{
		tmpVel.y = -1024.0f;
		grounded = false;
	}
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
	if (tmpVel.y > 1024.0f)
	{
		tmpVel.y = 1024.0f;
	}
	// On ground
	else if (grounded)
	{

	}
	// Not on ground, increase y velocity
	else
	{
		tmpVel.y += 4096.0f * deltaTime;
	}

	// Restrict maximum movement speed
	if (tmpVel.x > 512.0)
	{
		tmpVel.x = 512.0;
	}
	if (tmpVel.x < -512.0)
	{
		tmpVel.x = -512.0;
	}

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
	// Check if colliding with ground
	if (target->getHitbox()->getInertia() == Hitbox::HARD)
	{
		// Get slope of colliding edge
		double slope = (collisionData.edge.second.y - collisionData.edge.first.y) / (collisionData.edge.second.x - collisionData.edge.first.x);
		if (abs(slope) <= 1)
		{
			grounded = true;
		}
	}
}
