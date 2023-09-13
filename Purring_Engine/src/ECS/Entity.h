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

// Typedefs
typedef unsigned long long EntityID;				// typedef for storing the unique ID of the entity, same as size_t
typedef std::string ComponentID;					// typedf for component ID 32 bits, max components is 32

// Global & static vars
static unsigned g_componentCounter = 0;
namespace Engine
{
	class EntityManager
	{
	private:
		std::set<EntityID> m_entities;
		std::map<ComponentID, ComponentPool*> m_componentPools;
		std::queue<EntityID> m_removed;
	public:
		EntityManager();
		~EntityManager();

		
		EntityID NewEntity();
		
		/*!************************************************************************
		 \brief 	Assigns components to an entity

		 \tparam T 			Component to assign
		 \param[in] id 		Entity to assign component to
		 \return T* 		A pointer to the component generated
		**************************************************************************/
		template<typename T>
		T* Assign(EntityID id);
		

		template<typename T>
		T* Assign(EntityID id, T const& val);


		void Assign(const EntityID& id, const char* componentID, const ComponentCreator* creator);

		void Assign(const EntityID& id, const ComponentID& componentID);

		template<typename T>
		ComponentID GetComponentID() const;

		template<typename T>
		ComponentPool* GetComponentPoolPtr();

		const ComponentPool* GetComponentPoolPtr(const ComponentID& component) const;

		template<typename T>
		ComponentPool& GetComponentPool();

		template<typename T>
		const ComponentPool& GetComponentPool() const;

		template<typename T>
		T* GetPtr(EntityID id);

		template<typename T>
		T& Get(EntityID id);

		template<typename T>
		const T& Get(EntityID id) const;

		void CopyComponent(EntityID src, EntityID dest, const ComponentID& component);

		template <typename T>
		void Copy(EntityID id, const T& src);

		template<typename T>
		bool Has(EntityID id) const;

		bool Has(EntityID id, const ComponentID& component) const;

		template<typename T>
		void Remove(EntityID id);

		void RemoveEntity(EntityID id);

		inline bool IsEntityValid(EntityID id) const
		{
			return m_entities.count(id);
		}

		inline size_t Size() const
		{
			return m_entities.size();
		}
	};
	extern EntityManager* g_entityManager;


	template<typename T>
	T* EntityManager::Assign(EntityID id)
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
		if (m_componentPools[componentID]->m_size >= m_componentPools[componentID]->m_capacity - 1)
		{
			m_componentPools[componentID]->resize(m_componentPools[componentID]->m_capacity * 2);
		}
		// if you new at an existing region of allocated memory, and you specify where, like in this case
		// it will call the constructor at this position instead of allocating more memory
		T* p_component = new (m_componentPools[componentID]->get(id)) T();
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
		T* p_component = new (m_componentPools[componentID]->get(id)) T(val);
		++(m_componentPools[componentID]->size);

		//m_entities[id].mask.set(componentID);
		return p_component;
	}


	template<typename T>
	ComponentID EntityManager::GetComponentID() const
	{
		return (typeid(T).name()[0] == 's') ? typeid(T).name() + 7 :
			(typeid(T).name()[0] == 'c') ? typeid(T).name() + 6 : typeid(T).name();
	}

	template<typename T>
	ComponentPool* EntityManager::GetComponentPoolPtr()
	{
		return m_componentPools[GetComponentID<T>()];
	}

	template<typename T>
	ComponentPool& EntityManager::GetComponentPool()
	{
		return *GetComponentPoolPtr<T>();
	}

	template<typename T>
	const ComponentPool& EntityManager::GetComponentPool() const
	{
		return *GetComponentPoolPtr<T>();
	}

	template<typename T>
	T* EntityManager::GetPtr(EntityID id)
	{
		return static_cast<T*>(m_componentPools[GetComponentID<T>()]->get(id));
	}

	template<typename T>
	T& EntityManager::Get(EntityID id)
	{
		T* p_comp = GetPtr<T>(id);
		if (!p_comp)
			throw;	// to add error
		return *p_comp;
	}

	template<typename T>
	const T& EntityManager::Get(EntityID id) const
	{
		T* p_comp = GetPtr<T>(id);
		if (!p_comp)
			throw;	// to add error
		return *p_comp;
	}

	template <typename T>
	void EntityManager::Copy(EntityID id, const T& src)
	{
		if (Has<T>(id))
			Get<T>(id) = src;
	}

	template<typename T>
	bool EntityManager::Has(EntityID id) const
	{
		// if the return is not a nullptr, it has the component
		return (GetPtr<T>(id) != nullptr);
	}
	template<typename T>
	void EntityManager::Remove(EntityID id)
	{
		if (!Has<T>(id))
			return; // log in the future
		const unsigned componentID = GetComponentID<T>();
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
		--(m_componentPools[componentID]->size);
	}
}

/*

template<typename... ComponentTypes>
struct SceneView
{
	SceneView() : p_entityManager(Engine::g_entityManager)
	{
		if constexpr(sizeof...(ComponentTypes))
		{
			std::initializer_list<ComponentID> componentIDs = { p_entityManager->GetComponentID<ComponentTypes>() ... };
			for (const ComponentID& c : componentIDs)
			{
				m_components.emplace_back(c);
			}
		}
		else
		{
			m_all = true;
		}
	}

	struct Iterator
	{
		Iterator(EntityID index, const std::vector<ComponentID>& components, bool all) : 
			p_entityManager(Engine::g_entityManager), m_index(index), m_components(components), m_all(all)
		{
		}

		EntityID operator* () const
		{
			return *(p_entityManager->m_entities.find(m_index));
		}

		bool operator== (const Iterator& rhs) const
		{
			return (m_index == rhs.m_index) || (m_index == p_entityManager->m_entities.size());
		}

		bool operator!= (const Iterator& rhs) const
		{
			return (m_index != rhs.m_index) || (m_index != p_entityManager->m_entities.size());
		}

		bool HasComponents() const
		{
			for (const ComponentID& components : m_components)
			{
				if (!p_entityManager->m_componentPools[components]->HasEntity(m_index))
					return false;
			}
			return true;
		}

		bool ValidIndex() const
		{
			return (p_entityManager->m_entities.contains(m_index)) &&
				   (m_all || (HasComponents()));
		}

		Iterator& operator++()
		{
			do
			{
				++m_index;
			} while (m_index < p_entityManager->m_entities.size() && !ValidIndex());
			return *this;
		}

		Engine::EntityManager* p_entityManager;
		EntityID m_index;
		const std::vector<ComponentID>& m_components;
		bool m_all{ false };
	};

	bool HasComponents(size_t index) const
	{
		for (const ComponentID& components : m_components)
		{
			if (!p_entityManager->m_componentPools[components]->HasEntity(index))
				return false;
		}
		return true;
	}


	const Iterator begin() const
	{
		if (p_entityManager->m_entities.empty())
			throw; // update to error log
		size_t firstIndex{};
		while (firstIndex < p_entityManager->m_entities.size() &&
			  (!HasComponents(firstIndex)) ||
			  (!p_entityManager->m_entities.contains(firstIndex))
			)
		{
			++firstIndex;
		}
		return Iterator(firstIndex, m_components, m_all);
	}

	const Iterator end() const
	{
		return Iterator(p_entityManager->m_entities.size(), m_components, m_all);
	}

	Engine::EntityManager* p_entityManager{ nullptr };
	std::vector<ComponentID> m_components;
	bool m_all{ false };
};

*/