#include "WObject.h"
#include "TypeIdManager.h"

WLUW::WObject::WObject() : id(WLUW::TypeIdManager<WLUW::WObject>::getNewID())
{
}

WLUW::WObject::WObject(const WObject& obj) : id(WLUW::TypeIdManager<WLUW::WObject>::getNewID())
{
}

WLUW::WObject::WObject(WObject&& obj) noexcept
{
}

WLUW::WObject& WLUW::WObject::operator=(const WObject& other)
{
	return *this = WObject(other);
}

WLUW::WObject& WLUW::WObject::operator=(WObject&& other) noexcept
{
	if (this != &other)
	{
		this->id = other.id;

		other.id = 0;
	}

	return *this;
}

bool WLUW::operator==(const WLUW::WObject& lhs, const WLUW::WObject& rhs)
{
	return lhs.isEqual(rhs);
}

bool WLUW::WObject::isEqual(WObject other) const
{
	return this->id == other.id;
}

void WLUW::WObject::attachComponent(std::unique_ptr<WComponentBase> component)
{
	int key = component->getComponentId();
	componentMap.erase(key);
	componentMap.emplace(key, std::move(component));
}

int WLUW::WObject::removeComponent(int key)
{
	return componentMap.erase(key);
}
