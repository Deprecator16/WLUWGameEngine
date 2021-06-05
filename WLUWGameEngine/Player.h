#pragma once

#include "Vector2.h"
#include "WTexture.h"
#include "WAnimation.h"
#include "Hitbox.h"
#include "WWindow.h"
#include "Header.h"
#include "WObject.h"

class Player : public WObject
{
public:
	// Constructor
	Player(Vector2 hitboxPos, Vector2 hitboxSize, WTexture* sprite, Vector2 imageCount, WWindow* window, SDL_Renderer* renderer);
	~Player();

	// State machine functions
	void deadState(Uint32 deltaTime);
	void idleState(Uint32 deltaTime);
	void walkState(Uint32 deltaTime);
	void runState(Uint32 deltaTime);
	void jumpState(Uint32 deltaTime);
	void crouchState(Uint32 deltaTime);

	// Function for handling event
	void handleEvent(SDL_Event& e);
	
	// Update functions
	void updateAnimation(Uint32 deltaTime);
	void update(Uint32 deltaTime);
	void render();

	// Information functions
	Vector2 getPos();
	Vector2 getSize();
	Vector2 getVel();
	Hitbox* getHitbox();
	playerState getState();

	// Management functions
	void setPos(Vector2 pos);
	void setVel(Vector2 vel);
	void setCanJump(bool canJump);

protected:
	// Associated hitbox
	Hitbox hitbox;

	// Graphics
	WAnimation animation;
	int row;
	int colBegin;
	int colEnd;
	Uint32 switchTime;
	bool faceRight;
	SDL_Renderer* renderer;

	// Movement
	float speed;
	Vector2 vel;
	bool canJump;

	// State
	playerState state;

	// Keys
	bool keyW, keyS, keyA, keyD;
};

