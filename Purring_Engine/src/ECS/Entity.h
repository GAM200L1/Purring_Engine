/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Entity.h
 \date     03-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 	Contains the implementation of an Entity(m_entities) which is just a set
			of entity IDs', the individual pools have the knowledge of which entity
			has which component.

			Also contains the implementation of SceneView, where it allows the user
			to scope an iterator to the defined components.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

// Helpful links:
// https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html
// https://stackoverflow.com/questions/1554774/create-new-c-object-at-specific-memory-address

// INCLUDES
#include "prpch.h"
#include "Components.h"
#include "Data/SerializationManager.h"


// Typedefs
typedef unsigned long long EntityID;				// typedef for storing the unique ID of the entity, same as size_t
typedef std::string ComponentID;					// ComponentID type, internally it is a std::string

namespace PE
{
	/*!***********************************************************************************
	 \brief Entity manager struct
	 
	*************************************************************************************/
	class EntityManager
	{
	// ----- Constructors ----- //
	public:
		/*!***********************************************************************************
		 \brief Construct a new Entity Manager object
		 		Handles the instancing of the entity manager as well
				if an additional instance was created, an exception will be thrown

		*************************************************************************************/
		EntityManager();
		/*!***********************************************************************************
		 \brief Destroy the Entity Manager object
		 		Handles the deleteion of the component pools
		 
		*************************************************************************************/
		~EntityManager();
	// ----- Public Methods -----//
	public:
		/*!***********************************************************************************
		 \brief Creates a new entity and returns its' ID
		 
		 \return EntityID ID of the generated entity
		*************************************************************************************/
		EntityID NewEntity();
		
		/*!************************************************************************
		 \brief 	Assigns components to an entity

		 \tparam T 			Component to assign
		 \param[in] id 		Entity to assign component to
		 \return T* 		A pointer to the component generated
		**************************************************************************/
		template<typename T>
		T* Assign(EntityID id);
		
		/*!***********************************************************************************
		 \brief 	Assigns a componnet to an enity, and initializes it's component with val
		 
		 \tparam T 		Type of component to assign
		 \param[in] id 	The id of the entity to asign it to
		 \param[in] val The value to initialize the component with
		 \return T* 	The created entity
		*************************************************************************************/
		template<typename T>
		T* Assign(EntityID id, T const& val);

		/*!***********************************************************************************
		 \brief Assign an entity with a component specified by the componentID & the 
		 		createor input parameters
		 
		 \param[in] id 				ID of the entity to assign components to
		 \param[in] componentID 	The ID of the component to assign 
		*************************************************************************************/
		void Assign(const EntityID& id, const char* componentID);


		/*!***********************************************************************************
		 \brief Assign an entity with a component specified by the componentID 
		 
		 \param[in] id 				ID of the entity to asign components to
		 \param[in] componentID 	The component to assign
		*************************************************************************************/
		void Assign(const EntityID& id, const ComponentID& componentID);

		/*!***********************************************************************************
		 \brief Get the Component ID
		 
		 \tparam T 				The component to Get the ID of
		 \return ComponentID 	The component's id
		*************************************************************************************/
		template<typename T>
		ComponentID GetComponentID() const;
		
		/*!***********************************************************************************
		 \brief Get a pointer to a specific component pool
		 
		 \tparam T 				The pool to Get
		 \return ComponentPool* The pointer to the pool
		*************************************************************************************/
		template<typename T>
		ComponentPool* GetComponentPoolPointer();

		/*!***********************************************************************************
		 \brief Get a pointer to a specific component pool (const)
		 
		 \param[in] component 			The pool to Get
		 \return const ComponentPool* 	The pointer to the pool
		*************************************************************************************/
		const ComponentPool* GetComponentPoolPointer(const ComponentID& component) const;
		
		/*!***********************************************************************************
		 \brief Get a pointer to a specific component pool
		 
		 \param[in] component 			The pool to Get
		 \return const ComponentPool* 	The pointer to the pool
		*************************************************************************************/
		ComponentPool* GetComponentPoolPointer(const ComponentID& component);


		/*!***********************************************************************************
		 \brief Get a reference to a component pool
		 
		 \tparam T 				The pool to Get
		 \return ComponentPool& The component pool
		*************************************************************************************/
		template<typename T>
		ComponentPool& GetComponentPool();

		/*!***********************************************************************************
		 \brief Get a reference to a component pool (const)
		 
		 \tparam T 						The pool to Get
		 \return const ComponentPool& 	The component pool
		*************************************************************************************/
		template<typename T>
		const ComponentPool& GetComponentPool() const;

		/*!***********************************************************************************
		 \brief Get a pointer to the entity's component
		 
		 \tparam T 		Component to Get
		 \param[in] id 	Entity to Get the component from
		 \return T* 	Pointer to the component (nullptr if not found)
		*************************************************************************************/
		template<typename T>
		T* GetPointer(EntityID id);

		/*!***********************************************************************************
		 \brief Get a pointer to the entity's component (const)

		 \tparam T 			Component to Get
		 \param[in] id 		Entity to Get the component from
		 \return const T* 	Pointer to the component (nullptr if not found)
		*************************************************************************************/
		template<typename T>
		const T* GetPointer(EntityID id) const;

		/*!***********************************************************************************
		 \brief Get a reference to the entity's component
		 
		 \tparam T 		Component to Get
		 \param[in] id 	Entity to Get the component from
		 \return T& 	Reference to the component
		*************************************************************************************/
		template<typename T>
		T& Get(EntityID id);

		/*!***********************************************************************************
		 \brief Get a reference to the entity's component (const)
		 
		 \tparam T 			Component to Get
		 \param[in] id 		Entity to Get the component from
		 \return const T& 	Const reference to the component
		*************************************************************************************/
		template<typename T>
		const T& Get(EntityID id) const;

		/*!***********************************************************************************
		 \brief Copies a component from src entity into dest entity
		 		If the destination does not have the the component, it will be assigned it,
				and the value copied over afterward.
		 
		 \param[in] src 		Source to copy from
		 \param[in] dest 		Destination to copy to
		 \param[in] component 	The component to copy
		*************************************************************************************/
		void CopyComponent(EntityID src, EntityID dest, const ComponentID& component);

		/*!***********************************************************************************
		 \brief Copies a component's value from src into the entity specified by id
		 
		 \tparam T 			The type of the component to copy
		 \param[in] id 		The destination entity
		 \param[in] src 	The source component to copy from
		*************************************************************************************/
		template <typename T>
		void Copy(EntityID id, const T& src);

		/*!***********************************************************************************
		 \brief Checks if the entity has component type T
		 
		 \tparam T 		The component to check
		 \param[in] id 	The entity to check
		 \return true 	Entity has component T
		 \return false 	Entity does not have component T
		*************************************************************************************/
		template<typename T>
		bool Has(EntityID id) const;

		/*!***********************************************************************************
		 \brief Checks if entity has component using ComponentID
		 
		 \param[in] id 			The entity to check
		 \param[in] component 	The component to check
		 \return true 			Entity has component
		 \return false 			Entity does not have component
		*************************************************************************************/
		bool Has(EntityID id, const ComponentID& component) const;

		/*!***********************************************************************************
		 \brief Removes a component from an entity
		 
		 \tparam T 		Component to remove
		 \param[in] id 	Entity to remove
		*************************************************************************************/
		template<typename T>
		void Remove(EntityID id);

		/*!***********************************************************************************
		 \brief Removes an entity
		 
		 \param[in] id Entity to remove
		*************************************************************************************/
		void RemoveEntity(EntityID id);

		/*!***********************************************************************************
		 \brief Checks if an entity is valid(exists within the current pool of entities)
		 
		 \param[in] id 	The id of the entity to find
		 \return true 	The entity is valid/exists
		 \return false 	The entity is not valid/does not exist
		*************************************************************************************/
		inline bool IsEntityValid(EntityID id) const
		{
			return m_entities.count(id);
		}

		/*!***********************************************************************************
		 \brief Gets the number of entities
		 
		 \return size_t The number of entities
		*************************************************************************************/
		inline size_t Size() const
		{
			return m_entities.size();
		}

		/*!***********************************************************************************
		 \brief Adds a component to the component pool
		 
		 \tparam T The component to add
		*************************************************************************************/
		template<typename T>
		void AddToPool()
		{
			m_componentPools.emplace(GetComponentID<T>(), new PoolData<T>());
		}

		/*!***********************************************************************************
		 \brief Get the entity's component list
		 
		 \param[in] id 						Entity to get all components of
		 \return std::vector<ComponentID> 	vector of all the components this entity has
		*************************************************************************************/
		std::vector<ComponentID> GetComponentIDs(EntityID id)
		{
			std::vector<ComponentID> ret;
			for (std::pair<const ComponentID, ComponentPool*>& component : m_componentPools)
			{
				if (component.second->HasEntity(id))
				{
					ret.emplace_back(component.first);
				}
			}
			return ret;
		}
	// ----- Private Functions ----- //
	private:

	// ----- Private Variables ----- //
	private:
		// set of entities picked over vector to increase the speed of searches for specific entites
		std::set<EntityID> m_entities;
		// map of to store pointers to individual componnet pools
		std::map<ComponentID, ComponentPool*> m_componentPools;
		// a queue of entity IDs to handle removed entities
		std::queue<EntityID> m_removed;
		
		// fns ptr to functions for handling the destruction of component pool
	};

	// extern to allow the access to the entity manager instance
	extern EntityManager* g_entityManager;

	//-------------------- Templated function implementations --------------------//

	template<typename T>
	T* EntityManager::Assign(EntityID id)
	{
		static ComponentID componentID = GetComponentID<T>();

		// if component is not found
		if (m_componentPools.find(componentID) == m_componentPools.end())
		{
			// add to map
			throw;
		}

		// add to component pool's map keeping track of index
		if (m_componentPools[componentID]->m_removed.empty())
		{
			m_componentPools[componentID]->m_idxMap.emplace(id, m_componentPools[componentID]->m_idxMap.size());
		}
		else
		{
			// reuse old slot if exists
			m_componentPools[componentID]->m_idxMap.emplace(id, m_componentPools[componentID]->m_removed.front());
			m_componentPools[componentID]->m_removed.pop();
		}
		// initialize that region of memory
		if (m_componentPools[componentID]->m_size >= m_componentPools[componentID]->m_capacity - 1)
		{
			m_componentPools[componentID]->resize(m_componentPools[componentID]->m_capacity * 2);
		}
		// if you new at an existing region of allocated memory, and you specify where, like in this case
		// it will call the constructor at this position instead of allocating more memory
		m_componentPools[componentID]->Get(id) =  T();
		++(m_componentPools[componentID]->m_size);

		//m_entities[id].mask.set(componentID);
		return p_component;
	}


	template<typename T>
	T* EntityManager::Assign(EntityID id, T const& val)
	{
		static ComponentID componentID = GetComponentID<T>();

		// if component is not found
		if (m_componentPools.find(componentID) == m_componentPools.end())
		{
			// add to map
			m_componentPools.emplace(std::make_pair(componentID, new ComponentPool(sizeof(T))));
		}

		// add to component pool's map keeping track of index
		if (m_componentPools[componentID]->m_removed.empty())
		{
			m_componentPools[componentID]->m_idxMap.emplace(id, m_componentPools[componentID]->m_idxMap.size());
		}
		else
		{
			// reuse old slot if exists
			m_componentPools[componentID]->m_idxMap.emplace(id, m_componentPools[componentID]->m_removed.front());
			m_componentPools[componentID]->m_removed.pop();
		}
		// initialize that region of memory
		if (m_componentPools[componentID]->m_size >= m_componentPools[componentID]->capacity - 1)
		{
			m_componentPools[componentID]->resize(m_componentPools[componentID]->capacity * 2);
		}


		// if you new at an existing region of allocated memory, and you specify where, like in this case
		// it will call the constructor at this position instead of allocating more memory
		m_componentPools[componentID]->Get(id) = T(val);
		++(m_componentPools[componentID]->size);

		//m_entities[id].mask.set(componentID);
		return p_component;
	}

	//-------------------- Templated function implementations --------------------//

	template<typename T>
	ComponentID EntityManager::GetComponentID() const
	{
		ComponentID tmp = typeid(T).name();
		size_t cPos = tmp.find_last_of(":");
		cPos = (cPos == std::string::npos) ? 0 : cPos + 1;
		tmp = tmp.substr(cPos);
		return (tmp[0] == 's') ? tmp.substr(7) :
			   (tmp[0] == 'c') ? tmp.substr(6) : tmp;
	}

	template<typename T>
	ComponentPool* EntityManager::GetComponentPoolPointer()
	{
		return m_componentPools[GetComponentID<T>()];
	}


	template<typename T>
	ComponentPool& EntityManager::GetComponentPool()
	{
		return *GetComponentPoolPointer<T>();
	}

	template<typename T>
	const ComponentPool& EntityManager::GetComponentPool() const
	{
		return *GetComponentPoolPointer<T>();
	}

	template<typename T>
	T* EntityManager::GetPointer(EntityID id)
	{
		return static_cast<T*>(m_componentPools[GetComponentID<T>()]->Get(id));
	}

	template<typename T>
	const T* EntityManager::GetPointer(EntityID id) const
	{
		return static_cast<T*>(m_componentPools.at(GetComponentID<T>())->Get(id));
	}

	template<typename T>
	T& EntityManager::Get(EntityID id)
	{
		T* p_comp = GetPointer<T>(id);
		if (!p_comp)
			throw;	// to add error
		return *p_comp;
	}

	template<typename T>
	const T& EntityManager::Get(EntityID id) const
	{
		T* p_comp = GetPointer<T>(id);
		if (!p_comp)
			throw;	// to add error
		return *p_comp;
	}

	template <typename T>
	void EntityManager::Copy(EntityID id, const T& src)
	{
		if (m_componentPools[GetComponentID<T>()]->HasEntity(id))
		{
			Get<T>(id) = T(src);
		}
	}

	template<typename T>
	bool EntityManager::Has(EntityID id) const
	{
		// if the return is not a nullptr, it has the component
		return (GetPointer<T>(id) != nullptr);
	}
	template<typename T>
	void EntityManager::Remove(EntityID id)
	{
		if (!Has<T>(id))
			return; // log in the future
		const ComponentID componentID = GetComponentID<T>();
		size_t lastEntID = (*std::prev(m_componentPools[componentID]->m_idxMap.end())).first;
		size_t poolID = m_componentPools[componentID]->m_idxMap[id];
		if (m_componentPools[componentID]->m_idxMap.size() > 1)
		{
			std::swap(Get<T>(id), Get<T>(lastEntID));
		}
		// remove the current component, and the last component from the map
		m_componentPools[componentID]->remove(id);
		m_componentPools[componentID]->remove(lastEntID);
		// re-empalce the "last" entity inplace to the existing id's position
		m_componentPools[componentID]->m_idxMap.emplace(lastEntID, poolID);
		--(m_componentPools[componentID]->m_size);
	}
}
