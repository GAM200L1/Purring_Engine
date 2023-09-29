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
#include "prpch.h"
#include "Entity.h"
#include "Components.h"


namespace PE
{

/*!***********************************************************************************
 \brief Helper function to sort all duplicates to the "left" and push all the rest
		to the "right", will return an iterator to the new "end".
		Takes in a sorted vector/container.
 
 \tparam Iter Iterator to the new end
*************************************************************************************/
template<class Iter>
/** Moves duplicates to front, returning end of duplicates range.
 *  Use a sorted range as input. */
Iter Duplicates(Iter begin, Iter end) {
	Iter dup = begin;
	for (Iter it = begin; it != end; ++it) {
		Iter next = it;
		++next;
		Iter const miss = std::mismatch(next, end, it).second;
		if (miss != it) {
			*dup++ = *miss;
			it = miss;
		}
	}
	return dup;
}

/*!***********************************************************************************
 \brief A compare function (ascending order) for use inside qsort. (follows the 
 		requirements of it as well, hence the void ptr inputs)
		Compares p_lhs and p_rhs. 
 
 \param[in] p_lhs 	Left hand value
 \param[in] p_rhs 	Right hand value
 \return int 		1 = p_lhs > p_rhs, -1 = p_lhs < p_rhs, 0 otherwise
*************************************************************************************/
int compare(const void* p_lhs, const void* p_rhs);

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
		std::set<ComponentID> components;
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
			Iterator(EntityID index, const std::set<ComponentID>& r_components, bool all) :
				p_entityManager(&EntityManager::GetInstance()), index(index), all(all)
			{
				poolIdx = (all)? p_entityManager->GetEntitiesInPool("All") : p_entityManager->GetEntitiesInPool((*r_components.begin()));
				poolIdx.emplace_back(p_entityManager->OnePast());
			}

			/*!***********************************************************************************
			\brief Returns the Entity ID at the current iterator position.

			\return EntityID ID of the entity
			*************************************************************************************/
			EntityID operator* () const
			{
				return index;
			}

			/*!***********************************************************************************
			\brief operator== overload, checks if the two iterators are the same.

			\param[in] rhs the iterator to compare to this iterator
			\return true 	iterators are the same
			\return false 	iterators are different
			*************************************************************************************/
			bool operator== (const Iterator& r_rhs) const
			{
				return (index == r_rhs.index) || (index == p_entityManager->OnePast());
			}

			/*!***********************************************************************************
			\brief operator!= overload, checks if the two iterators are different

			\param[in] rhs the iterators to comapre to this iterator
			\return true 	the iterators are different
			\return false  the iterators are the same
			*************************************************************************************/
			bool operator!= (const Iterator& r_rhs) const
			{
				return (index != r_rhs.index) || (index != p_entityManager->OnePast());
			}


			/*!***********************************************************************************
			\brief operator++ overload, it increments the iterator until it reaches a valid
					entity, or it reaches the limits of the iterator

			\return Iterator& reference to this, emulating normal operator++ behaviour
			*************************************************************************************/
			Iterator& operator++()
			{
				if ((poolIdx[idxIterator] != p_entityManager->OnePast()))
					++idxIterator;
				
				index = poolIdx[idxIterator];
				return *this;
			}

			// ptr to the entity manager
			PE::EntityManager* p_entityManager;
			// The stored vector of entities for this iterator
			std::vector<EntityID> poolIdx;
			// The current iteration inside poolIdx
			size_t idxIterator{};
			// the current index/entity
			EntityID index{};
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
					components.emplace(c);
				}
			}
			else // if no components are provided, it assume the user wants to scope to all components
			{
				all = true;
			}
		}
		// ----- Public Methods ----- //

		/*!***********************************************************************************
		\brief Checks if entity at index has all the components within this scope

		\param[in] index index of the entity to check
		\return true 	  the entity has all the components in this scope
		\return false 	  the entity does not have all the components in this scope
		*************************************************************************************/
		bool HasComponents(size_t index) const
		{
			for (const ComponentID& r_component : components)
			{
				if (!p_entityManager->GetComponentPoolPointer(r_component)->HasEntity(index))
					return false;
			}
			return true;
		}

		/*!***********************************************************************************
		\brief Generates a begin iterator to the first entity that matches the scope

		\return const Iterator the iterator for the first "legal" entity
		*************************************************************************************/
		const Iterator begin() const	// cannot follow coding conventions due to c++ begin() & end() standards
		{
			if (!p_entityManager->Size())
				Iterator(0, components, all); // update to error log
			size_t firstIndex{};
			while ((firstIndex < p_entityManager->OnePast()) &&
				(!HasComponents(firstIndex) ||
					!p_entityManager->IsEntityValid(firstIndex))
				)
			{
				++firstIndex;
			}
			return Iterator(firstIndex, components, all);
		}

		/*!***********************************************************************************
		\brief Generates the last entity

		\return const Iterator the iterator for the last "legal" entity
		*************************************************************************************/
		const Iterator end() const		// cannot follow coding conventions due to c++ begin() & end() standards
		{
			return Iterator(p_entityManager->OnePast(), components, all);
		}
	};
}