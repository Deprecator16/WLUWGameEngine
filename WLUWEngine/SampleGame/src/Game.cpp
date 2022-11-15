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

/*
double getColliders(Hitbox* softBox, std::vector<Hitbox*> hardBoxes, std::vector<Collision>& colliders, double deltaTime)
{
	double minTimeOfImpact = INT_MAX;

	// Loop through all hard objects
	for (unsigned int i = 0 ; i < hardBoxes.size(); ++i)
	{
		Hitbox* hardBox = hardBoxes[i];

		// Prune
		// Check if there is actually movement
		if (softBox == softBox->getPredict() &&
			hardBox == hardBox->getPredict())
			continue;

		// Check if the boxes' AABBs overlap
		std::pair<Vector2, Vector2> AABB1 = softBox->getAABB();
		std::pair<Vector2, Vector2> AABB2 = hardBox->getAABB();
		if (!(AABB1.first.x + AABB1.second.x >= AABB2.first.x &&
			AABB2.first.x + AABB2.second.x >= AABB1.first.x &&
			AABB1.first.y + AABB1.second.y >= AABB2.first.y &&
			AABB2.first.y + AABB2.second.y >= AABB1.first.y))
			continue;

		// Check if a collision occurs
		std::pair<Vector2, double> mtv = softBox->predictCollision(hardBox);
		if (mtv.second == 0) // No collision
			continue;

		// A collision is guaranteed to occur
		// Get collision data and add to storage
		Collision collisionData = getCollisionData(softBox, hardBox, i, deltaTime);
		//colliders.push_back(collisionData);

		if (collisionData.timeOfImpact < minTimeOfImpact)
		{
			// New min time of impact found, reset colliders storage and add new box
			minTimeOfImpact = collisionData.timeOfImpact;
			colliders.clear();
			colliders.push_back(collisionData);
		}
		else if (collisionData.timeOfImpact == minTimeOfImpact)
		{
			// Same time of impact as current min, add box to colliders storage
			colliders.push_back(collisionData);
		}
	}

	return minTimeOfImpact;
}
*/

/*
void WLUW::SampleGame::Game::handleCollisions(double deltaTime)
{
	if (objects.size() < 2)
	{
		return;
	}

	// Trigger OnCollision callbacks for all colliding objects
	// Check every object with every other object
	for (unsigned int i = 0; i < objects.size() - 1; ++i)
	{
		// Get hitbox and update prediction box
		Hitbox* box1 = objects[i]->getHitbox();
		box1->updatePredict(deltaTime);
		for (unsigned int j = i + 1; j < objects.size(); ++j)
		{
			// Get hitbox and update prediction box
			Hitbox* box2 = objects[j]->getHitbox();
			box2->updatePredict(deltaTime);

			// Prune
			// Check if there is actually movement
			if (box1 == box1->getPredict() &&
				box2 == box2->getPredict())
				continue;

			// Check if the objects' AABBs overlap
			std::pair<Vector2, Vector2> AABB1 = box1->getAABB();
			std::pair<Vector2, Vector2> AABB2 = box2->getAABB();
			if (!(AABB1.first.x + AABB1.second.x >= AABB2.first.x &&
				AABB2.first.x + AABB2.second.x >= AABB1.first.x &&
				AABB1.first.y + AABB1.second.y >= AABB2.first.y &&
				AABB2.first.y + AABB2.second.y >= AABB1.first.y))
				continue;

			// Check if a collision occurs
			std::pair<Vector2, double> mtv = box1->predictCollision(box2);
			if (mtv.second == 0) // No collision
				continue;

			Collision collisionData;

			// Only resolve collision if one object is soft and the other is hard
			if (box1->getInertia() != box2->getInertia())
			{
				// Resolve collision
				// Get collision data
				if (objects[i]->getHitbox()->getInertia() == Hitbox::Inertia::SOFT)
					collisionData = getCollisionData(objects[i]->getHitbox(), objects[j]->getHitbox(), j, deltaTime);
				else
					collisionData = getCollisionData(objects[i]->getHitbox(), objects[j]->getHitbox(), j, deltaTime);
			}

			// Trigger OnCollide callbacks
			objects[i]->OnCollide(objects[j], collisionData);
			objects[j]->OnCollide(objects[i], collisionData);
		}
	}


	// TO DO: Implement sweep-based continuous collisiion detection
	// https://docs.unity3d.com/Manual/ContinuousCollisionDetection.html

	// Separate objects by their hitbox inertia
	std::vector<Hitbox*> softBoxes, hardBoxes;
	for (auto& object : objects)
		object->getHitbox()->getInertia() == Hitbox::SOFT ? softBoxes.push_back(object->getHitbox()) : hardBoxes.push_back(object->getHitbox());

	// Check rigid collisions for all soft objects
	for (auto& softBox : softBoxes)
	{
		// For each soft object, get a vector of all hard objects that it will collide with
		std::vector<Collision> colliders;
		double timeOfImpact = INT_MAX;

		// Find and resolve collisions until no more collisions are found
		do
		{
			// Recalculate predict
			softBox->updatePredict(deltaTime);

			// Get a list of colliders as well as the minimum time of impact
			colliders.clear();
			timeOfImpact = getColliders(softBox, hardBoxes, colliders, deltaTime);

			// No more collisions need to be resolved
			if (colliders.size() == 0)
				break;

			// Resolve collision
			// Sort colliders
			std::sort(colliders.begin(), colliders.end(), compareCollisionData);

			// Remove unnecessary collisions
			for (unsigned int i = 0; i < colliders.size(); ++i)
			{
				if (colliders[i].timeOfImpact != timeOfImpact)
				{
					colliders.erase(colliders.begin() + i);
					--i;
				}
			}
			double bestTotalDistance = colliders[0].totalDistanceFromEdgeToShape;
			for (unsigned int i = 0; i < colliders.size(); ++i)
			{
				if (colliders[i].totalDistanceFromEdgeToShape != bestTotalDistance)
				{
					colliders.erase(colliders.begin() + i);
					--i;
				}
			}

			// Find minimum distance
			Vector2 minDistance = colliders[0].distance;
			for (auto& collider : colliders)
			{
				if (collider.distance.size() < minDistance.size())
					minDistance = collider.distance;
			}

			// Move soft box based on min distance
			softBox->setPos(softBox->getPos() + minDistance);

			for (auto& collider : colliders)
			{
				// Redirect soft object velocity
				softBox->setVel((softBox->getVel() - minDistance).projectOntoAxis(collider.edge.slope()));

				//softBox->setVel((softBox->getVel() - minDistance).projectOntoAxis(collider.edge.slope()) * pow(1.0f / 99999.0f, deltaTime));
			}

			// Remove all colliders from the next loop
			for (unsigned int i = 0; i < hardBoxes.size(); ++i)
			{
				for (auto& collider : colliders)
				{
					if (hardBoxes[i] == collider.otherBox)
						hardBoxes.erase(hardBoxes.begin() + i);
				}
			}

			// Stop loop if the soft box isn't moving anymore
			if (softBox->getVel() == Vector2())
				break;

			// Check if new position clips any other objects
			if (clips(softBox, hardBoxes, deltaTime))
			{
				std::cout << "Object clips after collision resolution" << std::endl;
			}
		}
		while (colliders.size() > 0);
	}
}
*/

void WLUW::SampleGame::Game::update(double deltaTime)
{
	std::vector<WObject*> newObjects;
	Edge edge = Edge(Vector2(0.0, 0.0), Vector2(2.0, 0.0));
	Hitbox newHitbox;

	newHitbox.addPoint(Vector2(1.0, 4.0));
	newHitbox.addPoint(Vector2(3.0, 8.0));
	newHitbox.addPoint(Vector2(3.0, 12.0));
	newHitbox.addPoint(Vector2(1.0, 12.0));

	newHitbox.getPredict()->addPoint(Vector2(1.0, 4.0));
	newHitbox.getPredict()->addPoint(Vector2(3.0, 8.0));
	newHitbox.getPredict()->addPoint(Vector2(3.0, 12.0));
	newHitbox.getPredict()->addPoint(Vector2(1.0, 12.0));

	Block* newBlock = new Block(newHitbox);
	newObjects.push_back(newBlock);


	RaycastHit newHit = Physics::edgecast(newObjects, edge, Vector2(0.0, 1.0), 20.0);

	std::cout <<
		"[centroid=" << newHit.centroid << "], " <<
		"[point=" << newHit.point << "], " <<
		"[normal=" << newHit.normal << "], " <<
		"[separation=" << newHit.separation << "], " <<
		"[fraction=" << newHit.fraction << "]" << std::endl;

	for (auto& o : newObjects)
		delete o;


	// Update all objects
	for (auto& o : objects)
		o->update(deltaTime);

	// Move all objects
	for (auto& o : objects)
		o->getHitbox()->move(objects, deltaTime);
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
	item->QueryDoubleAttribute("x", cameraMin);
	// Max
	item = ele->FirstChildElement("max");
	item->QueryDoubleAttribute("x", cameraMax);
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
	item->QueryDoubleAttribute("x", &(pos.x));
	item->QueryDoubleAttribute("y", &(pos.y));

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
		item->QueryDoubleAttribute("x", &(pos.x));
		item->QueryDoubleAttribute("y", &(pos.y));
		newHitbox.setPos(pos);

		// Get points
		tinyxml2::XMLElement* point = item->FirstChildElement("point");
		while (point != nullptr)
		{
			// Read point data from file
			point->QueryDoubleAttribute("x", &(pos.x));
			point->QueryDoubleAttribute("y", &(pos.y));
			newHitbox.addPoint(pos);
			newHitbox.getPredict()->addPoint(pos);

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
