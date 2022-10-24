/*****************************************************************//**
 * \file   WWorld.h
 * \brief  Class which manages all aspects of the game world
 *
 * \author Bassil Virk
 * \date   February 2022
 *********************************************************************/

#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>
#include <memory>

#include "WObject.h"

namespace WLUW
{
	/**
	 * \class WWorld WWorld.h
	 * \brief Game world manager
	 */
	class WWorld
	{
	public:
		/////////////////////
		//// Constructors
		/////////////////////
		
		/**
		 * \brief Default constructor
		 */
		WWorld()
		{
		}

		//////////////////////
		//// Modifier Methods
		//////////////////////
		
		/**
		 * \brief Add dynamic (moveable) WObject to world
		 * 
		 * \param object WObject to add
		 */
		void addWorldObject(std::unique_ptr<WLUW::WObject> object);

		/**
		 * \brief Remove a dynamic (moveable) WObject from world by ID.
		 * 
		 * \param id ID of object to remove
		 * \return Removed WObject
		 */
		std::unique_ptr<WLUW::WObject> removeWorldObject(int id);

		/**
		 * \brief Rudimentary collision detection for all dynamic objects. Should be replaced by component system
		 * \todo Replace with components system
		 */
		//void doCollisionDetection(double deltaTime);

	private:
		std::vector<std::unique_ptr<WLUW::WObject>> worldObjects;
	};
}

