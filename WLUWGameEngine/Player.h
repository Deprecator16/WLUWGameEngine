#pragma once

#include "Vector2.h"
#include "WTexture.h"
#include "WAnimation.h"
#include "Hitbox.h"
#include "WWindow.h"

class Player
{
public:
	// Constructor
	Player(Vector2 hitboxPos, Vector2 hitboxSize, WTexture* sprite, WWindow* window, SDL_Renderer* renderer);

	// State machine functions
	void deadState(float deltaTime);
	void idleState(float deltaTime);
	void walkState(float deltaTime);
	void runState(float deltaTime);
	void jumpState(float deltaTime);
	void crouchState(float deltaTime);

	// Function for handling event
	void handleEvent(SDL_Event& e);
	
	// Update functions
	void updateAnimation(float deltaTime);
	void update(float deltaTime);
	void render();

	// Information functions
	Vector2 getPos();
	Vector2 getSize();
	Vector2 getVel();
	Hitbox* getHitbox();
	playerState getState();

private:
	// Associated hitbox
	Hitbox hitbox;

	// Graphics
	WTexture* sprite;
	WAnimation animation;
	int row;
	int colBegin;
	int colEnd;
	float switchTime;
	bool faceRight;

	// Movement
	Vector2 vel;

	// State
	playerState state;

	// Keys
	bool keyW, keyS, keyA, keyD;
};

