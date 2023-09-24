/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Entity.cpp
 \date     03-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief     This contains the defenitions of the Entity class.

 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "Entity.h"
#include "SceneView.h"
#include "Physics/Colliders.h"
#include "Logging/Logger.h"

extern Logger engine_logger;


namespace PE
{
	// singleton
	EntityManager* g_entityManager{ nullptr };

	EntityManager::EntityManager()
	{
		if (g_entityManager != nullptr)
		{
			engine_logger.AddLog(true, "Another instance of Entity Manager was created!!", __FUNCTION__);
			engine_logger.FlushLog();
			throw;
		}
		g_entityManager = this;

	}

	EntityManager::~EntityManager()
	{
		m_entities.clear();
		for (std::pair<const ComponentID, ComponentPool*>& compPool : m_componentPools)
		{
			delete compPool.second;
		}
		m_componentPools.clear();
	}

	EntityID EntityManager::NewEntity()
	{
		size_t id = (m_removed.empty()) ? m_entities.size() : *(m_removed.begin());
		if (!m_removed.empty())
			m_removed.erase(id);
		m_entities.emplace(id);
		return id;
	}


	void EntityManager::Assign(const EntityID& id, const char* componentID)
	{
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
		// it will call the constructor at this position instead  of allocating more memory
		++(m_componentPools[componentID]->m_size);
	}

	void EntityManager::Assign(const EntityID& id, const ComponentID& componentID)
	{
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
		// it will call the constructor at this position instead  of allocating more memory
		++(m_componentPools[componentID]->m_size);
	}

	const ComponentPool* EntityManager::GetComponentPoolPointer(const ComponentID& component) const
	{
		return m_componentPools.at(component);
	}

	ComponentPool* EntityManager::GetComponentPoolPointer(const ComponentID& component) 
	{
		return m_componentPools.at(component);
	}

	void EntityManager::CopyComponent(EntityID src, EntityID dest, const ComponentID& component)
	{
		if (!Has(src, component))
			return;
		if (!Has(dest, component))
			Assign(dest, component);

		memcpy_s(m_componentPools[component]->Get(dest), m_componentPools[component]->m_elementSize, m_componentPools[component]->Get(src), m_componentPools[component]->m_elementSize);
	}

	

	bool EntityManager::Has(EntityID id, const ComponentID& component) const
	{
		return m_componentPools.at(component)->HasEntity(id);
	}


	void EntityManager::RemoveEntity(EntityID id)
	{
		if (m_entities.count(id))
		{
			for (std::pair<const ComponentID, ComponentPool*>& pool : m_componentPools)
			{
				if (pool.second->HasEntity(id))
				{
					pool.second->remove(id);
				}
			}
			m_entities.erase(id);
			m_removed.emplace(id);
			std::string str = "Removed Entity-";
			str += std::to_string(id);
			engine_logger.AddLog(false, str, __FUNCTION__);
		}
	}
}
