#pragma once
#include "Vector2.h"

namespace WLUW
{
	class WObject
	{
	public:
		WObject();
		WObject(Vector2 position);

	private:
		Vector2 pos;
	};
}