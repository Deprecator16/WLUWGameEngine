#include "Game.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <unordered_map>

using namespace WLUW;

WLUW::SampleGame::Game::Game(GameData gameData) :
	window{gameData.window},
	inputManager{gameData.inputManager}
{
	try
	{
		loadGame();
	}
	catch (LoadFailException e)
	{

	}
}

WLUW::SampleGame::Game::~Game()
{
	freeObjects();
	objects.clear();
}

void WLUW::SampleGame::Game::freeObjects()
{
	for (auto& o : objects)
		delete o;
}

void WLUW::SampleGame::Game::update(float deltaTime)
{
	/*
	float dist1 = 10.0;
	float dist2 = 20.0;
	Shape newShape1(Vector2(0.0, 0.0));
	newShape1.addPoint(Vector2(0.0, 0.0));
	newShape1.addPoint(Vector2(dist1, dist1));
	newShape1.addPoint(Vector2(dist1, dist1 + 2.0));
	newShape1.addPoint(Vector2(0.0, 2.0));
	Shape newShape2(Vector2(-2.0, 0.0));
	newShape2.addPoint(Vector2(0.0, 0.0));
	newShape2.addPoint(Vector2(dist2, dist2));
	newShape2.addPoint(Vector2(0.0, dist2));
	std::pair<Vector2, float> mtv = Shape::checkCollision(newShape1, newShape2);
	if (mtv.second != 0.0) // No collision
		std::cout << "COLLIISION DETECTED: " << mtv.first << " " << mtv.second << std::endl;
	else
		std::cout << "ALL GOOD" << std::endl;
		*/

	/*
	std::vector<WObject*> newObjects;

	Shape newShape(Vector2(0.0, 0.0));
	newShape.addPoint(Vector2(0.0, 0.0));
	newShape.addPoint(Vector2(1.0, -1.0));
	newShape.addPoint(Vector2(2.0, 0.0));
	newShape.addPoint(Vector2(2.0, 2.0));
	newShape.addPoint(Vector2(0.0, 2.0));

	Hitbox newHitbox;
	newHitbox.addPoint(Vector2(0.0, 8.0));
	newHitbox.addPoint(Vector2(0.0, 12.0));
	newHitbox.addPoint(Vector2(2.0, 12.0));

	Block* newBlock = new Block(newHitbox);
	newObjects.push_back(newBlock);

	//std::vector<ContactPoint> contacts = Physics::shapecastContacts(newBlock, newShape, Vector2(0.0, 1.0), 20.0);
	std::vector<ContactPoint> contacts = Physics::getContactPoints(&newShape, newBlock->getHitbox(), Vector2(0.0, 1.0), 20.0);

	std::cout << "==================================================" << std::endl;
	for (auto& contact : contacts)
	{
		std::cout <<
			"[point=" << contact.point << "], " <<
			"[normal=" << contact.normal << "], " <<
			"[separation=" << contact.separation << "], " <<
			"[fraction=" << contact.fraction << "], " <<
			"[contactType=" << contact.contactType << "]" <<
			std::endl;
	}

	for (auto& o : newObjects)
		delete o;
		*/


	std::cout << "NEW FRAME ====================================================================================================" << std::endl;

	// Update all objects
	for (auto& o : objects)
		o->update(deltaTime);

	// Handle collisions
	for (auto& o : objects)
		o->getHitbox()->handleCollisions(objects, deltaTime);

	// Move all objects
	for (auto& o : objects)
		o->getHitbox()->move(objects, deltaTime);
		//o->getHitbox()->move(deltaTime);
}

void WLUW::SampleGame::Game::render(SDL_Renderer* renderer)
{
	// Render all objects
	for (auto& o : objects)
	{
		o->render(renderer);
	}
}

bool WLUW::SampleGame::Game::loadGame()
{
	// Clear all objects
	freeObjects();
	objects.clear();

	// Storage variables
	Vector2 pos;

	// Open file and initialize nodes
	tinyxml2::XMLDocument doc;
	doc.LoadFile("assets/levels/test.xml");
	/*
	switch (level)
	{
	case 0:
		doc.LoadFile("assets/levels/test.xml");
		break;
	}
	*/
	tinyxml2::XMLNode* root = doc.NewElement("root");
	tinyxml2::XMLElement* ele = doc.NewElement("ele");
	tinyxml2::XMLElement* item = doc.NewElement("item");

	// Root node
	root = doc.FirstChild();
	if (!root)
	{
		std::cout << "Could not load root node" << std::endl;
		throw LoadFailException();
		return false;
	}

	/*
	// Camera
	ele = root->FirstChildElement("camera");
	if (!ele)
	{
		std::cout << "Could not load camera data" << std::endl;
		throw LoadFailException();
		return false;
	}
	// Min
	item = ele->FirstChildElement("min");
	item->QueryFloatAttribute("x", cameraMin);
	// Max
	item = ele->FirstChildElement("max");
	item->QueryFloatAttribute("x", cameraMax);
	*/

	// Load player
	ele = root->FirstChildElement("player");
	if (!ele)
	{
		std::cout << "Could not load player data" << std::endl;
		throw LoadFailException();
		return false;
	}
	// Initialize new player object
	Player* player = new Player(inputManager);

	item = ele->FirstChildElement("item");
	item->QueryFloatAttribute("x", &(pos.x));
	item->QueryFloatAttribute("y", &(pos.y));

	player->setPos(pos);
	objects.push_back(player);

	// Reset storage variables
	pos = Vector2();

	// Load blocks
	ele = root->FirstChildElement("blocks");
	if (!ele)
	{
		std::cout << "Could not load blocks data" << std::endl;
		throw LoadFailException();
		return false;
	}
	item = ele->FirstChildElement("item");
	while (item != nullptr)
	{
		Hitbox newHitbox;

		// Read shape data from file
		// Get position
		item->QueryFloatAttribute("x", &(pos.x));
		item->QueryFloatAttribute("y", &(pos.y));
		newHitbox.setPos(pos);

		// Get points
		tinyxml2::XMLElement* point = item->FirstChildElement("point");
		while (point != nullptr)
		{
			// Read point data from file
			point->QueryFloatAttribute("x", &(pos.x));
			point->QueryFloatAttribute("y", &(pos.y));
			newHitbox.addPoint(pos);

			// Go to next node
			point = point->NextSiblingElement("point");
		}

		// Append new block to blocks vector
		Block* newBlock = new Block(newHitbox);
		objects.push_back(newBlock);

		// Reset storage variables
		pos = Vector2();

		// Go to next node
		item = item->NextSiblingElement("item");
	}

	return true;
}
