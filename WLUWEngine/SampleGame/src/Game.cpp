#include "Game.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace WLUW;

enum Orientation
{
	COLLINEAR = 0,
	CLOCKWISE,
	COUNTERCLOCKWISE
};

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

/**
 * \brief Helper function that returns whether a given point lies on a given edge
 *
 * \param point: Specified point
 * \param edge: Specified edge
 * \return True if point lies on edge, false otherwise
 */
bool onSegment(Vector2 point, Edge edge)
{
	// Check collinearity
	double val = (edge.first.y - point.y) * (edge.second.x - edge.first.x) - (edge.first.x - point.x) * (edge.second.y - edge.first.y);
	if (val != 0.0)
		return false;

	return (point - edge.first).size() + (point - edge.second).size() == (edge.second - edge.first).size();
}

/**
 * \brief Helper function that returns the orientation of an ordered triplet
 *
 * \param point1: First point in the triplet
 * \param point2: Second point in the triplet
 * \param point3: Third point in the triplet
 * \return Orientation object specifying the orientation of the given ordered triplet, or COLLINEAR if collinear
 */
Orientation getOrientation(Vector2 point1, Vector2 point2, Vector2 point3)
{
	double val = (point2.y - point1.y) * (point3.x - point2.x) - (point2.x - point1.x) * (point3.y - point2.y);

	if (abs(val) < 0.0001) // Make sure to compare to an episilon value (Acceptable error), otherwise the algorithm is buggy
		return Orientation::COLLINEAR;

	return (val > 0) ? Orientation::CLOCKWISE : Orientation::COUNTERCLOCKWISE;
}

/**
 * \brief Helper function that returns whether two edges intersect
 *
 * \param edge1: First edge
 * \param edge2: Second edge
 * \return True if edge1 and edge2 intersect, false otherwise
 */
bool areIntersecting(Edge edge1, Edge edge2)
{
	// Check for parallelism
	if (edge1.second - edge1.first == edge2.second - edge2.first ||
		edge1.second - edge1.first == edge2.first - edge2.second)
		return false;

	// Find the four orientations needed for general and special cases
	Orientation o1 = getOrientation(edge1.first, edge1.second, edge2.first);
	Orientation o2 = getOrientation(edge1.first, edge1.second, edge2.second);
	Orientation o3 = getOrientation(edge2.first, edge2.second, edge1.first);
	Orientation o4 = getOrientation(edge2.first, edge2.second, edge1.second);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// edge2.first collinear with edge1
	if (o1 == 0 && onSegment(edge2.first, edge1)) return true;
	// edge2.second collinear with edge1
	if (o2 == 0 && onSegment(edge2.second, edge1)) return true;
	// edge1.first collinear with edge2
	if (o3 == 0 && onSegment(edge1.first, edge2)) return true;
	// edge1.second collinear with edge2
	if (o4 == 0 && onSegment(edge1.second, edge2)) return true;

	return false; // Doesn't fall in any of the above cases
}

/**
 * \brief Helper function that returns the point of intersection between two edges
 *
 * \param edge1: First edge
 * \param edge2: Second edge
 * \return The point of intersection between edge1 and edge2
 */
Vector2 getPointOfIntersection(Edge edge1, Edge edge2)
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
		// Return the point that is closest to the first point in edge1
		if ((edge2.first - edge1.first).size() <= (edge2.second - edge1.first).size())
			return edge2.first;
		return edge2.second;
	}
	else
	{
		// Get point of intersection
		double x = (b2 * c1 - b1 * c2) / determinant;
		double y = (a1 * c2 - a2 * c1) / determinant;

		return Vector2(x, y);
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

	if (vel.x == 0.0)
		timeOfImpact = abs(point2.y - point1.y) / vel.y;
	else if (vel.y == 0.0)
		timeOfImpact = abs(point2.x - point1.x) / vel.x;
	else
		timeOfImpact = std::min(abs(point2.x - point1.x) / vel.x, abs(point2.y - point1.y) / vel.y);

	// Return clamped value to prevent errors
	return std::clamp(timeOfImpact, 0.0, 1.0);
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
	double slope = (edge.second.y - edge.first.y) / (edge.second.x - edge.first.x);

	if (abs(slope) <= 1 && pointOfIntersection.y > box->getBox()->getCenter().y)
		direction = Hitbox::Direction::BOTTOM;
	else if (abs(slope) <= 1 && pointOfIntersection.y < box->getBox()->getCenter().y)
		direction = Hitbox::Direction::TOP;
	else if ((abs(slope) > 1 || edge.first.x == edge.second.x) && pointOfIntersection.x > box->getBox()->getCenter().x)
		direction = Hitbox::Direction::RIGHT;
	else if ((abs(slope) > 1 || edge.first.x == edge.second.x) && pointOfIntersection.x < box->getBox()->getCenter().x)
		direction = Hitbox::Direction::LEFT;

	return direction;
}

/**
 * \brief Helper function that returns collision data of the collision between a soft hitbox and a hard hitbo
 *
 * \param softBox: Moving soft hitbox
 * \param hardBox: Stationary hard hitbox
 * \param deltaTime: deltaTime for velocity calculations
 * \return CollisionData object with details of the collision between softBox and hardBox (See Hitbox.h for details)
 */
Hitbox::CollisionData getCollisionData(Hitbox* softBox, Hitbox* hardBox, double deltaTime)
{
	// Get a list of all edges and points that the soft box will collide with
	std::vector<Hitbox::CollisionData> colliders;
	double minTimeOfImpact = INT_MAX;

	// Loop through all edges in hardBox
	for (unsigned int i = 0; i < hardBox->getBox()->getPoints().size(); ++i)
	{
		Edge edge = std::make_pair(
			hardBox->getPos() + hardBox->getBox()->getPoints()[i],
			hardBox->getPos() + hardBox->getBox()->getPoints()[(i + 1) % hardBox->getBox()->getPoints().size()]
		);

		// Check if any edges in soft box will intersect current edge
		for (unsigned int j = 0; j < softBox->getBox()->getPoints().size(); ++j)
		{
			// Create edge using the trajectory of current point
			Vector2 point1 = softBox->getPos() + softBox->getBox()->getPoints()[j];
			Vector2 point1Predict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[j];
			Vector2 point2 = softBox->getPos() + softBox->getBox()->getPoints()[(j + 1) % softBox->getBox()->getPoints().size()];
			Vector2 point2Predict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[(j + 1) % softBox->getBox()->getPoints().size()];
			Vector2 average = (point1 + point2) / 2;
			Vector2 averagePredict = (point1Predict + point2Predict) / 2;

			// Check if the point will intersect the edge
			if (!areIntersecting(std::make_pair(point1, point1Predict), edge) || !areIntersecting(std::make_pair(point2, point2Predict), edge))
				continue;

			// Get total distance from edge to shape
			double totalDistanceFromEdgeToShape = getTotalDistanceFromEdgeToShape(edge, softBox->getBox());

			// Check if point is collinear to edge
			if (onSegment(point1, edge) && onSegment(point2, edge))
			{
				// Clear colliders if we have a new minimum time of impact
				if (minTimeOfImpact != 0.0)
					colliders.clear();

				// Add new collider
				colliders.push_back(Hitbox::CollisionData(average, edge, average, Vector2(), 0.0, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, edge, average), Hitbox::CollisionType::EDGE_EDGE));
				minTimeOfImpact = 0.0;
			}

			// If we've already found a collision on segment, then we can ignore all other possible collisions
			if (minTimeOfImpact == 0.0)
				continue;

			// Find point of intersection
			Vector2 pointOfIntersection1 = getPointOfIntersection(std::make_pair(point1, point1Predict), edge);
			Vector2 pointOfIntersection2 = getPointOfIntersection(std::make_pair(point2, point2Predict), edge);

			// Find time for original point to reach point of intersection
			double timeOfImpact = std::min(getTimeOfImpact(point1, pointOfIntersection1, softBox->getVel()), getTimeOfImpact(point2, pointOfIntersection2, softBox->getVel()));
			Vector2 distance = (pointOfIntersection1 - point1).size() < (pointOfIntersection2 - point2).size() ? pointOfIntersection1 - point1 : pointOfIntersection2 - point2;

			// Clear colliders if we have a new minimum time of impact
			if (timeOfImpact < minTimeOfImpact)
			{
				colliders.clear();
				minTimeOfImpact = timeOfImpact;
			}

			// Add new collider if its time of collision is not greater
			if (timeOfImpact <= minTimeOfImpact)
				colliders.push_back(Hitbox::CollisionData(average, edge, pointOfIntersection1, distance, timeOfImpact, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, edge, pointOfIntersection1), Hitbox::CollisionType::EDGE_EDGE));
		}

		// Check if any points in soft box will intersect current edge
		for (unsigned int j = 0; j < softBox->getBox()->getPoints().size(); ++j)
		{
			// Create edge using the trajectory of current point
			Vector2 point = softBox->getPos() + softBox->getBox()->getPoints()[j];
			Vector2 pointPredict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[j];

			// Check if the point will intersect the edge
			if (!areIntersecting(std::make_pair(point, pointPredict), edge))
				continue;

			// Get total distance from edge to shape
			double totalDistanceFromEdgeToShape = getTotalDistanceFromEdgeToShape(edge, softBox->getBox());

			// Check if point is collinear to edge
			if (onSegment(point, edge))
			{
				// Clear colliders if we have a new minimum time of impact
				if (minTimeOfImpact != 0.0)
					colliders.clear();

				// Add new collider
				colliders.push_back(Hitbox::CollisionData(point, edge, point, Vector2(), 0.0, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, edge, point), Hitbox::CollisionType::POINT_EDGE));
				minTimeOfImpact = 0.0;
			}

			// Find point of intersection
			Vector2 pointOfIntersection = getPointOfIntersection(std::make_pair(point, pointPredict), edge);

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
				colliders.push_back(Hitbox::CollisionData(point, edge, pointOfIntersection, pointOfIntersection - point, timeOfImpact, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, edge, pointOfIntersection), Hitbox::CollisionType::POINT_EDGE));
		}
	}

	// Loop through all points in hardBox
	for (unsigned int i = 0; i < hardBox->getBox()->getPoints().size(); ++i)
	{
		// Get point in hard box
		Vector2 point = hardBox->getPos() + hardBox->getBox()->getPoints()[i];

		// Check if any edges in soft box will intersect current point
		for (unsigned int j = 0; j < softBox->getBox()->getPoints().size(); ++j)
		{
			// Get edge in soft box
			Edge edge = std::make_pair(
				softBox->getPos() + softBox->getBox()->getPoints()[j],
				softBox->getPos() + softBox->getBox()->getPoints()[(j + 1) % softBox->getBox()->getPoints().size()]
			);

			// Get negative velocity of softbox
			Vector2 negVel = -softBox->getVel();

			// Apply negative velocity to point
			Vector2 pointPredict = point + negVel * deltaTime;

			// Check if the point will intersect the edge
			if (!areIntersecting(std::make_pair(point, pointPredict), edge))
				continue;

			// Get total distance from edge to shape
			double totalDistanceFromPointToShape = getTotalDistanceFromPointToShape(point, softBox->getBox());

			// Check if point is collinear to edge
			if (onSegment(point, edge))
			{
				// Clear colliders if we have a new minimum time of impact
				if (minTimeOfImpact != 0.0)
					colliders.clear();

				// Add new collider
				colliders.push_back(Hitbox::CollisionData(point, edge, point, Vector2(), 0.0, totalDistanceFromPointToShape, getDirectionOfImpact(softBox, edge, point), Hitbox::CollisionType::EDGE_POINT));
				minTimeOfImpact = 0.0;
			}

			// If we've already found a collision on segment, then we can ignore all other possible collisions
			if (minTimeOfImpact == 0.0)
				continue;

			// Point of intersection lies on the edge of the soft box
			Vector2 pointOfIntersection = getPointOfIntersection(std::make_pair(point, pointPredict), edge);

			// Find time for original point to reach point of intersection
			double timeOfImpact = getTimeOfImpact(pointOfIntersection, point, softBox->getVel());

			// Clear colliders if we have a new minimum time of impact
			if (timeOfImpact < minTimeOfImpact)
			{
				colliders.clear();
				minTimeOfImpact = timeOfImpact;
			}

			// Add new collider if its time of collision is not greater
			if (timeOfImpact <= minTimeOfImpact)
				colliders.push_back(Hitbox::CollisionData(point, edge, pointOfIntersection, point - pointOfIntersection, timeOfImpact, totalDistanceFromPointToShape, getDirectionOfImpact(softBox, edge, point), Hitbox::CollisionType::EDGE_POINT));
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
	std::cout << std::endl <<
		"[point: " << bestCollider.point << "]" <<
		", [predictedPoint: " << bestCollider.point + softBox->getVel() * deltaTime << "]" <<
		", [edge: p1: " << bestCollider.edge.first << "]" <<
		", p2: " << bestCollider.edge.second << "]" <<
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
	for (auto& hardBox : hardBoxes)
	{
		// Prune
		// Check if there is actually movement
		if (softBox->getBox() == softBox->getPredict() &&
			hardBox->getBox() == hardBox->getPredict())
			continue;

		// Check if the boxes' AABBs overlap
		SDL_Rect AABB1 = softBox->getAABB();
		SDL_Rect AABB2 = hardBox->getAABB();
		if (!(AABB1.x + AABB1.w >= AABB2.x &&
			AABB2.x + AABB2.x >= AABB1.x &&
			AABB1.y + AABB1.h >= AABB2.y &&
			AABB2.y + AABB2.y >= AABB1.y))
			continue;

		// Check if a collision occurs
		std::pair<Vector2, double> mtv = softBox->predictCollision(hardBox);
		if (mtv.second == 0) // No collision
			continue;

		// A collision is guaranteed to occur
		// Get collision data
		Hitbox::CollisionData collisionData = getCollisionData(softBox, hardBox, deltaTime);

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
			SDL_Rect AABB1 = box1->getAABB();
			SDL_Rect AABB2 = box2->getAABB();
			if (!(AABB1.x + AABB1.w >= AABB2.x &&
				AABB2.x + AABB2.x >= AABB1.x &&
				AABB1.y + AABB1.h >= AABB2.y &&
				AABB2.y + AABB2.y >= AABB1.y))
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
				// Check which object is soft
				Hitbox* softBox = box1->getInertia() == Hitbox::SOFT ? box1 : box2;
				Hitbox* hardBox = box1->getInertia() == Hitbox::HARD ? box1 : box2;

				// Get collision data
				collisionData = getCollisionData(softBox, hardBox, deltaTime);
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
		double lastTimeOfImpact = timeOfImpact;

		// Find and resolve collisions until no more collisions are found
		do
		{
			// Recalculate predict
			softBox->updatePredict(deltaTime);

			// Get a list of colliders as well as the minimum time of impact
			colliders.clear();
			timeOfImpact = getColliders(softBox, hardBoxes, colliders, deltaTime);

			// Check if time of impact is repeated
			if (timeOfImpact == lastTimeOfImpact)
				break;

			lastTimeOfImpact = timeOfImpact;

			// No more collisions need to be resolved
			if (colliders.size() == 0)
				break;

			// Resolve collision
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
				Vector2 slope = collider.edge.second - collider.edge.first;
				softBox->setVel((softBox->getVel() - minDistance).projectOntoAxis(slope));
				//softBox->setVel((softBox->getVel() - minDistance).projectOntoAxis(slope) * pow(1.0f / 32.0f, deltaTime));

				//std::cout << "slope: " << slope << std::endl;
			}

			//std::cout << "vel = " << softBox->getVel() << std::endl;
			//std::cout << colliders.size() << std::endl;
			//std::cout << "while loop" << std::endl;
			
			// Check if new position clips any other objects
			if (clips(softBox, hardBoxes, deltaTime))
				std::cout << "Object clips after collision resolution" << std::endl;
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
