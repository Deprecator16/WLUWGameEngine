#include "WWorld.h"
#include "Shape.h"

#include <utility>
#include <memory>

using MTV = std::pair<WLUW::Vector2, double>;

void WLUW::WWorld::addWorldObject(std::unique_ptr<WObject> object)
{
	worldObjects.push_back(std::move(object));
}

std::unique_ptr<WLUW::WObject> WLUW::WWorld::removeWorldObject(int id)
{
	auto is_id = [id](std::unique_ptr<WObject> &obj) { return obj->getId() == id; };
	auto foundObject = std::find_if(std::begin(worldObjects), std::end(worldObjects), is_id);

	if (foundObject != std::end(worldObjects))
	{
		auto mover = std::make_move_iterator(foundObject);
		std::unique_ptr<WObject> ret = *mover;
		worldObjects.erase(mover.base());

		return ret;
	}

	return nullptr;
}

//void WLUW::WWorld::doCollisionDetection(double deltaTime)
//{
//	for (std::unique_ptr<WObject> &collider : worldObjects)
//	{
//		for (std::unique_ptr<WObject> &wobj : worldObjects)
//		{
//			MTV mtv = Shape::checkCollision(collider->getHitboxShape(), wobj->getHitboxShape());
//			collider->setHitboxShape(collider->getHitboxShape().getPosition() + mtv.first * mtv.second);
//		}
//	}
//}
