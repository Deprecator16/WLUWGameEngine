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
	std::vector<Vector2> edgecastShapePoints;
	edgecastShapePoints.push_back(edge.first);
	edgecastShapePoints.push_back(edge.second);
	edgecastShapePoints.push_back(edge.second + (direction * distance));
	edgecastShapePoints.push_back(edge.first + (direction * distance));
	Shape edgecastShape(edgecastShapePoints, edge.first);

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
	return std::vector< WLUW::RaycastHit>();
}
// Casts a shape and returns the first hit
WLUW::RaycastHit WLUW::Physics::shapecast(std::vector<WObject*> objects, Shape shape, Vector2 direction, double distance)
{
	std::vector<RaycastHit> hits = shapecastAll(objects, shape, direction, distance);

	if (hits.size() == 0)
		return RaycastHit();

	return hits[0];
}

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
	return std::clamp(timeOfImpact, 0.0, 1.0);
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

// Comparison function to sort collision data
bool compareCollisions(const Collision& a, const Collision& b)
{
	if (a.timeOfImpact < b.timeOfImpact) return true;
	if (a.timeOfImpact > b.timeOfImpact) return false;

	if (a.totalDistanceFromEdgeToShape < b.totalDistanceFromEdgeToShape) return true;
	if (a.totalDistanceFromEdgeToShape > b.totalDistanceFromEdgeToShape) return false;

	return false;
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

/*
// Helper function that returns collision data of the collision between a soft hitbox and a hard hitbo
WLUW::Collision WLUW::Physics::getCollisionData(Hitbox* softBox, Hitbox* hardBox, double deltaTime)
{
	// Get a list of all edges and points that the soft box will collide with
	std::vector<Collision> colliders;
	double minTimeOfImpact = INT_MAX;

	// Loop through all hard box points
	for (unsigned int i = 0; i < hardBox->getPoints().size(); ++i)
	{
		// Get data from hard box
		Vector2 hardPoint = hardBox->getPos() + hardBox->getPoints()[i];
		Edge hardEdge = Edge(
			hardBox->getPos() + hardBox->getPoints()[i],
			hardBox->getPos() + hardBox->getPoints()[(i + 1) % hardBox->getPoints().size()]
		);

		// Check if the soft box has to move through other edges to reach current edge
		bool edgeValid = true;
		for (unsigned int j = 0; j < softBox->getPoints().size(); ++j)
		{
			// Create line from current soft point to the midpoint of current hard edge
			Edge cut(softBox->getPos() + softBox->getPoints()[j], hardEdge.midpoint());

			if (cut.first == cut.second)
				continue;

			if (cut.slope().normalized() == hardEdge.slope().normalized() || cut.slope().normalized() == -hardEdge.slope().normalized())
				continue;

			for (unsigned int k = 0; k < hardBox->getPoints().size(); ++k)
			{
				Edge edgeToBeChecked = Edge(
					hardBox->getPos() + hardBox->getPoints()[k],
					hardBox->getPos() + hardBox->getPoints()[(k + 1) % hardBox->getPoints().size()]
				);

				if (edgeToBeChecked == hardEdge)
					continue;

				if (Edge::areIntersecting(cut, edgeToBeChecked))
				{
					edgeValid = false;
					break;
				}
			}

			// Check if we have already invalidated current hard edge
			if (!edgeValid)
				break;
		}









		if (edgeValid)
		{
			// Check for edge-to-edge collisions
			for (unsigned int j = 0; j < softBox->getPoints().size(); ++j)
			{
				// Create edge using the trajectory of current point
				Vector2 point1 = softBox->getPos() + softBox->getPoints()[j];
				Vector2 point1Predict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[j];
				Vector2 point2 = softBox->getPos() + softBox->getPoints()[(j + 1) % softBox->getPoints().size()];
				Vector2 point2Predict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[(j + 1) % softBox->getPoints().size()];
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
				double totalDistanceFromEdgeToShape = getTotalDistanceFromEdgeToShape(hardEdge, softBox);

				// Check if point is collinear to edge
				if (hardEdge.onSegment(point1) || hardEdge.onSegment(point2) || softEdge.onSegment(hardEdge.first) || softEdge.onSegment(hardEdge.second))
				{
					// Clear colliders if we have a new minimum time of impact
					if (minTimeOfImpact != 0.0)
						colliders.clear();

					// Add new collider
					colliders.push_back(Collision(softBox, hardBox, average, hardEdge, average, Vector2(), 0.0, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, hardEdge, average), Collision::CollisionType::EDGE_EDGE));
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
					colliders.push_back(Collision(softBox, hardBox, pointOfCollisionOnSoftBox, hardEdge, pointOfIntersection, distance, timeOfImpact, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, hardEdge, pointOfIntersection), Collision::CollisionType::EDGE_EDGE));
			}

			// Check for point-to-edge collisions
			for (unsigned int j = 0; j < softBox->getPoints().size(); ++j)
			{
				// Create edge using the trajectory of current point
				Vector2 point = softBox->getPos() + softBox->getPoints()[j];
				Vector2 pointPredict = softBox->getPredictPos() + softBox->getPredict()->getPoints()[j];

				// Check if the point will intersect the edge
				if (!Edge::areIntersecting(Edge(point, pointPredict), hardEdge))
					continue;

				// Get total distance from edge to shape
				double totalDistanceFromEdgeToShape = getTotalDistanceFromEdgeToShape(hardEdge, softBox);

				// Check if point is collinear to edge
				if (hardEdge.onSegment(point))
				{
					// Clear colliders if we have a new minimum time of impact
					if (minTimeOfImpact != 0.0)
						colliders.clear();

					// Add new collider
					colliders.push_back(Collision(softBox, hardBox, point, hardEdge, point, Vector2(), 0.0, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, hardEdge, point), Collision::CollisionType::POINT_EDGE));
					minTimeOfImpact = 0.0;
				}

				// Find point of intersection
				Vector2 pointOfIntersection = Edge::getPointOfIntersection(Edge(point, pointPredict), hardEdge);
				Vector2 distance = pointOfIntersection - point;

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
				{
					colliders.push_back(Collision(softBox, hardBox, point, hardEdge, pointOfIntersection, distance, timeOfImpact, totalDistanceFromEdgeToShape, getDirectionOfImpact(softBox, hardEdge, pointOfIntersection), Collision::CollisionType::POINT_EDGE));
				}
			}
		}

		// Check for edge-to-point collisions
		for (unsigned int j = 0; j < softBox->getPoints().size(); ++j)
		{
			// Get edge in soft box
			Edge softEdge = Edge(
				softBox->getPos() + softBox->getPoints()[j],
				softBox->getPos() + softBox->getPoints()[(j + 1) % softBox->getPoints().size()]
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
			double totalDistanceFromPointToShape = getTotalDistanceFromPointToShape(hardPoint, softBox);

			// Check if point is collinear to edge
			if (softEdge.onSegment(hardPoint))
			{
				// Clear colliders if we have a new minimum time of impact
				if (minTimeOfImpact != 0.0)
					colliders.clear();

				// Add new collider
				colliders.push_back(Collision(softBox, hardBox, hardPoint, softEdge, hardPoint, Vector2(), 0.0, totalDistanceFromPointToShape, getDirectionOfImpact(softBox, softEdge, hardPoint), Collision::CollisionType::EDGE_POINT));
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
			{
				colliders.push_back(Collision(softBox, hardBox, hardPoint, softEdge, pointOfIntersection, hardPoint - pointOfIntersection, timeOfImpact, totalDistanceFromPointToShape, getDirectionOfImpact(softBox, softEdge, hardPoint), Collision::CollisionType::EDGE_POINT));
			}
		}
	}

	if (colliders.size() == 0)
		return Collision();

	/*
	// Get rid of unnecessary edge-to-point collisions
	for (unsigned int i = 0; i < colliders.size(); ++i)
	{
		if (colliders[i].collisionType != Hitbox::CollisionType::EDGE_POINT)
			continue;

		for (auto& collider2 : colliders)
		{
			if (collider2 == colliders[i])
				continue;

			if (collider2.collisionType == Hitbox::CollisionType::EDGE_POINT)
				continue;

			if (colliders[i].point == collider2.edge.first || colliders[i].point == collider2.edge.second)
			{
				colliders.erase(colliders.begin() + i);
				--i;
				break;
			}
		}
	}
	





	// Narrow down colliders to one
	Collision bestCollider = colliders[0];
	for (auto& collider : colliders)
	{
		// Prioritize the edge that is closest to the point
		if (collider.totalDistanceFromEdgeToShape < bestCollider.totalDistanceFromEdgeToShape)
			bestCollider = collider;
	}

	/*
	if (bestCollider.direction != 2)
	{
		for (auto& collider : colliders)
		{
			std::cout << colliders.size() <<
				", [point: " << collider.point << "]" <<
				", [predictedPoint: " << collider.point + softBox->getVel() * deltaTime << "]" <<
				", [edge: " << collider.edge << "]" <<
				", [POI: " << collider.pointOfIntersection << "]" <<
				", [distance: " << collider.distance << "]" <<
				", [timeOfImpact=" << collider.timeOfImpact << "]" <<
				", [direction=" << collider.direction << "]" <<
				std::endl;
		}
	}
	*/

	/*
	std::cout <<
			"[point: " << bestCollider.point << "]" <<
			", [predictedPoint: " << bestCollider.point + softBox->getVel() * deltaTime << "]" <<
			", [edge: " << bestCollider.edge << "]" <<
			", [POI: " << bestCollider.pointOfIntersection << "]" <<
			", [distance: " << bestCollider.distance << "]" <<
			", [timeOfImpact=" << bestCollider.timeOfImpact << "]" <<
			", [direction=" << bestCollider.direction << "]" <<
			std::endl;
			


	return bestCollider;
}
*/

void WLUW::Physics::solveCollision(Collision collision)
{

}


