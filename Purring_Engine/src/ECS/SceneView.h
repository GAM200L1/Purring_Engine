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
	 \brief 
	 
	 \tparam ComponentTypes 	This variadic template, this allows the input of multiple
	 							components to give the ability to scope an iterator to
								the input component types
	*************************************************************************************/
	template<typename... ComponentTypes>
	struct SceneView
	{
		/*!***********************************************************************************
		 \brief Construct a new Scene View object
		 
		*************************************************************************************/
		SceneView() : p_entityManager(Engine::g_entityManager)
		{
			// checks if the number of components is zero
			if constexpr (sizeof...(ComponentTypes))
			{
				//unpacks all the component IDs provided by the user into a initializer list
				std::initializer_list<ComponentID> componentIDs = { p_entityManager->GetComponentID<ComponentTypes>() ... };
				for (const ComponentID& c : componentIDs)
				{
					m_components.emplace(c);
				}
			}
			else // if no components are provided, it assume the user wants to scope to all components
			{
				m_all = true;
			}
		}
		
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
			Iterator(EntityID index, const std::set<ComponentID>& components, bool all) :
				p_entityManager(Engine::g_entityManager), m_index(index), m_components(components), m_all(all)
			{
			}

			/*!***********************************************************************************
			 \brief Returns the Entity ID at the current iterator position.
			 
			 \return EntityID ID of the entity
			*************************************************************************************/
			EntityID operator* () const
			{
				return m_index;
			}

			/*!***********************************************************************************
			 \brief operator== overload, checks if the two iterators are the same.
			 
			 \param[in] rhs the iterator to compare to this iterator
			 \return true 	iterators are the same
			 \return false 	iterators are different
			*************************************************************************************/
			bool operator== (const Iterator& rhs) const
			{
				return (m_index == rhs.m_index) || (m_index == p_entityManager->Size());
			}

			/*!***********************************************************************************
			 \brief operator!= overload, checks if the two iterators are different
			 
			 \param[in] rhs the iterators to comapre to this iterator
			 \return true 	the iterators are different
			 \return false  the iterators are the same
			*************************************************************************************/
			bool operator!= (const Iterator& rhs) const
			{
				return (m_index != rhs.m_index) || (m_index != p_entityManager->Size());
			}

			/*!***********************************************************************************
			 \brief Checks if the entity at the current iterator has all the scoped components
			 
			 \return true 	current iterator's entity has all the components within this scope
			 \return false 	current iterator's entity does not have all the components within this
			 				scope
			*************************************************************************************/
			bool HasComponents() const
			{
				for (const ComponentID& components : m_components)
				{
					if (!p_entityManager->GetComponentPoolPointer(components)->HasEntity(m_index))
						return false;
				}
				return true;
			}

			/*!***********************************************************************************
			 \brief Is the current iterator's entity a valid entity/index as well as contains all
			 		the scoped components
			 
			 \return true 	entity is valid
			 \return false 	entity is not valid
			*************************************************************************************/
			bool ValidIndex() const
			{
				return (p_entityManager->IsEntityValid(m_index)) &&
					(m_all || (HasComponents()));
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
					++m_index;
				} while (m_index < p_entityManager->Size() && !ValidIndex());
				return *this;
			}
			
			// ptr to the entity manager
			Engine::EntityManager* p_entityManager;
			// the current index/entity
			EntityID m_index;
			// the set of components in this scope
			const std::set<ComponentID>& m_components;
			// flag for toggling whether all components are in scope
			bool m_all{ false };
		};
		
		/*!***********************************************************************************
		 \brief Checks if entity at index has all the components within this scope
		 
		 \param[in] index index of the entity to check
		 \return true 	  the entity has all the components in this scope
		 \return false 	  the entity does not have all the components in this scope
		*************************************************************************************/
		bool HasComponents(size_t index) const
		{
			for (const ComponentID& components : m_components)
			{
				if (!p_entityManager->GetComponentPoolPointer(components)->HasEntity(index))
					return false;
			}
			return true;
		}

		/*!***********************************************************************************
		 \brief Generates a begin iterator to the first entity that matches the scope
		 
		 \return const Iterator the iterator for the first "legal" entity
		*************************************************************************************/
		const Iterator begin() const
		{
			if (!p_entityManager->Size())
				throw; // update to error log
			size_t firstIndex{};
			while (firstIndex < p_entityManager->Size() &&
				(!HasComponents(firstIndex)) ||
				(!p_entityManager->IsEntityValid(firstIndex))
				)
			{
				++firstIndex;
			}
			return Iterator(firstIndex, m_components, m_all);
		}

		/*!***********************************************************************************
		 \brief Generates the last entity
		 
		 \return const Iterator the iterator for the last "legal" entity
		*************************************************************************************/
		const Iterator end() const
		{
			return Iterator(p_entityManager->Size(), m_components, m_all);
		}

		// ptr to the entity manager
		Engine::EntityManager* p_entityManager{ nullptr };
		// the components for this scope
		std::set<ComponentID> m_components;
		// flag for toggling whether all components are in scope
		bool m_all{ false };
	};
}