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

	EntityManager::EntityManager()
	{
		m_poolsEntity["All"];
	}

	EntityManager::~EntityManager()
	{
		m_entities.clear();
		for (std::pair<const ComponentID, ComponentPool*>& r_compPool : m_componentPools)
		{
			delete r_compPool.second;
		}
		m_componentPools.clear();
	}

	EntityID EntityManager::NewEntity()
	{
		size_t id = (m_removed.empty()) ? m_entities.size() : (m_removed.front());
		if (!m_removed.empty())
			m_removed.pop();
		m_entities.emplace(id);
		++m_entityCounter;
		return id;
	}


	void EntityManager::Assign(const EntityID& r_id, const char* p_componentID)
	{
		// if component is not found
		if (m_componentPools.find(p_componentID) == m_componentPools.end())
		{
			engine_logger.AddLog(true, "Component was not registered!!", __FUNCTION__);
			engine_logger.FlushLog();
			throw;
		}
		if (m_componentPools[p_componentID]->HasEntity(r_id))
		{
			return;
		}
		// add to component pool's map keeping track of index
		m_componentPools[p_componentID]->idxMap.emplace(r_id, m_componentPools[p_componentID]->idxMap.size());
		// initialize that region of memory
		if (m_componentPools[p_componentID]->size >= m_componentPools[p_componentID]->capacity - 1)
		{
			m_componentPools[p_componentID]->Resize(m_componentPools[p_componentID]->capacity * 2);
		}

		// if you new at an existing region of allocated memory, and you specify where, like in this case
		// it will call the constructor at this position instead  of allocating more memory
		++(m_componentPools[p_componentID]->size);
	}

	void EntityManager::Assign(const EntityID& r_id, const ComponentID& r_componentID)
	{
		// if component is not found
		if (m_componentPools.find(r_componentID) == m_componentPools.end())
		{
			engine_logger.AddLog(true, "Component was not registered!!", __FUNCTION__);
			engine_logger.FlushLog();
			throw;
		}

		if (m_componentPools[r_componentID]->HasEntity(r_id))
		{
			return;
		}
		// add to component pool's map keeping track of index
		m_componentPools[r_componentID]->idxMap.emplace(r_id, m_componentPools[r_componentID]->idxMap.size());

		// initialize that region of memory
		if (m_componentPools[r_componentID]->size >= m_componentPools[r_componentID]->capacity - 1)
		{
			m_componentPools[r_componentID]->Resize(m_componentPools[r_componentID]->capacity * 2);
		}

		// if you new at an existing region of allocated memory, and you specify where, like in this case
		// it will call the constructor at this position instead  of allocating more memory
		++(m_componentPools[r_componentID]->size);
	}

	const ComponentPool* EntityManager::GetComponentPoolPointer(const ComponentID& r_component) const
	{
		return m_componentPools.at(r_component);
	}

	ComponentPool* EntityManager::GetComponentPoolPointer(const ComponentID& r_component) 
	{
		return m_componentPools.at(r_component);
	}

	void EntityManager::CopyComponent(EntityID src, EntityID dest, const ComponentID& r_component)
	{
		if (!Has(src, r_component))
			return;
		if (!Has(dest, r_component))
			Assign(dest, r_component);

		memcpy_s(m_componentPools[r_component]->Get(dest), m_componentPools[r_component]->elementSize, m_componentPools[r_component]->Get(src), m_componentPools[r_component]->elementSize);
	}

	

	bool EntityManager::Has(EntityID id, const ComponentID& r_component) const
	{
		return m_componentPools.at(r_component)->HasEntity(id);
	}


	void EntityManager::RemoveEntity(EntityID id)
	{
		if (m_entities.count(id))
		{
			for (const ComponentID& r_pool : GetComponentIDs(id))
			{
				m_componentPools[r_pool]->Remove(id);
			}
			m_entities.erase(id);
			m_removed.emplace(id);
			UpdateVectors(id, false);
		}
	}
}
