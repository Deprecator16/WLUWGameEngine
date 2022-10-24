/*********************************************************************
 * \file   WComponentBase.h
 * \brief  Component class
 *
 * \author Bassil Virk
 * \date   February 2022
 *********************************************************************/

#pragma once

#include "TypeIdManager.h"

namespace WLUW
{
	/**
	* \class WComponentBase WComponentBase.h
	* \brief Abstract parent ComponentBase class for all components
	*/
	class WComponentBase
	{
	private:
		static int id;	/* Unique ID for each component class */
		TypeIdManager<WComponentBase> idManager;

	protected:
		WComponentBase()
		{
			if (id == 0)
				id = idManager.getClassUniqueID<decltype(this)>();
		};

	public:
		virtual ~WComponentBase() = 0;

		//////////////
		//// Methods
		//////////////
		
		int getComponentId() { return id; }
	};

	inline WComponentBase::~WComponentBase() {}

	int WComponentBase::id = 0;
}

