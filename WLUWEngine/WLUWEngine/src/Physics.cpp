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

// Casts an edge and returns all hits
std::vector<WLUW::WObject*> edgecastAll(std::vector<WObject*> objects, Edge edge, Vector2 direction, double distance)
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
		std::pair<Vector2, double> mtv = Shape::checkCollision(*object->getHitbox(), edgecastShape);

		if (mtv.second != 0) // Collision detected
			collisionDetected = true;

		if (collisionDetected)
			objectsHit.push_back(object);
	}
	return objectsHit;
}

// Casts a shape and returns all hits
std::vector<WLUW::WObject*> WLUW::Physics::shapecastAll(std::vector<WObject*> objects, Shape shape, Vector2 direction, double distance)
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
			bool exists = false;
			for (auto& obj : objectsHit)
			{
				if (edgecastHit->getHitbox() == obj->getHitbox())
				{
					exists = true;
					break;
				}
			}
			if (exists)
				continue;

			objectsHit.push_back(edgecastHit);
		}
	}

	return objectsHit;
}

std::vector<ContactPoint> linecastContacts(Shape* shape, Vector2 start, Vector2 end)
{
	std::vector<ContactPoint> contacts;

	// Loop through all edges in object
	for (unsigned int i = 0; i < shape->getPoints().size(); ++i)
	{
		Edge edge(Edge(
			shape->getPos() + shape->getPoints()[i],
			shape->getPos() + shape->getPoints()[(i + 1) % shape->getPoints().size()]
		));

		if (Edge::areIntersecting(Edge(start, end), edge))
		{
			// Find point of intersection of line with edge
			Vector2 pointOfIntersection = Edge::getPointOfIntersection(Edge(start, end), edge);

			// Determine contact point type
			ContactPoint::ContactType contactType = ContactPoint::ContactType::EDGE;
			if (pointOfIntersection == edge.first || pointOfIntersection == edge.second)
				contactType = ContactPoint::ContactType::POINT;

			// Add to contacts
			contacts.push_back(ContactPoint(pointOfIntersection, edge.normal().normalized(), pointOfIntersection - start, (pointOfIntersection - start).size() / (end - start).size(), contactType));
		}
	}

	// Sort the hits by fraction
	std::sort(contacts.begin(), contacts.end(), ContactPoint::compare);

	return contacts;
}

ContactPoint linecastContact(Shape* shape, Vector2 start, Vector2 end)
{
	std::vector<ContactPoint> contacts = linecastContacts(shape, start, end);
	if (contacts.size() == 0)
		return ContactPoint();
	return contacts[0];
}

std::vector<ContactPoint> WLUW::Physics::getContactPoints(Shape* softBox, Shape* hardBox, Vector2 direction, double distance)
{
	std::vector<ContactPoint> contacts;

	// Loop through all points in soft box
	for (unsigned int i = 0; i < softBox->getPoints().size(); ++i)
	{
		Vector2 softPoint = softBox->getPos() + softBox->getPoints()[i];

		// Check if the line cast from current soft point will interesect an edge or another point in soft box
		bool validPoint = true;
		for (unsigned int j = 0; j < softBox->getPoints().size(); ++j)
		{
			Vector2 testPoint = softBox->getPos() + softBox->getPoints()[j];
			Edge testEdge = Edge(
				softBox->getPos() + softBox->getPoints()[j],
				softBox->getPos() + softBox->getPoints()[(j + 1) % softBox->getPoints().size()]
			);

			if (testPoint == softPoint) // Don't test the soft point itself
				continue;

			// Test current point
			if (Edge(softPoint, softPoint + (direction * distance)).onSegment(testPoint))
			{
				validPoint = false;
				break;
			}

			if (testEdge.first == softPoint || testEdge.second == softPoint) // Don't test any edges that the soft point is an endpoint of
				continue;

			// Test current edge
			if (Edge::areIntersecting(testEdge, Edge(softPoint, softPoint + (direction * distance))))
			{
				validPoint = false;
				break;
			}
		}

		if (!validPoint) // Point was invalidated in the checks above
			continue;

		// Cast every line
		ContactPoint newContact = linecastContact(hardBox, softPoint, softPoint + (direction * distance));

		// Check if no contact was found
		if (newContact.contactType == ContactPoint::ContactType::NO_CONTACT)
			continue;

		// Check if point already exists in contacts
		bool exists = false;
		for (unsigned int j = 0; j < contacts.size(); ++j)
		{
			if (newContact.point == contacts[j].point) // Point exists
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
		bool validPoint = true;
		for (unsigned int j = 0; j < hardBox->getPoints().size(); ++j)
		{
			Vector2 testPoint = hardBox->getPos() + hardBox->getPoints()[j];
			Edge testEdge = Edge(
				hardBox->getPos() + hardBox->getPoints()[j],
				hardBox->getPos() + hardBox->getPoints()[(j + 1) % hardBox->getPoints().size()]
			);

			if (testPoint == hardPoint) // Don't test the hard point itself
				continue;

			// Test current point
			if (Edge(hardPoint, hardPoint - (direction * distance)).onSegment(testPoint))
			{
				validPoint = false;
				break;
			}

			if (testEdge.first == hardPoint || testEdge.second == hardPoint) // Don't test any edges that the hard point is an endpoint of
				continue;

			// Test current edge
			if (Edge::areIntersecting(testEdge, Edge(hardPoint, hardPoint - (direction * distance))))
			{
				validPoint = false;
				break;
			}
		}
		
		if (!validPoint) // Point was invalidated in the checks above
			continue;

		// Cast every line
		ContactPoint newContact = linecastContact(softBox, hardPoint, hardPoint - (direction * distance));

		// Check if no contact was found
		if (newContact.contactType == ContactPoint::ContactType::NO_CONTACT)
			continue;

		// Modify point since we are casting backwards
		ContactPoint modifiedContact = ContactPoint(hardPoint, newContact.normal, -newContact.separation, newContact.fraction, ContactPoint::ContactType::POINT);

		// Check if point already exists in contacts
		bool exists = false;
		for (unsigned int j = 0; j < contacts.size(); ++j)
		{
			if (modifiedContact.point == contacts[j].point) // Point exists
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

Collision WLUW::Physics::getCollisionData(WObject* softObject, WObject* hardObject, double deltaTime)
{
	std::vector<ContactPoint> contacts = getContactPoints(softObject->getHitbox(), hardObject->getHitbox(), softObject->getHitbox()->getVel().normalized(), softObject->getHitbox()->getVel().size() * deltaTime);

	// Check if no contacts were detected
	if (contacts.size() == 0)
		return Collision();

	// Check for edge collisions
	if (contacts[0].contactType == ContactPoint::ContactType::EDGE)
		return Collision(softObject, hardObject, contacts[0].point, contacts[0].normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].point, contacts[0].normal), Collision::CollisionType::EDGE);
	if (contacts.size() > 1)
	{
		if (contacts[0].fraction == contacts[1].fraction)
		{
			Vector2 normal = (contacts[1].point - contacts[0].point).normal().normalized();
			return Collision(softObject, hardObject, contacts[0].point, normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].point, normal), Collision::CollisionType::EDGE);
		}
	}

	// We can now guarantee that:
	//     There's only one contact point with the smallest fracton
	//     That contact point is with a point and not an edge
	Edge edge1, edge2;
	for (unsigned int i = 0; i < hardObject->getHitbox()->getPoints().size(); ++i)
	{
		// Get the two edges that the contact point is an endpoint of
		if (hardObject->getHitbox()->getPos() + hardObject->getHitbox()->getPoints()[i] == contacts[0].point)
		{
			edge1 = Edge(
				hardObject->getHitbox()->getPos() + hardObject->getHitbox()->getPoints()[(i + hardObject->getHitbox()->getPoints().size() - 1) % hardObject->getHitbox()->getPoints().size()],
				hardObject->getHitbox()->getPos() + hardObject->getHitbox()->getPoints()[i]
			);
			edge2 = Edge(
				hardObject->getHitbox()->getPos() + hardObject->getHitbox()->getPoints()[i],
				hardObject->getHitbox()->getPos() + hardObject->getHitbox()->getPoints()[(i + 1) % hardObject->getHitbox()->getPoints().size()]
			);

			break;
		}
	}

	// Check if any other contact points are along the two edges above
	ContactPoint edge1Contact, edge2Contact;
	for (auto& contact : contacts)
	{
		if (edge1Contact.contactType == ContactPoint::ContactType::NO_CONTACT && edge1.onSegment(contact.point))
			edge1Contact = contact;
		if (edge2Contact.contactType == ContactPoint::ContactType::NO_CONTACT && edge2.onSegment(contact.point))
			edge2Contact = contact;
	}

	// Only one edge contains a contact point
	// edge1 has contact
	if (edge1Contact.contactType != ContactPoint::ContactType::NO_CONTACT && edge2Contact.contactType == ContactPoint::ContactType::NO_CONTACT)
	{
		Vector2 normal = (edge1Contact.point - contacts[0].point).normal().normalized();
		return Collision(softObject, hardObject, contacts[0].point, normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].point, normal), Collision::CollisionType::EDGE);
	}
	// edge2 has contact
	if (edge1Contact.contactType == ContactPoint::ContactType::NO_CONTACT && edge2Contact.contactType != ContactPoint::ContactType::NO_CONTACT)
	{
		Vector2 normal = (edge2Contact.point - contacts[0].point).normal().normalized();
		return Collision(softObject, hardObject, contacts[0].point, normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].point, normal), Collision::CollisionType::EDGE);
	}

	// Both edges contain contact points, we have a point collision
	if (edge1Contact.contactType != ContactPoint::ContactType::NO_CONTACT && edge2Contact.contactType != ContactPoint::ContactType::NO_CONTACT)
		return Collision(softObject, hardObject, contacts[0].point, contacts[0].normal, contacts[0].separation, contacts[0].fraction, getDirectionOfImpact(softObject->getHitbox(), contacts[0].point, contacts[0].normal), Collision::CollisionType::POINT);

	return Collision();
}
