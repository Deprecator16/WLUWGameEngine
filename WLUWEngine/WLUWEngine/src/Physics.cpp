#include "Physics.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include "Collision.h"
#include "Hitbox.h"
#include "WObject.h"

using namespace WLUW;


// Casts a line segment and returns all hits
std::vector<WLUW::RaycastHit> WLUW::Physics::linecastAll(std::vector<WObject*> objects, Vector2 start, Vector2 end)
{
	std::vector<RaycastHit> hits;

	// Loop through all objects
	for (auto& object : objects)
	{
		// Loop through all edges in object
		for (unsigned int i = 0; i < object->getHitbox()->getPoints().size(); ++i)
		{
			Edge edge = Edge(
				objects[i]->getHitbox()->getPos() + objects[i]->getHitbox()->getPoints()[i],
				objects[i]->getHitbox()->getPos() + objects[i]->getHitbox()->getPoints()[(i + 1) % object->getHitbox()->getPoints().size()]
			);

			if (Edge::areIntersecting(Edge(start, end), edge))
			{
				Vector2 pointOfIntersection = Edge::getPointOfIntersection(Edge(start, end), edge);
				hits.push_back(RaycastHit(pointOfIntersection, pointOfIntersection, edge.normal().normalized(), pointOfIntersection - start, (pointOfIntersection - start).size() / (end - start).size(), object->getHitbox()));
			}
		}
	}

	// Sort the hits by fraction
	std::sort(hits.begin(), hits.end(), RaycastHit::compare);

	return hits;
}
WLUW::RaycastHit WLUW::Physics::linecast(std::vector<WObject*> objects, Vector2 start, Vector2 end)
{
	std::vector<RaycastHit> hits = linecastAll(objects, start, end);

	if (hits.size() == 0)
		return RaycastHit();

	return hits[0];
}

// Casts a ray and returns all hits
std::vector<WLUW::RaycastHit> WLUW::Physics::raycastAll(std::vector<WObject*> objects, Vector2 origin, Vector2 direction, double distance)
{
	return linecastAll(objects, origin, origin + (direction * distance));
}
// Casts a ray and returns the first hit
WLUW::RaycastHit WLUW::Physics::raycast(std::vector<WObject*> objects, Vector2 origin, Vector2 direction, double distance)
{
	std::vector<RaycastHit> hits = raycastAll(objects, origin, direction, distance);

	if (hits.size() == 0)
		return RaycastHit();

	return hits[0];
}
// Casts an edge and returns all hits
std::vector<WLUW::RaycastHit> WLUW::Physics::edgecastAll(std::vector<WObject*> objects, Edge edge, Vector2 direction, double distance)
{
	std::vector<RaycastHit> hits;
	Shape edgecastShape(edge.first);
	edgecastShape.addPoint(Vector2());
	edgecastShape.addPoint(edge.second - edge.first);
	edgecastShape.addPoint(edge.second + (direction * distance) - edge.first);
	edgecastShape.addPoint(direction * distance);

	// Loop through all objects
	for (auto& object : objects)
	{
		// Check if object overlaps with edgecastShape
		std::pair<Vector2, double> mtv = Shape::checkCollision(*object->getHitbox(), edgecastShape);
		if (mtv.second == 0) // No collision
			continue;

		// Get the best hit in the shape
		std::vector<RaycastHit> shapeHits;

		// Loop through all points and edges in object
		for (unsigned int i = 0; i < object->getHitbox()->getPoints().size(); ++i)
		{
			Vector2 boxPoint = object->getHitbox()->getPoints()[i];
			Edge boxEdge = Edge(
				object->getHitbox()->getPos() + object->getHitbox()->getPoints()[i],
				object->getHitbox()->getPos() + object->getHitbox()->getPoints()[(i + 1) % object->getHitbox()->getPoints().size()]
			);

			// Check separation of box point towards edge
			if (Edge::areIntersecting(edge, Edge(boxPoint, boxPoint - (direction * distance))))
			{
				Vector2 pointOfIntersection = Edge::getPointOfIntersection(edge, Edge(boxPoint, boxPoint - (direction * distance)));
				Vector2 separation = boxPoint - pointOfIntersection;
				shapeHits.push_back(RaycastHit(edge.first + separation, boxPoint, edge.normal().normalized(), separation, separation.size() / distance, object->getHitbox()));
			}

			// Check separation of the two points in edge to box edge
			if (Edge::areIntersecting(Edge(edge.first, edge.first + (direction * distance)), boxEdge))
			{
				Vector2 pointOfIntersection = Edge::getPointOfIntersection(Edge(edge.first, edge.first + (direction * distance)), boxEdge);
				Vector2 separation = pointOfIntersection - edge.first;
				shapeHits.push_back(RaycastHit(edge.first + separation, pointOfIntersection, boxEdge.normal().normalized(), separation, separation.size() / distance, object->getHitbox()));
			}
			if (Edge::areIntersecting(Edge(edge.second, edge.second + (direction * distance)), boxEdge))
			{
				Vector2 pointOfIntersection = Edge::getPointOfIntersection(Edge(edge.second, (direction * distance) + edge.second), boxEdge);
				Vector2 separation = pointOfIntersection - edge.second;
				shapeHits.push_back(RaycastHit(edge.first + separation, pointOfIntersection, boxEdge.normal().normalized(), separation, separation.size() / distance, object->getHitbox()));
			}
		}

		// Check if there are no hits
		if (shapeHits.size() == 0)
			return std::vector<RaycastHit>();

		// Find the best hit in shape hits and add it to the final hits vector
		std::sort(shapeHits.begin(), shapeHits.end(), RaycastHit::compare);
		hits.push_back(shapeHits[0]);
	}

	// Sort the hits by fraction
	std::sort(hits.begin(), hits.end(), RaycastHit::compare);

	return hits;
}
// Casts an edge and returns the first hit
WLUW::RaycastHit WLUW::Physics::edgecast(std::vector<WObject*> objects, Edge edge, Vector2 direction, double distance)
{
	std::vector<RaycastHit> hits = edgecastAll(objects, edge, direction, distance);

	if (hits.size() == 0)
		return RaycastHit();

	return hits[0];
}
// Casts a shape and returns all hits
std::vector<WLUW::RaycastHit> WLUW::Physics::shapecastAll(std::vector<WObject*> objects, Shape shape, Vector2 direction, double distance)
{
	std::vector<RaycastHit> hits;

	// Get the best hit in the shape
	std::vector<RaycastHit> shapeHits;

	// Loop through all edges in shape
	for (unsigned int i = 0; i < shape.getPoints().size(); ++i)
	{
		Edge shapeEdge(Edge(
			shape.getPos() + shape.getPoints()[i],
			shape.getPos() + shape.getPoints()[(i + 1) % shape.getPoints().size()]
		));

		// Cast every edge
		RaycastHit edgecastHit = edgecast(objects, shapeEdge, direction, distance);
		shapeHits.push_back(edgecastHit);
	}

	// Find the best hit
	std::sort(shapeHits.begin(), shapeHits.end(), RaycastHit::compare);
	shapeHits[0].centroid = shape.getPos() + shapeHits[0].separation;
	hits.push_back(shapeHits[0]);

	// Sort the hits by fraction
	std::sort(hits.begin(), hits.end(), RaycastHit::compare);

	return hits;
}
// Casts a shape and returns the first hit
WLUW::RaycastHit WLUW::Physics::shapecast(std::vector<WObject*> objects, Shape shape, Vector2 direction, double distance)
{
	std::vector<RaycastHit> hits = shapecastAll(objects, shape, direction, distance);

	if (hits.size() == 0)
		return RaycastHit();

	return hits[0];
}

/*
// Helper function that returns the time of impact (0 to 1) between two points given the first point's velocity
double getTimeOfImpact(Vector2 point1, Vector2 point2, Vector2 vel)
{
	double timeOfImpact;

	if (vel.size() == 0.0)
		return 0.0;

	if (vel.x == 0.0)
		timeOfImpact = abs(point2.y - point1.y) / vel.y;
	else if (vel.y == 0.0)
		timeOfImpact = abs(point2.x - point1.x) / vel.x;
	else
		timeOfImpact = std::max(abs((point2.x - point1.x) / vel.x), abs((point2.y - point1.y) / vel.y));

	// Return clamped value to prevent errors
	return std::clamp(timeOfImpact, 0.0f, 1.0f);
}

// Helper function that returns the closest distance from point to an edge
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

// Helper function that returns the total distance from an edge to a shape
double getTotalDistanceFromEdgeToShape(Edge edge, Shape* shape)
{
	double totalDistance = 0.0;

	for (auto& point : shape->getPoints())
		totalDistance += getDistanceFromPointToEdge(shape->getPos() + point, edge);

	return totalDistance;
}

/// Helper function that returns the total distance from a point to a shape
double getTotalDistanceFromPointToShape(Vector2 point, Shape* shape)
{
	double totalDistance = 0.0;

	for (auto& shapePoint : shape->getPoints())
		totalDistance += abs((shapePoint - point).size());

	return totalDistance;
}

/// Helper function that returns the area of a triangle
double getTriangleArea(Vector2 a, Vector2 b, Vector2 c)
{
	return ((c.x * b.y - b.x * c.y) - (c.x * a.y - a.x * c.y) + (b.x * a.y - a.x * b.y)) / 2.0;
}

// Helper function that returns whether a point is inside a square
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
*/

// Helper function that returns the direction of impact between a colliding box and edge
Collision::Direction getDirectionOfImpact(Hitbox* box, Edge edge, Vector2 pointOfIntersection)
{
	// Get slope of colliding edge
	Collision::Direction direction = Collision::Direction::NO_DIRECTION;

	if (abs(edge.slope().x) >= abs(edge.slope().y) && pointOfIntersection.y > box->getCenter().y)
		direction = Collision::Direction::BOTTOM;
	else if (abs(edge.slope().x) >= abs(edge.slope().y) && pointOfIntersection.y < box->getCenter().y)
		direction = Collision::Direction::TOP;
	else if (abs(edge.slope().x) < abs(edge.slope().y) && pointOfIntersection.x > box->getCenter().x)
		direction = Collision::Direction::RIGHT;
	else if (abs(edge.slope().x) < abs(edge.slope().y) && pointOfIntersection.x < box->getCenter().x)
		direction = Collision::Direction::LEFT;

	return direction;
}

bool clips(Hitbox* softBox, std::vector<Hitbox*> hardBoxes, double deltaTime)
{
	// Check if softBox clips any hardBoxes
	for (auto& hardBox : hardBoxes)
	{
		// Check if a collision occurs
		std::pair<Vector2, double> mtv = Shape::checkCollision(*softBox, *hardBox);
		if (mtv.second == 0) // No collision
			continue;

		return true;
	}

	return false;
}

void WLUW::Physics::solveCollision(Collision collision, std::vector<WObject*> objects, double deltaTime)
{
	// Check if we have a collision between a soft object and a hard object
	if (!collision.ignore)
	{
		std::cout << collision.separation << std::endl;

		if (collision.box->getInertia() == Hitbox::Inertia::SOFT)
		{
			collision.box->setPos(collision.box->getPos() + collision.separation);
			collision.box->setVel((collision.box->getVel() - collision.separation).projectOntoAxis(collision.normal.normal()));
		}
		else
		{
			collision.otherBox->setPos(collision.otherBox->getPos() + collision.separation);
			collision.otherBox->setVel((collision.otherBox->getVel() - collision.separation).projectOntoAxis(collision.normal.normal()));
		}

		collision.box->move(objects, deltaTime);
		collision.otherBox->move(objects, deltaTime);
	}
}


