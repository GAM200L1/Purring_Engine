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

// INCLUDES
#include "Components.h"
#include "Data/json.hpp"
#include "Data/SerializationManager.h"
#include "Singleton.h"
#include <bitset>
#include <set>
#include <queue>
#include <optional>
#include <climits>

// Const expressions
constexpr unsigned MAX_COMPONENTS = 32;
static unsigned s_componentCounter{};

// Typedefs
typedef unsigned long long EntityID;								// typedef for storing the unique ID of the entity, same as size_t
typedef std::bitset<MAX_COMPONENTS> ComponentID;					// ComponentID type, internally it is a bitset


const auto ALL = std::move(std::bitset<MAX_COMPONENTS>{}.set());

namespace PE
{
	struct Comparer {
		bool operator() (const std::bitset<MAX_COMPONENTS>& b1, const std::bitset<MAX_COMPONENTS>& b2) const {
			return b1.to_ulong() < b2.to_ulong();
		}
	};


	/*!***********************************************************************************
	 \brief Entity manager struct
	 
	*************************************************************************************/
	class EntityManager : public Singleton<EntityManager>
	{
	public:
		friend class Singleton<EntityManager>;
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


	// ----- Public Getters ----- //
	public:

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

		 \param[in] r_component 			The pool to Get
		 \return const ComponentPool* 	The pointer to the pool
		*************************************************************************************/
		const ComponentPool* GetComponentPoolPointer(const ComponentID& r_component) const;

		/*!***********************************************************************************
		 \brief Get a pointer to a specific component pool

		 \param[in] r_component 			The pool to Get
		 \return const ComponentPool* 	The pointer to the pool
		*************************************************************************************/
		ComponentPool* GetComponentPoolPointer(const ComponentID& r_component);


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

	// ----- Public Methods ----- //
	public:
		/*!***********************************************************************************
		 \brief Creates a new entity and returns its' ID
		 
		 \return EntityID ID of the generated entity
		*************************************************************************************/
		EntityID NewEntity();

		/*!***********************************************************************************
		 \brief Creates a new with request for specific ID entity and returns its' ID

		 \return EntityID ID of the generated entity
		*************************************************************************************/
		EntityID NewEntity(EntityID id);
		
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
		T* Assign(EntityID id, T const& r_val);

		/*!***********************************************************************************
		 \brief Assign an entity with a component specified by the componentID & the 
		 		createor input parameters
		 
		 \param[in] id 				ID of the entity to assign components to
		 \param[in] componentID 	The ID of the component to assign 
		*************************************************************************************/
		void Assign(const EntityID& r_id, const char* p_componentID);


		/*!***********************************************************************************
		 \brief Assign an entity with a component specified by the componentID 
		 
		 \param[in] id 				ID of the entity to asign components to
		 \param[in] componentID 	The component to assign
		*************************************************************************************/
		void Assign(const EntityID& r_id, const ComponentID& r_componentID);

		/*!***********************************************************************************
		 \brief Copies a component from src entity into dest entity
		 		If the destination does not have the the component, it will be assigned it,
				and the value copied over afterward.
		 
		 \param[in] src 		Source to copy from
		 \param[in] dest 		Destination to copy to
		 \param[in] component 	The component to copy
		*************************************************************************************/
		void CopyComponent(EntityID src, EntityID dest, const ComponentID& r_component);

		/*!***********************************************************************************
		 \brief Copies a component's value from src into the entity specified by id
		 
		 \tparam T 			The type of the component to copy
		 \param[in] id 		The destination entity
		 \param[in] src 	The source component to copy from
		*************************************************************************************/
		template <typename T>
		void Copy(EntityID id, const T& r_src);

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
		bool Has(EntityID id, const ComponentID& r_component) const;

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
		 \brief Gets one past the larges entity id (for looping)

		 \return size_t The number of entities
		*************************************************************************************/
		inline size_t OnePast() const
		{
			return m_entityCounter + 1;
		}
		

		/*!***********************************************************************************
		 \brief Adds a component to the component pool
		 
		 \tparam T The component to add
		*************************************************************************************/
		template<typename T>
		void AddToPool()
		{
			m_componentPools.emplace(GetComponentID<T>(), new PoolData<T>());
			m_poolsEntity[GetComponentID<T>()];
		}

		/*!***********************************************************************************
		 \brief Get the entity's component list
		 
		 \param[in] id 						Entity to get all components of
		 \return std::vector<ComponentID> 	vector of all the components this entity has
		*************************************************************************************/
		std::vector<ComponentID> GetComponentIDs(EntityID id)
		{
			std::vector<ComponentID> ret;
			for (std::pair<const ComponentID, ComponentPool*>& r_component : m_componentPools)
			{
				if (r_component.second->HasEntity(id))
				{
					ret.emplace_back(r_component.first);
				}
			}
			return ret;
		}

		/*!***********************************************************************************
		 \brief Get the Entities In Pool object
		 
		 \param[in] r_pool 						The pool to get the eneity vector from
		 \return const std::vector<EntityID>& 	Gets the pool's vector of entities
		*************************************************************************************/
		std::vector<EntityID>& GetEntitiesInPool(const ComponentID& r_pool);

		/*!***********************************************************************************
		 \brief Updates the entity vectors, helps keeps track of which entity can be found in
		 		in which pool

		 \param[in] id 	ID of the entity to handle
		 \param[in] add Add or remove flag, true = add, false = remove from pool
		*************************************************************************************/
		void UpdateVectors(EntityID id, bool add = true)
		{
			if (add)
			{
				if (std::find(m_poolsEntity[ALL].begin(), m_poolsEntity[ALL].end(), id) == m_poolsEntity[ALL].end())
				{
					m_poolsEntity[ALL].emplace_back(id);
				}
				for (const auto& vec : m_poolsEntity)
				{
					const size_t cnt = vec.first.count();
					size_t cnt2{};
					for (const std::pair<const ComponentID, ComponentPool*>pool : m_componentPools)
					{
						if ((vec.first & pool.first).any() && pool.second->HasEntity(id)) 
						if (std::find(m_poolsEntity[vec.first].begin(), m_poolsEntity[vec.first].end(), id) == m_poolsEntity[vec.first].end())
						{
							++cnt2;
						}
					}
					if (cnt2 == cnt)
						m_poolsEntity[vec.first].emplace_back(id);
				}
			}
			else
			{
				if (!m_entities.count(id) &&
					(std::find(m_poolsEntity[ALL].begin(), m_poolsEntity[ALL].end(), id) != m_poolsEntity[ALL].end()))
				{
					m_poolsEntity[ALL].erase(std::remove(m_poolsEntity[ALL].begin(), m_poolsEntity[ALL].end(), id), m_poolsEntity[ALL].end());
				}
				for (const auto& pool : m_poolsEntity)
				{
					if ((std::find(m_poolsEntity[pool.first].begin(), m_poolsEntity[pool.first].end(), id) != m_poolsEntity[pool.first].end()))
					{
						m_poolsEntity[pool.first].erase(std::remove(m_poolsEntity[pool.first].begin(), m_poolsEntity[pool.first].end(), id), m_poolsEntity[pool.first].end());
					}
				}
			}
		}
			// ----- Private Variables ----- //
	private:
		// set of entities picked over vector to increase the speed of searches for specific entites
		std::set<EntityID> m_entities;
		// map of to store pointers to individual componnet pools
		std::map<ComponentID, ComponentPool*, Comparer> m_componentPools;
		// a queue of entity IDs to handle removed entities
		std::set<EntityID> m_removed;
		// a map to a vector of entity IDs used to keep track of entity components (used to iterate in SceneView)
		std::map<ComponentID, std::vector<EntityID>, Comparer> m_poolsEntity;
		// a counter to help keep track of the entities "absolute" count
		size_t m_entityCounter{0};
	};

	//-------------------- Templated function implementations --------------------//

	template<typename T>
	T* EntityManager::Assign(EntityID id)
	{
		static ComponentID componentID = GetComponentID<T>();

		// if component is not found
		if (m_componentPools.find(componentID) == m_componentPools.end())
		{
			throw;
		}
		if (m_componentPools[componentID]->HasEntity(id))
		{
			return;
		}

		// add to component pool's map keeping track of index
		m_componentPools[componentID]->idxMap.emplace(id, m_componentPools[componentID]->idxMap.size());
		
		// initialize that region of memory
		if (m_componentPools[componentID]->size >= m_componentPools[componentID]->capacity - 1)
		{
			m_componentPools[componentID]->Resize(m_componentPools[componentID]->m_capacity * 2);
		}
		// if you new at an existing region of allocated memory, and you specify where, like in this case
		// it will call the constructor at this position instead of allocating more memory
		m_componentPools[componentID]->Get(id) =  T();
		++(m_componentPools[componentID]->size);
		return p_component;
	}


	//template<typename T>
	//T* EntityManager::Assign(EntityID id, T const& r_val)
	//{
	//	static ComponentID componentID = GetComponentID<T>();

	//	// if component is not found
	//	if (m_componentPools.find(componentID) == m_componentPools.end())
	//	{
	//		throw;
	//	}
	//	if (m_componentPools[componentID]->HasEntity(id))
	//	{
	//		return nullptr;
	//	}
	//	// add to component pool's map keeping track of index
	//	m_componentPools[componentID]->idxMap.emplace(id, m_componentPools[componentID]->idxMap.size());

	//	// initialize that region of memory
	//	if (m_componentPools[componentID]->size >= m_componentPools[componentID]->capacity - 1)
	//	{
	//		m_componentPools[componentID]->Resize(m_componentPools[componentID]->capacity * 2);
	//	}


	//	// if you new at an existing region of allocated memory, and you specify where, like in this case
	//	// it will call the constructor at this position instead of allocating more memory
	//	m_componentPools[componentID]->Get(id) = T(r_val);
	//	++(m_componentPools[componentID]->size);
	//	return p_component;
	//}

	//-------------------- Templated function implementations --------------------//

	template<typename T>
	ComponentID EntityManager::GetComponentID() const
	{
		//auto tmp = typeid(T).name();
		//size_t cPos = tmp.find_last_of(":");
		//cPos = (cPos == std::string::npos) ? 0 : cPos + 1;
		//tmp = tmp.substr(cPos);
		//return (tmp[0] == 's') ? tmp.substr(7) :
		//	   (tmp[0] == 'c') ? tmp.substr(6) : tmp;

		static unsigned s_componentID = s_componentCounter++;
		return ComponentID().set(s_componentID);
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
		return reinterpret_cast<T*>(m_componentPools[GetComponentID<T>()]->Get(id));
	}

	template<typename T>
	const T* EntityManager::GetPointer(EntityID id) const
	{
		return reinterpret_cast<T*>(m_componentPools.at(GetComponentID<T>())->Get(id));
	}

	template<typename T>
	T& EntityManager::Get(EntityID id)
	{
		T* p_comp = GetPointer<T>(id);
		if (!p_comp)
		{
			throw;	// to add error
		}
		return *p_comp;
	}

	template<typename T>
	const T& EntityManager::Get(EntityID id) const
	{
		T* p_comp = GetPointer<T>(id);
		if (!p_comp)
		{
			throw;	// to add error
		}
		return *p_comp;
	}

	template <typename T>
	void EntityManager::Copy(EntityID id, const T& r_src)
	{
		if (m_componentPools[GetComponentID<T>()]->HasEntity(id))
		{
			Get<T>(id) = T(r_src);
		}
	}

	template<typename T>
	bool EntityManager::Has(EntityID id) const
	{
		return m_componentPools.at(GetComponentID<T>())->HasEntity(id);
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
		UpdateVectors(id, false);
	}


	struct EntityDescriptor
	{
		std::string name;
		std::optional<EntityID> parent;
		std::set<EntityID> children;

		EntityID sceneID{ ULLONG_MAX }; // technically also kinda stores the order of the entity in the scene
		

		nlohmann::json ToJson() const;
		static EntityDescriptor Deserialize(const nlohmann::json& j);
	};

}
