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

bool isOverlapping(Vector2 a, Vector2 b)
{
	if (b.x > a.y || a.x > b.y)
		return false;
	return true;
}

// Casts an edge and returns all hits
std::vector<WLUW::WObject*> edgecastAll(std::vector<WObject*> objects, Edge edge, Vector2 direction, float distance)
{
	std::vector<WObject*> objectsHit;

	Shape edgecastShape(edge.first);
	edgecastShape.addPoint(Vector2());
	edgecastShape.addPoint(edge.second - edge.first);
	edgecastShape.addPoint(edge.second + (direction * distance) - edge.first);
	edgecastShape.addPoint(direction * distance);

	// Loop through all objects
	for (auto& object : objects)
	{
		bool collisionDetected = false;

		// Check if object overlaps with edgecastShape
		std::pair<Vector2, float> mtv = Shape::checkCollision(*object->getHitbox(), edgecastShape);

		if (mtv.second != 0) // Collision detected
			collisionDetected = true;

		if (collisionDetected)
			objectsHit.push_back(object);
	}
	return objectsHit;
}

// Casts a shape and returns all hits
std::vector<WLUW::WObject*> WLUW::Physics::shapecastAll(std::vector<WObject*> objects, Shape shape, Vector2 direction, float distance)
{
	std::vector<WObject*> objectsHit;

	// Loop through all edges in shape
	for (unsigned int i = 0; i < shape.getPoints().size(); ++i)
	{
		Edge shapeEdge(Edge(
			shape.getPos() + shape.getPoints()[i],
			shape.getPos() + shape.getPoints()[(i + 1) % shape.getPoints().size()]
		));

		// Cast every edge
		std::vector<WObject*> edgecastHits = edgecastAll(objects, shapeEdge, direction, distance);
		for (auto& edgecastHit : edgecastHits)
		{
			// Check if object already exists in objectsHit
			if (std::find(objectsHit.begin(), objectsHit.end(), edgecastHit) == objectsHit.end())
				objectsHit.push_back(edgecastHit);
		}
	}

	return objectsHit;
}

std::vector<ContactPoint> linecastAll(Shape* shape, Vector2 start, Vector2 end, float fractionBase)
{
	std::vector<ContactPoint> contacts;
	Vector2 castDirection = (end - start).normalized();

	// Loop through all edges in object
	for (unsigned int i = 0; i < shape->getPoints().size(); ++i)
	{
		Edge edge(Edge(
			shape->getPos() + shape->getPoints()[i],
			shape->getPos() + shape->getPoints()[(i + 1) % shape->getPoints().size()]
		));

		// Continue if line is parallel to current edge
		if (Edge::areParallel(Edge(start - (castDirection * epsilon), end), edge))
			continue;

		/*
		// Continue if line moves away from current edge and not actually into the edge
		if (edge.onSegment(start))
		{
			Vector2 normal = edge.normal().normalized();
			float startProj = normal.dot(start);
			float endProj = normal.dot(end);
			if (endProj - startProj <= 0.0f)
				continue;
		}
		*/

		// Check for intersection
		if (Edge::areIntersecting(Edge(start - (castDirection * epsilon), end), edge))
		{
			// Find point of intersection of line with edge
			Vector2 pointOfIntersection;

			if (edge.onSegment(start))
				pointOfIntersection = start;
			else
				pointOfIntersection = Edge::getPointOfIntersection(Edge(start - (castDirection * epsilon), end), edge);

			// Determine contact point type
			ContactPoint::ContactType contactType = ContactPoint::ContactType::EDGE;
			if (pointOfIntersection == edge.first || pointOfIntersection == edge.second)
				contactType = ContactPoint::ContactType::POINT;

			Vector2 separation = pointOfIntersection - start;
			float fraction = separation.size() / fractionBase;

			// Add to contacts
			contacts.push_back(ContactPoint(pointOfIntersection, start, edge.normal().normalized(), separation, fraction, contactType));
		}
	}

	// Sort the hits by fraction
	std::sort(contacts.begin(), contacts.end(), ContactPoint::compare);

	return contacts;
}

ContactPoint linecast(Shape* shape, Vector2 start, Vector2 end, float fractionBase)
{
	std::vector<ContactPoint> contacts = linecastAll(shape, start, end, fractionBase);
	if (contacts.size() == 0)
		return ContactPoint();

	return contacts[0];
}

bool intersectsSelf(Shape* box, Vector2 start, Vector2 end)
{
	// Check if the line cast from current soft point will interesect an edge or another point in soft box
	for (unsigned int j = 0; j < box->getPoints().size(); ++j)
	{
		Vector2 testPoint = box->getPos() + box->getPoints()[j];
		Edge testEdge = Edge(
			box->getPos() + box->getPoints()[j],
			box->getPos() + box->getPoints()[(j + 1) % box->getPoints().size()]
		);

		if (testPoint == start) // Don't test the start point itself
			continue;

		// Test current point
		if (Edge(start, end).onSegment(testPoint))
			return true;

		if (testEdge.first == start || testEdge.second == start) // Don't test any edges that the start point is an endpoint of
			continue;

		// Test current edge
		if (Edge::areIntersecting(testEdge, Edge(start, end)))
			return true;
	}
	return false;
}

std::vector<ContactPoint> WLUW::Physics::getContactPoints(Shape* softBox, Shape* hardBox, Vector2 raycastDirection, float raycastDistance, float fractionBase)
{
	std::vector<ContactPoint> contacts;

	// Loop through all points in soft box
	for (unsigned int i = 0; i < softBox->getPoints().size(); ++i)
	{
		Vector2 softPoint = softBox->getPos() + softBox->getPoints()[i];

		// Check if the line cast from current soft point will interesect an edge or another point in soft box
		if (intersectsSelf(softBox, softPoint, softPoint + (raycastDirection * raycastDistance)))
			continue;

		// Get contact result from casting the line
		//ContactPoint newContact = linecastContact(hardBox, softPoint, softPoint + (direction * distance), distance);
		ContactPoint newContact = linecast(hardBox, softPoint, softPoint + (raycastDirection * raycastDistance), fractionBase);

		// Check if no contact was found
		if (newContact.contactType == ContactPoint::ContactType::NO_CONTACT)
			continue;

		// Check if point already exists in contacts
		bool exists = false;
		for (unsigned int j = 0; j < contacts.size(); ++j)
		{
			if (contacts[j].pointOfContact == newContact.pointOfContact) // Point exists
			{
				// Check which fraction is smaller
				if (newContact.fraction < contacts[j].fraction) // New hit is better, delete old hit
					contacts.erase(contacts.begin() + j);
				else // Existing hit is better
					exists = true;

				break;
			}
		}

		if (!exists) // Add new entry to list
			contacts.push_back(newContact);
	}

	// Loop through all points in hard box
	for (unsigned int i = 0; i < hardBox->getPoints().size(); ++i)
	{
		Vector2 hardPoint = hardBox->getPos() + hardBox->getPoints()[i];

		// Check if the line cast from current hard point will interesect an edge or another point in hard box
		if (intersectsSelf(hardBox, hardPoint, hardPoint - (raycastDirection * raycastDistance)))
			continue;

		// Get contact result from casting the line
		//ContactPoint newContact = linecastContact(softBox, hardPoint, hardPoint - (direction * distance), distance);
		ContactPoint newContact = linecast(softBox, hardPoint, hardPoint - (raycastDirection * raycastDistance), fractionBase);

		// Check if no contact was found
		if (newContact.contactType == ContactPoint::ContactType::NO_CONTACT)
			continue;

		// Modify point since we are casting backwards
		ContactPoint modifiedContact = ContactPoint(hardPoint, newContact.pointOfContact, newContact.normal, -newContact.separation, newContact.fraction, ContactPoint::ContactType::POINT);

		// Check if point already exists in contacts
		bool exists = false;
		for (unsigned int j = 0; j < contacts.size(); ++j)
		{
			if (contacts[j].pointOfContact == modifiedContact.pointOfContact) // Point exists
			{
				// Check which fraction is smaller
				if (modifiedContact.fraction < contacts[j].fraction) // New hit is better, delete old hit
					contacts.erase(contacts.begin() + j);
				else // Existing hit is better
					exists = true;

				break;
			}
		}

		if (!exists) // Add new entry to list
			contacts.push_back(modifiedContact);
	}

	/*
	// Remove contacts with a fraction greater than 1
	for (unsigned int i = 0; i < contacts.size(); ++i)
	{
		if (contacts[i].fraction > 1.0)
		{
			contacts.erase(contacts.begin() + i);
			--i;
		}
	}
	*/

	return contacts;
}

// Helper function that returns the direction of impact between a colliding box and edge
Collision::Direction getDirectionOfImpact(Hitbox* box, Vector2 point, Vector2 normal)
{
	// Get slope of colliding edge
	Collision::Direction direction = Collision::Direction::NO_DIRECTION;

	if (abs(normal.x) <= abs(normal.y) && point.y < box->getCenter().y)
		direction = Collision::Direction::TOP;
	if (abs(normal.x) <= abs(normal.y) && point.y > box->getCenter().y)
		direction = Collision::Direction::BOTTOM;
	if (abs(normal.x) > abs(normal.y) && point.x < box->getCenter().x)
		direction = Collision::Direction::LEFT;
	if (abs(normal.x) > abs(normal.y) && point.x > box->getCenter().x)
		direction = Collision::Direction::RIGHT;

	return direction;
}

Collision WLUW::Physics::getCollisionData(WObject* softObject, WObject* hardObject, float deltaTime)
{
	float hardBoundingBoxSize = hardObject->getHitbox()->getBoundingBoxSize();
	std::vector<ContactPoint> contacts = getContactPoints(softObject->getHitbox(), hardObject->getHitbox(), softObject->getHitbox()->getVel().normalized(), std::max(hardBoundingBoxSize * 2, 5000.0f), softObject->getHitbox()->getVel().size() * deltaTime);
	//std::vector<ContactPoint> contacts = getContactPoints(softObject->getHitbox(), hardObject->getHitbox(), softObject->getHitbox()->getVel().normalized(), 500.0f, 500.0f);

	// Check if no contacts were detected
	if (contacts.size() == 0)
	{
		if (debugOutput)
			std::cout << "NO CONTACTS" << std::endl;

		return Collision();
	}

	// Remove unnecessary contacts
	for (unsigned int i = 0; i < contacts.size(); ++i)
	{
		if (contacts[i].contactType == ContactPoint::ContactType::NO_CONTACT)
		{
			contacts.erase(contacts.begin() + i);
			--i;
		}
	}

	// Sort
	std::sort(contacts.begin(), contacts.end(), ContactPoint::compare);

	if (debugOutput)
	{
		std::cout << "CONTACTS ==================================================" << std::endl;
		for (auto& contact : contacts)
		{
			std::cout <<
				contacts.size() << " " <<
				"[pointOfContact = " << contact.pointOfContact << "], " <<
				"[pointDoingContact = " << contact.pointDoingContact << "], " <<
				"[normal=" << contact.normal << "], " <<
				"[separation=" << contact.separation << "], " <<
				"[fraction=" << contact.fraction << "], " <<
				"[contactType=" << contact.contactType << "], " <<
				"[pos=" << softObject->getHitbox()->getPos() << "], " <<
				"[vel=" << softObject->getHitbox()->getVel() << "]" <<
				std::endl;
		}
	}

	// Check if not enough contacts were detected
	if (contacts.size() < 2)
	{
		if (debugOutput)
			std::cout << "NOT ENOUGH CONTACTS" << std::endl;

		return Collision();
	}

	// Check for edge collisions
	if (contacts[0].contactType == ContactPoint::ContactType::EDGE)
	{
		if (debugOutput)
			std::cout << "SINGLE EDGE CONTACT" << std::endl;

		return Collision(softObject, hardObject, contacts[0].pointOfContact, contacts[0].normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].pointOfContact, contacts[0].normal), Collision::CollisionType::EDGE);
	}
	if (contacts.size() > 1)
	{
		if (contacts[1].fraction == contacts[0].fraction)
		{
			if (debugOutput)
				std::cout << "TWO POINT CONTACTS FORMING AN EDGE" << std::endl;

			Vector2 normal = (contacts[1].pointOfContact - contacts[0].pointOfContact).normal().normalized();
			return Collision(softObject, hardObject, contacts[0].pointOfContact, normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].pointOfContact, normal), Collision::CollisionType::EDGE);
		}
	}

	// We can now guarantee that:
	//     There's only one contact point with the smallest fracton
	//     That contact point is with a point and not an edge

	// Check for slant collision
	bool isSoftPoint = false;
	for (auto& point : softObject->getHitbox()->getPoints())
	{
		if (contacts[0].pointDoingContact == point)
		{
			isSoftPoint = true;
			break;
		}
	}

	if (isSoftPoint)
	{
		// Project all contact points onto the velocity's normal vector
		Vector2 velNormal = softObject->getHitbox()->getVel().normal().normalized();
		std::pair<ContactPoint, float> minProj = std::make_pair(contacts[0], velNormal.dot(contacts[0].pointOfContact));
		std::pair<ContactPoint, float> maxProj = std::make_pair(contacts[0], velNormal.dot(contacts[0].pointOfContact));
		for (auto& contact : contacts)
		{
			float proj = velNormal.dot(contact.pointOfContact);
			if (proj < minProj.second)
				minProj = std::make_pair(contact, proj);
			if (proj > maxProj.second)
				maxProj = std::make_pair(contact, proj);
		}

		if (minProj.first.pointOfContact == contacts[0].pointOfContact || maxProj.first.pointOfContact == contacts[0].pointOfContact)
		{
			if (debugOutput)
				std::cout << "SLANTED POINT CONTACTS FORMING AN EDGE" << std::endl;

			Vector2 normal = (contacts[1].pointOfContact - contacts[0].pointOfContact).normal().normalized();
			return Collision(softObject, hardObject, contacts[0].pointOfContact, normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].pointOfContact, contacts[0].normal), Collision::CollisionType::EDGE);
		}
	}

	// We have a point collision
	if (debugOutput)
		std::cout << "POINT CONTACT" << std::endl;

	return Collision(softObject, hardObject, contacts[0].pointOfContact, contacts[0].normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].pointOfContact, contacts[0].normal), Collision::CollisionType::POINT);
}

bool WLUW::Physics::clips(Hitbox* softBox, std::vector<WObject*> objects)
{
	// Check if softBox clips any hardBoxes
	for (auto& obj : objects)
	{
		// Check if a collision occurs
		std::pair<Vector2, float> mtv = Shape::checkCollision(*softBox, *obj->getHitbox());
		if (mtv.second == 0) // No collision
			continue;

		if (debugOutput)
			std::cout << "Object clips in direction: " << mtv.first << " by: " << mtv.second << std::endl;

		//softBox->setPos(softBox->getPos() - (mtv.first * mtv.second));

		return true;
	}

	return false;
}
