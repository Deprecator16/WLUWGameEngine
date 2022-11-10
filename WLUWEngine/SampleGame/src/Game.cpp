#include "Game.h"
#include <iostream>
#include <cmath>

using namespace WLUW;

// Aliases
using Edge = std::pair<Vector2, Vector2>;

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
	{
		delete o;
	}
}

void WLUW::SampleGame::Game::update(double deltaTime)
{
	// Update all objects
	for (auto &o : objects)
	{
		o->update(deltaTime);
	}

	// Handle collisions
	handleCollisions(deltaTime);

	// Move all objects
	for (auto &o : objects)
	{
		o->getHitbox()->move(deltaTime);
	}
}

void WLUW::SampleGame::Game::render(SDL_Renderer* renderer)
{
	// Render all objects
	for (auto& o : objects)
	{
		o->render(renderer);
	}
}

// Given three collinear points p, a, b, the function checks if
// point p lies on line segment 'ab'
bool WLUW::SampleGame::Game::onSegment(Vector2 point, Edge edge)
{
	if (point.x <= std::max(edge.first.x, edge.second.x) && point.x >= std::min(edge.first.x, edge.second.x) &&
		point.y <= std::max(edge.first.y, edge.second.y) && point.y >= std::min(edge.first.y, edge.second.y))
		return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r)
WLUW::SampleGame::Game::Orientation WLUW::SampleGame::Game::orientation(Vector2 point1, Vector2 point2, Vector2 point3)
{
	int val = (point2.y - point1.y) * (point3.x - point2.x) -
		(point2.x - point1.x) * (point3.y - point2.y);

	if (val == 0) return Orientation::COLLINEAR;

	return (val > 0) ? Orientation::CLOCKWISE : Orientation::COUNTERCLOCKWISE;
}

// The main function that returns true if line segment 'p1p2'
// and 'q1q2' intersect.
bool WLUW::SampleGame::Game::areIntersecting(Edge edge1, Edge edge2)
{
	// Check for parallelism
	if (edge1.second - edge1.first == edge2.second - edge2.first ||
		edge1.second - edge1.first == edge2.first - edge2.second)
		return false;

	// Find the four orientations needed for general and special cases
	Orientation o1 = orientation(edge1.first, edge1.second, edge2.first);
	Orientation o2 = orientation(edge1.first, edge1.second, edge2.second);
	Orientation o3 = orientation(edge2.first, edge2.second, edge1.first);
	Orientation o4 = orientation(edge2.first, edge2.second, edge1.second);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, p2 and q1 are collinear and q1 lies on segment p1p2
	if (o1 == 0 && onSegment(edge2.first, std::make_pair(edge1.first, edge1.second))) return true;
	// p1, p2 and q2 are collinear and q2 lies on segment p1p2
	if (o2 == 0 && onSegment(edge2.second, std::make_pair(edge1.first, edge1.second))) return true;
	// q1, q2 and p1 are collinear and p1 lies on segment q1q2
	if (o3 == 0 && onSegment(edge1.first, std::make_pair(edge2.first, edge2.second))) return true;
	// q1, q2 and p2 are collinear and p2 lies on segment q1q2
	if (o4 == 0 && onSegment(edge1.second, std::make_pair(edge2.first, edge2.second))) return true;

	return false; // Doesn't fall in any of the above cases
}

Vector2 WLUW::SampleGame::Game::getPointOfIntersection(Edge edge1, Edge edge2)
{
	// Line 1 represented as a1x + b1y = c1
	double a1 = edge1.second.y - edge1.first.y;
	double b1 = edge1.first.x - edge1.second.x;
	double c1 = a1 * (edge1.first.x) + b1 * (edge1.first.y);

	// Line 2 represented as a2x + b2y = c2
	double a2 = edge2.second.y - edge2.first.y;
	double b2 = edge2.first.x - edge2.second.x;
	double c2 = a2 * (edge2.first.x) + b2 * (edge2.first.y);

	double determinant = a1 * b2 - a2 * b1;

	if (determinant == 0)
	{
		// The lines are parallel, no intersection
	}
	else
	{
		// Get point of intersection
		double x = (b2 * c1 - b1 * c2) / determinant;
		double y = (a1 * c2 - a2 * c1) / determinant;

		return Vector2(x, y);
	}
}

// Returns a pair with
WLUW::Hitbox::CollisionData WLUW::SampleGame::Game::getCollisionData(Hitbox* box1, Hitbox* box2, double deltaTime)
{
	Hitbox::CollisionData collisionData;
	collisionData.timeOfImpact = INT_MAX;

	// For every point, find the first edge that its movement will intersect
	for (unsigned int i = 0; i < box1->getBox()->getPoints().size(); ++i)
	{
		// Create edge using the trajectory of current point
		Vector2 p1 = box1->getBox()->getPoints()[i] + box1->getPos();
		Vector2 p2 = box1->getPredict()->getPoints()[i] + box1->getPredictPos();

		for (unsigned int j = 0; j < box2->getBox()->getPoints().size(); ++j)
		{
			// Get edge from box2
			Edge edge = std::make_pair(
				box2->getPredict()->getPoints()[j] + box2->getPredictPos(),
				box2->getPredict()->getPoints()[(j + 1) % box2->getPredict()->getPoints().size()] + box2->getPredictPos()
			);

			if (areIntersecting(std::make_pair(p1, p2), edge))
			{
				// Check if original point is colinear to edge
				if (onSegment(p1, edge))
				{
					// Update collision data with new info
					collisionData.point = p1;
					collisionData.edge = edge;
					collisionData.pointOfIntersection = p1;
					collisionData.distance = Vector2();
					collisionData.timeOfImpact = 0.0;
					break;
				}

				// Find point of intersection
				Vector2 pointOfIntersection = getPointOfIntersection(std::make_pair(p1, p2), edge);

				// Find time for original point to reach point of intersection
				double timeOfImpact;
				if (box1->getVel().x == 0.0 && box1->getVel().y == 0.0)
				{
					throw("No velocity, collision is impossible");
				}
				else if (box1->getVel().x == 0.0)
				{
					timeOfImpact = (pointOfIntersection.y - p1.y) / (box1->getVel().y * deltaTime);
				}
				else if (box1->getVel().y == 0.0)
				{
					timeOfImpact = (pointOfIntersection.x - p1.x) / (box1->getVel().x * deltaTime);
				}
				else
				{
					timeOfImpact = std::min(
						(pointOfIntersection.x - p1.x) / (box1->getVel().x * deltaTime),
						(pointOfIntersection.y - p1.y) / (box1->getVel().y * deltaTime)
					);
				}

				// Check if time of impact is lower than current min
				if (timeOfImpact < collisionData.timeOfImpact)
				{
					// Update collision data with new info
					collisionData.point = p1;
					collisionData.edge = edge;
					collisionData.pointOfIntersection = pointOfIntersection;
					collisionData.distance = pointOfIntersection - p1;
					collisionData.timeOfImpact = timeOfImpact;
				}
			}
		}
	}

	// For every edge, find the first point that its movement will intersect
	for (unsigned int i = 0; i < box1->getBox()->getPoints().size(); ++i)
	{
		// Get edge from box1
		Edge edge = std::make_pair(
			box1->getBox()->getPoints()[i] + box1->getPos(),
			box1->getBox()->getPoints()[(i + 1) % box1->getBox()->getPoints().size()] + box1->getPos()
		);

		for (unsigned int j = 0; j < box2->getBox()->getPoints().size(); ++j)
		{
			// Get point from box2
			// Create edge using the trajectory of current point by applying the negative of box1's velocity
			Vector2 p1 = box2->getBox()->getPoints()[j] + box2->getPos();
			Vector2 p2 = p1 - box1->getVel();

			if (areIntersecting(std::make_pair(p1, p2), edge))
			{
				// Check if original point is colinear to edge
				if (onSegment(p1, edge))
				{
					// Update collision data with new info
					collisionData.point = p1;
					collisionData.edge = edge;
					collisionData.pointOfIntersection = p1;
					collisionData.distance = Vector2();
					collisionData.timeOfImpact = 0.0;
					break;
				}

				// Find point of intersection
				Vector2 pointOfIntersection = getPointOfIntersection(std::make_pair(p1, p2), edge);

				// Find time for original point to reach point of intersection
				double timeOfImpact;
				if (box1->getVel().x == 0.0 && box1->getVel().y == 0.0)
				{
					throw("No velocity, collision is impossible");
				}
				else if (box1->getVel().x == 0.0)
				{
					timeOfImpact = (pointOfIntersection.y - p1.y) / -(box1->getVel().y * deltaTime);
				}
				else if (box1->getVel().y == 0.0)
				{
					timeOfImpact = (pointOfIntersection.x - p1.x) / -(box1->getVel().x * deltaTime);
				}
				else
				{
					timeOfImpact = std::min(
						(pointOfIntersection.x - p1.x) / (-box1->getVel().x * deltaTime),
						(pointOfIntersection.y - p1.y) / (-box1->getVel().y * deltaTime)
					);
				}

				// Check if time of impact is lower than current min
				if (timeOfImpact < collisionData.timeOfImpact)
				{
					// Update collision data with new info
					collisionData.point = p1;
					collisionData.edge = edge;
					collisionData.pointOfIntersection = pointOfIntersection;
					collisionData.distance = pointOfIntersection - p1;
					collisionData.timeOfImpact = timeOfImpact;
				}
			}
		}
	}
	
	/*
	std::cout << "(point: x=" << collisionData.point.x << ", y=" << collisionData.point.y <<
		"), (edge: p1.x=" << collisionData.edge.first.x << ", p1.y=" << collisionData.edge.first.y <<
		", p2.x=" << collisionData.edge.second.x << ", p2.y=" << collisionData.edge.second.y <<
		"), (POI: x=" << collisionData.pointOfIntersection.x << ", y=" << collisionData.pointOfIntersection.y <<
		"), (distance: x=" << collisionData.distance.x << ", y=" << collisionData.distance.y <<
		"), (timeOfImpact=" << collisionData.timeOfImpact << ")" << std::endl;
		*/

	return collisionData;
}

void WLUW::SampleGame::Game::handleCollisions(double deltaTime)
{
	if (objects.size() < 2)
	{
		return;
	}

	/*
	The problem: The box may get pushed into another box due to collision with one box
	*/

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

			// Check if there is actually movement
			if (box1->getBox() == box1->getPredict() &&
				box2->getBox() == box2->getPredict())
				continue;

			// Detect incoming collision
			std::pair<Vector2, double> mtv = box1->predictCollision(box2);
			if (mtv.second != 0) // Collision detected
			{
				Hitbox::CollisionData collisionData;

				// Only resolve collision if one object is soft and the other is hard
				if (box1->getInertia() != box2->getInertia())
				{
					// Resolve collision
					// Check which object is soft
					Hitbox* softBox;
					Hitbox* hardBox;
					if (box1->getInertia() == Hitbox::SOFT)
					{
						softBox = box1;
						hardBox = box2;
					}
					else
					{
						softBox = box2;
						hardBox = box1;
					}

					// Get collision data
					collisionData = getCollisionData(softBox, hardBox, deltaTime);

					// Move soft object so that it touches hard object
					softBox->setPos(softBox->getPos() + collisionData.distance);

					// Redirect soft object velocity
					softBox->setVel(softBox->getVel().projectOntoAxis(collisionData.edge.second - collisionData.edge.first));
				}

				// Trigger OnCollide callbacks
				objects[i]->OnCollide(objects[j], collisionData);
				objects[j]->OnCollide(objects[i], collisionData);
			}
		}
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
		Shape newShape;

		// Read shape data from file
		// Get position
		item->QueryDoubleAttribute("x", &(pos.x));
		item->QueryDoubleAttribute("y", &(pos.y));
		newShape.setPos(pos);

		// Get points
		tinyxml2::XMLElement* point = item->FirstChildElement("point");
		while (point != nullptr)
		{
			// Read point data from file
			point->QueryDoubleAttribute("x", &(pos.x));
			point->QueryDoubleAttribute("y", &(pos.y));
			newShape.addPoint(pos);

			// Go to next node
			point = point->NextSiblingElement("point");
		}

		// Append new block to blocks vector
		Block* newBlock = new Block(newShape);
		objects.push_back(newBlock);

		// Reset storage variables
		pos = Vector2();

		// Go to next node
		item = item->NextSiblingElement("item");
	}

	return true;
}
