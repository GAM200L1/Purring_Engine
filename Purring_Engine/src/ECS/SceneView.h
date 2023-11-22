/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SceneView.h
 \date     13-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 	This file contains the implementation of SceneView.
			Its main use is to allow the iteration through the list of entities,
			while scoped to the user specified Component types.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once
#include "Entity.h"
#include "Components.h"


namespace PE
{
	/*!***********************************************************************************
	\brief

	\tparam ComponentTypes 	This variadic template, this allows the input of multiple
								components to give the ability to scope an iterator to
								the input component types
	*************************************************************************************/
	template<typename... ComponentTypes>
	struct SceneView
	{
		// ----- Public Variables ----- // 
		// ptr to the entity manager
		PE::EntityManager* p_entityManager{ nullptr };
		// the components for this scope
		ComponentID componentsCombined;
		// flag for toggling whether all components are in scope
		bool all{ false };

		/*!***********************************************************************************
		\brief		This iterator struct is a required interface to allow the use of foreach
					loops.

		*************************************************************************************/
		struct Iterator
		{
			/*!***********************************************************************************
			\brief Construct a new Iterator object

			\param[in] index 		The index to start from
			\param[in] components 	The components to scope to
			\param[in] all 		Whether or not the scope is to all copmonents
			*************************************************************************************/
			Iterator(bool index, const ComponentID& r_components, bool all) :
				p_entityManager(&EntityManager::GetInstance()), all(all)
			{
				if (index)
				{
					poolIdx = (all) ? p_entityManager->GetEntitiesInPool(ALL).begin() : p_entityManager->GetEntitiesInPool(r_components).begin();
					endIdx = (all) ? p_entityManager->GetEntitiesInPool(ALL).end() : p_entityManager->GetEntitiesInPool(r_components).end();
				}
				else
				{
					poolIdx = (all) ? p_entityManager->GetEntitiesInPool(ALL).end() : p_entityManager->GetEntitiesInPool(r_components).end();
					endIdx = (all) ? p_entityManager->GetEntitiesInPool(ALL).end() : p_entityManager->GetEntitiesInPool(r_components).end();
				}
				while (poolIdx != endIdx && !p_entityManager->Get<EntityDescriptor>(*poolIdx).isAlive) ++poolIdx;
			}

			/*!***********************************************************************************
			\brief Returns the Entity ID at the current iterator position.

			\return EntityID ID of the entity
			*************************************************************************************/
			EntityID operator* () const
			{
				return *poolIdx;
			}

			/*!***********************************************************************************
			\brief operator== overload, checks if the two iterators are the same.

			\param[in] rhs the iterator to compare to this iterator
			\return true 	iterators are the same
			\return false 	iterators are different
			*************************************************************************************/
			bool operator== (const Iterator& r_rhs) const
			{
				return (poolIdx == endIdx) || (poolIdx == r_rhs.poolIdx);
			}

			/*!***********************************************************************************
			\brief operator!= overload, checks if the two iterators are different

			\param[in] rhs the iterators to comapre to this iterator
			\return true 	the iterators are different
			\return false  the iterators are the same
			*************************************************************************************/
			bool operator!= (const Iterator& r_rhs) const
			{
				return (poolIdx != endIdx) || (poolIdx != r_rhs.poolIdx);
			}


			/*!***********************************************************************************
			\brief operator++ overload, it increments the iterator until it reaches a valid
					entity, or it reaches the limits of the iterator

			\return Iterator& reference to this, emulating normal operator++ behaviour
			*************************************************************************************/
			Iterator& operator++()
			{
				do 
				{
					++poolIdx;
					if (poolIdx != endIdx && p_entityManager->Get<EntityDescriptor>(*poolIdx).isAlive)
						break;
				} while (poolIdx != endIdx);
					
				return *this;
			}

			// ptr to the entity manager
			PE::EntityManager* p_entityManager;
			// The stored vector of entities for this iterator
			std::vector<EntityID>::iterator poolIdx;
			// end of the vector
			std::vector<EntityID>::iterator endIdx;
			// flag for toggling whether all components are in scope
			bool all{ false };
		};
		
		// ----- Constructors ------ //

		/*!***********************************************************************************
		\brief Construct a new Scene View object

		*************************************************************************************/
		SceneView() : p_entityManager(&EntityManager::GetInstance())
		{
			// checks if the number of components is zero
			if constexpr (sizeof...(ComponentTypes))
			{
				//unpacks all the component IDs provided by the user into a initializer list
				std::initializer_list<ComponentID> componentIDs = { p_entityManager->GetComponentID<ComponentTypes>() ... };
				for (const ComponentID& c : componentIDs)
				{
					componentsCombined |= c;
				}
			}
			else // if no components are provided, it assume the user wants to scope to all components
			{
				all = true;
			}
		}
		// ----- Public Methods ----- //


		/*!***********************************************************************************
		\brief Generates a begin iterator to the first entity that matches the scope

		\return const Iterator the iterator for the first "legal" entity
		*************************************************************************************/
		const Iterator begin() const	// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(true, componentsCombined, all);
		}

		/*!***********************************************************************************
		\brief Generates the last entity

		\return const Iterator the iterator for the last "legal" entity
		*************************************************************************************/
		const Iterator end() const		// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(false, componentsCombined, all);
		}
	};
}