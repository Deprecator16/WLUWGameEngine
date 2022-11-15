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

#include "Collision.h"
#include "Hitbox.h"
#include "SDL.h"
#include "Shape.h"
#include "WTexture.h"

#include <iostream>

namespace WLUW
{
	/**
	 * \class WObject WObject.h
	 * \brief Fundamental parent class for all objects and entities which can exist in the world.
	 */
	class WObject
	{
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

		// Assignment operators
		WObject& operator=(const WObject& other);
		WObject& operator=(WObject&& other) noexcept;

		// Update functions
		virtual void update(double deltaTime) = 0;
		virtual void render(SDL_Renderer* renderer) = 0;

		bool shouldDraw;
		bool isUIElement;

		// Event callbacks
		virtual void OnCollide(WObject* target, Collision collisionData) = 0;

		// Getters
		WTexture* getTexture() { return texture; }
		Hitbox* getHitbox() { return &hitbox; }

		void setTexture(WTexture* texture) { this->texture = texture; }

	protected:
		WTexture* texture;
		Hitbox hitbox;
	};
}