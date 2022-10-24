/*********************************************************************
 * \file   WObject.h
 * \brief  Fundamental parent class for all objects and entities which can exist in the world.
 *
 * \author Bassil Virk
 * \date   February 2022
 *********************************************************************/

#pragma once

#include <memory>
#include <map>

#include "Shape.h"
#include "WComponentBase.h"

namespace WLUW
{
	/**
	 * \class WObject WObject.h
	 * \brief Fundamental parent class for all objects and entities which can exist in the world.
	 */
	class WObject
	{
	public:
		std::map<int, std::unique_ptr<WComponentBase>> componentMap;	/* List of owned components */
	private:
		int id = 0;														/* Unique Object ID */

	public:
		/////////////////////
		//// Constructors
		/////////////////////

		/**
		 * \brief Default constructor
		 */
		WObject();

		/**
		 * \brief Copy constructor
		 *
		 * \param obj Object to copy from
		 */
		WObject(const WObject& obj);

		/**
		 * \brief Move constructor
		 *
		 * \param obj Object to move
		 */
		WObject(WObject&& obj) noexcept;

		///////////////////////////
		//// Operators/Assignments
		///////////////////////////

		// Copy assignment
		WObject& operator=(const WObject& other);

		// Move assignment
		WObject& operator=(WObject&& other) noexcept;

		// Equality operator
		friend bool operator==(const WObject& lhs, const WObject& rhs);

		///////////////////
		//// Methods
		///////////////////

		/**
		 * \brief Checks if this object and another are equal
		 * 
		 * \param other Other object to compare to
		 * \return Equal or not
		 */
		virtual bool isEqual(WObject other) const;

		/**
		 * \brief Attaches a component to this object.
		 * 
		 * \param component Component to attach
		 */
		void attachComponent(std::unique_ptr<WComponentBase> component);

		/**
		 * \brief Remove a component by key.
		 * 
		 * \param key Key of component to remove
		 * \return 0 if component was not in map, 1 if it was and it was removed
		 */
		int removeComponent(int key);

		/////////////////////
		//// Getters/Setters
		/////////////////////

		/**
		 * \brief Get ID
		 */
		int getId() const { return id; };
	};
}