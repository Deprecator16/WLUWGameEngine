/*********************************************************************
 * \file   TypeIdManager.h
 * \brief  Generate unique ID for any type. Modifed from https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/
 *
 * \author Bassil Virk
 * \date   February 2022
 *********************************************************************/

#pragma once

#include <functional>
#include <queue>
#include <set>
#include <vector>

namespace WLUW
{

    /**
    * \class TypeIdManager TypeIdManager.h
    * \tparam T Type to generate ID's for
    * \brief Manages creating and removing unique ID's for a specific type/class
    */
    template<class T>
    class TypeIdManager
    {
    private:

        static int count;                                                                   /* Tracks next ID to hand out */
        static int uniqueClassCounter;                                                      /* Tracks next ID to hand out for a unique class */
        static std::set<int> currentlyUsed;                                                 /* ID's which are currently in use */
        static std::priority_queue<int, std::vector<int>, std::greater<int>> reuseableIDs;  /* ID's which have been removed and can be reused in a new object */

    public:

        ///////////////
        //// Methods
        ///////////////

        /**
         * \brief Get a unique ID.
         *
         * \return Unique ID
         */
        static const int getNewID()
        {
            // Reassign an older, removed ID if there are any
            if (!reuseableIDs.empty())
            {
                int id = reuseableIDs.top();
                reuseableIDs.pop();
                currentlyUsed.insert(id);
                return id;
            }
            // Otherwise, create a new ID
            else
            {
                static const int id = count++;
                currentlyUsed.insert(id);
                return id;
            }
        }

        /**
         * \brief Remove an ID from use.
         *
         * \param id ID to remove.
         * \return True if ID removed, false if ID does not exist or was already removed
         */
        static const bool removeID(int id)
        {
            // If ID is in use
            if (currentlyUsed.count(id))
            {
                currentlyUsed.erase(id);
                    reuseableIDs.push(id);
                    return true;
            }
            // ID is not in use: Does not exist, or already removed
            else
            {
                return false;
            }
        }

        template<class U>
        static const int getClassUniqueID()
        {
            static int unique = 0;

            if (unique == 0)
                unique = uniqueClassCounter++;

            return unique;
        }
    };

    template<class T> int TypeIdManager<T>::count = 1;
    template<class T> int TypeIdManager<T>::uniqueClassCounter = 1;
}
