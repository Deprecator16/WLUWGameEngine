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
	{
		delete o;
	}
}

/**
 * \brief Helper function that returns the time of impact (0 to 1) between two points given the first point's velocity
 *
 * \param point1: First point
 * \param point2: Second point
 * \param vel: Velocity of first point
 * \return The time of impact (0 to 1) between point1 and point2 given point1's velocity
 */
double getTimeOfImpact(Vector2 point1, Vector2 point2, Vector2 vel)
{
	double timeOfImpact;

	if (vel.size() == 0.0)
		return 0.0;
	//return std::clamp((point2 - point1).size() / vel.size(), 0.0, 1.0);


	if (vel.x == 0.0)
		timeOfImpact = abs(point2.y - point1.y) / vel.y;
	else if (vel.y == 0.0)
		timeOfImpact = abs(point2.x - point1.x) / vel.x;
	else
		timeOfImpact = std::max(abs(point2.x - point1.x) / vel.x, abs(point2.y - point1.y) / vel.y);

	// Return clamped value to prevent errors
	return std::clamp(timeOfImpact, 0.0, 1.0);
}

/**
 * \brief Helper function that returns the time of impact (As  a ratio to vel) of the center of the shape to the edge as an extended line segment
 *
 * \param center: Point representing the center of a soft box
 * \param edge: Edge that will be extended
 * \param vel: Velocity of soft box
 * \return The time of impact (As a ratio to vel) of the center point and the edge as an extended line segment
 */
double getTimeOfImpactOfCenter(Vector2 center, Edge edge, Vector2 vel)
{
	if (vel.size() == 0.0)
		return 0.0;
	return (Edge::getPointOfIntersection(Edge(center, center + vel), edge) - center).size() / vel.size();
}

/**
 * \brief Helper function that returns the closest distance from point to an edge
 *
 * \param point: Sepcified point
 * \param edge: Specified edge
 * \return The length of the smallest line segment connecting the point and the edge
 */
double getDistanceFromPointToEdge(Vector2 point, Edge edge)
{
	const double lengthSquared = pow((edge.second - edge.first).size(), 2);

	// Check if edge start and end are the same
	if (lengthSquared == 0.0)
		return (edge.first - point).size();

	// Consider the line extending the segment, parameterized as edge.first + (edge.second - edge.first) * t.
	// We find projection of the point onto the line. 
	// It falls where t = [(point - edge.first) . (edge.second - edge.first)] / |edge.second - edge.first|^2
	// We clamp t from [0,1] to handle points outside the edge.
	const double t = std::max(0.0, std::min(1.0, (point - edge.first).dot(edge.second - edge.first) / lengthSquared));
	const Vector2 projection = edge.first + (edge.second - edge.first) * t;  // Projection falls on the segment
	return (projection - point).size();
}

/**
 * \brief Helper function that returns the total distance from an edge to a shape
 *
 * \param edge: Sepcified edge
 * \param shape: Specified shape
 * \return The sum of the distance between the provided edge and every point in the provided shape
 */
double getTotalDistanceFromEdgeToShape(Edge edge, Shape* shape)
{
	double totalDistance = 0.0;

	for (auto& point : shape->getPoints())
		totalDistance += getDistanceFromPointToEdge(shape->getPos() + point, edge);

	return totalDistance;
}

/**
 * \brief Helper function that returns the total distance from a point to a shape
 *
 * \param point: Sepcified point
 * \param shape: Specified shape
 * \return The sum of the distance between the provided point and every point in the provided shape
 */
double getTotalDistanceFromPointToShape(Vector2 point, Shape* shape)
{
	double totalDistance = 0.0;

	for (auto& shapePoint : shape->getPoints())
		totalDistance += abs((shapePoint - point).size());

	return totalDistance;
}

/**
 * \brief Helper function that returns the area of a triangle
 *
 * \param a, b, c: The points of the triangle
 * \return The area of triangle abc
 */
double getTriangleArea(Vector2 a, Vector2 b, Vector2 c)
{
	return ((c.x * b.y - b.x * c.y) - (c.x * a.y - a.x * c.y) + (b.x * a.y - a.x * b.y)) / 2.0;
}

/**
 * \brief Helper function that returns whether a point is inside a square
 *
 * \param point: Specified point
 * \param a, b, c, d: Points of the square
 * \return True if the point is inside square abcd
 */
bool inSquare(Vector2 point, Vector2 a, Vector2 b, Vector2 c, Vector2 d)
{
	// Get triangle areas
	double areaABP = getTriangleArea(a, b, point);
	double areaBCP = getTriangleArea(b, c, point);
	double areaCDP = getTriangleArea(c, d, point);
	double areaDAP = getTriangleArea(d, a, point);

	// Check signs of triangles
	if (areaABP >= 0.0 && areaBCP >= 0.0 && areaCDP >= 0.0 && areaDAP >= 0.0)
		return true;
	if (areaABP <= 0.0 && areaBCP <= 0.0 && areaCDP <= 0.0 && areaDAP <= 0.0)
		return true;

	return false;
}

/**
 * \brief Helper function that returns the direction of impact between a colliding box and edge
 *
 * \param box: Colliding hitbox
 * \param edge: Colliding edge
 * \param pointOfIntersection: The point at which the hitbox and edge collide
 * \return The direction of impact between box and edge at pointOfIntersection
 */
Hitbox::Direction getDirectionOfImpact(Hitbox* box, Edge edge, Vector2 pointOfIntersection)
{
	// Get slope of colliding edge
	Hitbox::Direction direction = Hitbox::Direction::NO_DIRECTION;

	if (abs(edge.slope().x) >= abs(edge.slope().y) && pointOfIntersection.y > box->getBox()->getCenter().y)
		direction = Hitbox::Direction::BOTTOM;
	else if (abs(edge.slope().x) >= abs(edge.slope().y) && pointOfIntersection.y < box->getBox()->getCenter().y)
		direction = Hitbox::Direction::TOP;
	else if (abs(edge.slope().x) < abs(edge.slope().y) && pointOfIntersection.x > box->getBox()->getCenter().x)
		direction = Hitbox::Direction::RIGHT;
	else if (abs(edge.slope().x) < abs(edge.slope().y) && pointOfIntersection.x < box->getBox()->getCenter().x)
		direction = Hitbox::Direction::LEFT;

	return direction;
}

/**
 * \brief Comparison function to sort collision data
 *
 * \param a, b: Collision data objects being compared
 * \return True if a should be placed before b
 */
bool compareCollisionData(const Hitbox::CollisionData& a, const Hitbox::CollisionData& b)
{
	if (a.timeOfImpact < b.timeOfImpact) return true;
	if (a.timeOfImpact > b.timeOfImpact) return false;

	if (a.collisionType == Hitbox::CollisionType::EDGE_EDGE && b.collisionType != Hitbox::CollisionType::EDGE_EDGE) return true;
	if (a.collisionType != Hitbox::CollisionType::EDGE_EDGE && b.collisionType == Hitbox::CollisionType::EDGE_EDGE) return false;

	return a.totalDistanceFromEdgeToShape < b.totalDistanceFromEdgeToShape;
}

/**
 * \brief Helper function that returns collision data of the collision between a soft hitbox and a hard hitbo
 *
 * \param softBox: Moving soft hitbox
 * \param hardBox: Stationary hard hitbox
 * \param deltaTime: deltaTime for velocity calculations
 * \return CollisionData object with details of the collision between softBox and hardBox (See Hitbox.h for details)
 */
Hitbox::CollisionData getCollisionData(Hitbox* softBox, Hitbox* hardBox, unsigned int hardBoxId, double deltaTime)
{
	// Get a list of all edges and points that the soft box will collide with
	std::vector<Hitbox::CollisionData> colliders;
	double minTimeOfImpact = INT_MAX;

	// Loop through all points
	for (unsigned int i = 0; i < hardBox->getBox()->getPoints().size(); ++i)
	{
		// Get data from hard box
		Vector2 hardPoint = hardBox->getPos() + hardBox->getBox()->getPoints()[i];
		Edge hardEdge = Edge(
			hardBox->getPos() + hardBox->getBox()->getPoints()[i],
			hardBox->getPos() + hardBox->getBox()->getPoints()[(i + 1) % hardBox->getBox()->getPoints().size()]
		);

		// Check for edge-to-edge collisions
		for (unsigned int j = 0; j < softBox->getBox()->getPoints().size(); ++j)
		{
			// Create edge using the trajectory of current point
			Vector2 point1 = softBox->getPos() + softBox->getBox()->getPoints()[j];
			Vector2 point1Predict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[j];
			Vector2 point2 = softBox->getPos() + softBox->getBox()->getPoints()[(j + 1) % softBox->getBox()->getPoints().size()];
			Vector2 point2Predict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[(j + 1) % softBox->getBox()->getPoints().size()];
			Vector2 average = (point1 + point2) / 2;
			Vector2 averagePredict = (point1Predict + point2Predict) / 2;

			Edge softEdge = Edge(point1, point2);

			Edge hardEdgePredict = Edge(
				hardEdge.first + (-softBox->getVel() * deltaTime),
				hardEdge.second + (-softBox->getVel() * deltaTime)
			);

			// Check if slopes are equal
			if (softEdge.slope().normalized() != hardEdge.slope().normalized() && softEdge.slope().normalized() != -hardEdge.slope().normalized())
				continue;

			// Check if any points will intersect the opposite edge and get point of collision on soft box
			Vector2 pointOfCollisionOnSoftBox;
			if (Edge::areIntersecting(Edge(point1, point1Predict), hardEdge))
				pointOfCollisionOnSoftBox = point1;
			else if (Edge::areIntersecting(Edge(point2, point2Predict), hardEdge))
				pointOfCollisionOnSoftBox = point2;
			else if (Edge::areIntersecting(Edge(hardEdge.first, hardEdgePredict.first), softEdge))
				pointOfCollisionOnSoftBox = Edge::getPointOfIntersection(Edge(hardEdge.first, hardEdgePredict.first), softEdge);
			else if (Edge::areIntersecting(Edge(hardEdge.second, hardEdgePredict.second), softEdge))
				pointOfCollisionOnSoftBox = Edge::getPointOfIntersection(Edge(hardEdge.second, hardEdgePredict.second), softEdge);
			else
				continue;

			// Get total distance from edge to shape
			double totalDistanceFromEdgeToShape = getTotalDistanceFromEdgeToShape(hardEdge, softBox->getBox());
			//double totalDistanceFromEdgeToShape = getTimeOfImpactOfCenter(softBox->getBox()->getCenter(), hardEdge, softBox->getVel());

			// Check if point is collinear to edge
			if (hardEdge.onSegment(point1) || hardEdge.onSegment(point2) || softEdge.onSegment(hardEdge.first) || softEdge.onSegment(hardEdge.second))
			{
				// Clear colliders if we have a new minimum time of impact
				if (minTimeOfImpact != 0.0)
					colliders.clear();

				// Add new collider
				colliders.push_back(Hitbox::CollisionData(hardBoxId, average, hardEdge, average, Vector2(), 0.0, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, hardEdge, average), Hitbox::CollisionType::EDGE_EDGE));
				minTimeOfImpact = 0.0;
			}

			// If we've already found a collision on segment, then we can ignore all other possible collisions
			if (minTimeOfImpact == 0.0)
				continue;

			// Find point of intersection, time of impact, and distance
			Vector2 pointOfIntersection;
			double timeOfImpact = INT_MAX;
			Vector2 distance;
			if (Edge::areIntersecting(Edge(point1, point1Predict), hardEdge))
			{
				pointOfIntersection = Edge::getPointOfIntersection(Edge(point1, point1Predict), hardEdge);
				timeOfImpact = getTimeOfImpact(point1, pointOfIntersection, softBox->getVel());
				distance = pointOfIntersection - point1;
			}
			else if (Edge::areIntersecting(Edge(point2, point2Predict), hardEdge))
			{
				pointOfIntersection = Edge::getPointOfIntersection(Edge(point2, point2Predict), hardEdge);
				timeOfImpact = getTimeOfImpact(point2, pointOfIntersection, softBox->getVel());
				distance = pointOfIntersection - point2;
			}
			else if (Edge::areIntersecting(Edge(hardEdge.first, hardEdgePredict.first), softEdge))
			{
				pointOfIntersection = hardEdge.first;
				timeOfImpact = getTimeOfImpact(Edge::getPointOfIntersection(Edge(hardEdge.first, hardEdgePredict.first), softEdge), hardEdge.first, softBox->getVel());
				distance = hardEdge.first - pointOfCollisionOnSoftBox;
			}
			else if (Edge::areIntersecting(Edge(hardEdge.second, hardEdgePredict.second), softEdge))
			{
				pointOfIntersection = hardEdge.second;
				timeOfImpact = getTimeOfImpact(Edge::getPointOfIntersection(Edge(hardEdge.second, hardEdgePredict.second), softEdge), hardEdge.second, softBox->getVel());
				distance = hardEdge.second - pointOfCollisionOnSoftBox;
			}

			// Clear colliders if we have a new minimum time of impact
			if (timeOfImpact < minTimeOfImpact)
			{
				colliders.clear();
				minTimeOfImpact = timeOfImpact;
			}

			// Add new collider if its time of collision is not greater
			if (timeOfImpact <= minTimeOfImpact)
				colliders.push_back(Hitbox::CollisionData(hardBoxId, pointOfCollisionOnSoftBox, hardEdge, pointOfIntersection, distance, timeOfImpact, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, hardEdge, pointOfIntersection), Hitbox::CollisionType::EDGE_EDGE));
		}

		// Check for point-to-edge collisions
		for (unsigned int j = 0; j < softBox->getBox()->getPoints().size(); ++j)
		{
			// Create edge using the trajectory of current point
			Vector2 point = softBox->getPos() + softBox->getBox()->getPoints()[j];
			Vector2 pointPredict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[j];

			// Check if the point will intersect the edge
			if (!Edge::areIntersecting(Edge(point, pointPredict), hardEdge))
				continue;

			// Get total distance from edge to shape
			double totalDistanceFromEdgeToShape = getTotalDistanceFromEdgeToShape(hardEdge, softBox->getBox());
			//double totalDistanceFromEdgeToShape = getTimeOfImpactOfCenter(softBox->getBox()->getCenter(), hardEdge, softBox->getVel());

			// Check if point is collinear to edge
			if (hardEdge.onSegment(point))
			{
				// Clear colliders if we have a new minimum time of impact
				if (minTimeOfImpact != 0.0)
					colliders.clear();

				// Add new collider
				colliders.push_back(Hitbox::CollisionData(hardBoxId, point, hardEdge, point, Vector2(), 0.0, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, hardEdge, point), Hitbox::CollisionType::POINT_EDGE));
				minTimeOfImpact = 0.0;
			}

			// Find point of intersection
			Vector2 pointOfIntersection = Edge::getPointOfIntersection(Edge(point, pointPredict), hardEdge);

			// Find time for original point to reach point of intersection
			double timeOfImpact = getTimeOfImpact(point, pointOfIntersection, softBox->getVel());

			// Clear colliders if we have a new minimum time of impact
			if (timeOfImpact < minTimeOfImpact)
			{
				colliders.clear();
				minTimeOfImpact = timeOfImpact;
			}

			// Add new collider if its time of collision is not greater
			if (timeOfImpact <= minTimeOfImpact)
				colliders.push_back(Hitbox::CollisionData(hardBoxId, point, hardEdge, pointOfIntersection, pointOfIntersection - point, timeOfImpact, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, hardEdge, pointOfIntersection), Hitbox::CollisionType::POINT_EDGE));
		}

		// Check for edge-to-point collisions
		for (unsigned int j = 0; j < softBox->getBox()->getPoints().size(); ++j)
		{
			// Get edge in soft box
			Edge softEdge = Edge(
				softBox->getPos() + softBox->getBox()->getPoints()[j],
				softBox->getPos() + softBox->getBox()->getPoints()[(j + 1) % softBox->getBox()->getPoints().size()]
			);
			Edge softEdgePredict = Edge(
				softBox->getPredictPos() + softBox->getPredict()->getPoints()[j],
				softBox->getPredictPos() + softBox->getPredict()->getPoints()[(j + 1) % softBox->getPredict()->getPoints().size()]
			);

			// Get negative velocity of softbox
			Vector2 negVel = -softBox->getVel();

			// Apply negative velocity to point
			Vector2 pointPredict = hardPoint + negVel * deltaTime;

			// Check if the point will intersect the edge
			if (!Edge::areIntersecting(Edge(hardPoint, pointPredict), softEdge))
			//if (!inSquare(point, edge.first, edge.second, softEdgePredict.second, softEdgePredict.first))
				continue;

			// Get total distance from edge to shape
			double totalDistanceFromPointToShape = getTotalDistanceFromPointToShape(hardPoint, softBox->getBox());
			//double totalDistanceFromPointToShape = getTimeOfImpactOfCenter(hardPoint, softEdge, -(softBox->getVel()));

			// Check if point is collinear to edge
			if (softEdge.onSegment(hardPoint))
			{
				// Clear colliders if we have a new minimum time of impact
				if (minTimeOfImpact != 0.0)
					colliders.clear();

				// Add new collider
				colliders.push_back(Hitbox::CollisionData(hardBoxId, hardPoint, softEdge, hardPoint, Vector2(), 0.0, totalDistanceFromPointToShape, getDirectionOfImpact(softBox, softEdge, hardPoint), Hitbox::CollisionType::EDGE_POINT));
				minTimeOfImpact = 0.0;
			}

			// If we've already found a collision on segment, then we can ignore all other possible collisions
			if (minTimeOfImpact == 0.0)
				continue;

			// Point of intersection lies on the edge of the soft box
			Vector2 pointOfIntersection = Edge::getPointOfIntersection(Edge(hardPoint, pointPredict), softEdge);

			// Find time for original point to reach point of intersection
			double timeOfImpact = getTimeOfImpact(pointOfIntersection, hardPoint, softBox->getVel());

			// Clear colliders if we have a new minimum time of impact
			if (timeOfImpact < minTimeOfImpact)
			{
				colliders.clear();
				minTimeOfImpact = timeOfImpact;
			}

			// Add new collider if its time of collision is not greater
			if (timeOfImpact <= minTimeOfImpact)
				colliders.push_back(Hitbox::CollisionData(hardBoxId, hardPoint, softEdge, pointOfIntersection, hardPoint - pointOfIntersection, timeOfImpact, totalDistanceFromPointToShape, getDirectionOfImpact(softBox, softEdge, hardPoint), Hitbox::CollisionType::EDGE_POINT));
		}
	}

	if (colliders.size() == 0)
		return Hitbox::CollisionData();

	// Narrow down colliders to one
	Hitbox::CollisionData bestCollider = colliders[0];
	for (auto& collider : colliders)
	{
		// Prioritize edge-to-edge collisions, then point-to-edge collisions
		if (collider.collisionType == Hitbox::CollisionType::EDGE_EDGE && bestCollider.collisionType != Hitbox::CollisionType::EDGE_EDGE)
			bestCollider = collider;
		else if (collider.collisionType == Hitbox::CollisionType::POINT_EDGE && bestCollider.collisionType == Hitbox::CollisionType::EDGE_POINT)
			bestCollider = collider;
		// Prioritize the edge that is closest to the point
		else if (collider.totalDistanceFromEdgeToShape < bestCollider.totalDistanceFromEdgeToShape)
			bestCollider = collider;
	}

	/*
	std::cout <<
		"[point: " << bestCollider.point << "]" <<
		", [predictedPoint: " << bestCollider.point + softBox->getVel() * deltaTime << "]" <<
		", [edge: " << bestCollider.edge << "]" <<
		", [POI: " << bestCollider.pointOfIntersection << "]" <<
		", [distance: " << bestCollider.distance << "]" <<
		", [timeOfImpact=" << bestCollider.timeOfImpact << "]" <<
		", [direction=" << bestCollider.direction << "]" <<
		", [collisionType=" << bestCollider.collisionType << "]" <<
		std::endl;
		*/
		

	return bestCollider;
}

/**
 * \brief Helper function that returns a vector of colliders with that the soft box will collide with first
 *
 * \param softBox: The soft object doing the colliding
 * \param hardBoxes: Vector storage of all hard boxes
 * \param colliders: External vector storage containing pairs of hitboxes and their collision data after colliding with the soft box
 * \param deltaTime: deltaTime for velocity calculations
 * \return The minimum time of impact between the soft box and a hard box. Also modifies the external colliders vector
 */
double getColliders(Hitbox* softBox, std::vector<Hitbox*> hardBoxes, std::vector<Hitbox::CollisionData>& colliders, double deltaTime)
{
	double minTimeOfImpact = INT_MAX;

	// Loop through all hard objects
	for (unsigned int i = 0 ; i < hardBoxes.size(); ++i)
	{
		Hitbox* hardBox = hardBoxes[i];

		// Prune
		// Check if there is actually movement
		if (softBox->getBox() == softBox->getPredict() &&
			hardBox->getBox() == hardBox->getPredict())
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
		Hitbox::CollisionData collisionData = getCollisionData(softBox, hardBox, i, deltaTime);
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

bool clips(Hitbox* softBox, std::vector<Hitbox*> hardBoxes, double deltaTime)
{
	// Check if softBox clips any hardBoxes
	for (auto& hardBox : hardBoxes)
	{
		// Check if a collision occurs
		std::pair<Vector2, double> mtv = softBox->checkCollision(hardBox);
		if (mtv.second == 0) // No collision
			continue;

		return true;
	}

	return false;
}

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
			if (box1->getBox() == box1->getPredict() &&
				box2->getBox() == box2->getPredict())
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

			Hitbox::CollisionData collisionData;

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
		std::vector<Hitbox::CollisionData> colliders;
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
			if (colliders[0].collisionType == Hitbox::CollisionType::EDGE_EDGE)
			{
				for (unsigned int i = 0; i < colliders.size(); ++i)
				{
					if (colliders[i].collisionType != Hitbox::CollisionType::EDGE_EDGE)
					{
						colliders.erase(colliders.begin() + i);
						--i;
					}
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

				std::cout <<
					"[point: " << collider.point << "]" <<
					", [predictedPoint: " << collider.point + softBox->getVel() * deltaTime << "]" <<
					", [edge: " << collider.edge << "]" <<
					", [POI: " << collider.pointOfIntersection << "]" <<
					", [distance: " << collider.distance << "]" <<
					", [timeOfImpact=" << collider.timeOfImpact << "]" <<
					", [direction=" << collider.direction << "]" <<
					", [collisionType=" << collider.collisionType << "]" <<
					std::endl;

				// Redirect soft object velocity
				softBox->setVel((softBox->getVel() - minDistance).projectOntoAxis(collider.edge.slope()));
				//softBox->setVel((softBox->getVel() - minDistance).projectOntoAxis(collider.edge.slope()) * pow(1.0f / 32.0f, deltaTime));
			}

			// Remove all colliders from the next loop
			for (unsigned int i = 0; i < hardBoxes.size(); ++i)
			{
				for (auto& collider : colliders)
				{
					if (i == collider.hardBoxId)
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

void WLUW::SampleGame::Game::update(double deltaTime)
{
	// Update all objects
	for (auto& o : objects)
	{
		o->update(deltaTime);
	}

	// Handle collisions
	handleCollisions(deltaTime);

	// Move all objects
	for (auto& o : objects)
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
