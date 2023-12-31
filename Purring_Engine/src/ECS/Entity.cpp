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
#include "EntityFactory.h"
#include "Hierarchy/HierarchyManager.h"

extern Logger engine_logger;

namespace PE
{
	// singleton

	EntityManager::EntityManager()
	{
		m_poolsEntity[ALL];
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
		size_t id = (m_removed.empty()) ? m_entities.size() : *(m_removed.begin());
		if (!m_removed.empty())
			m_removed.erase(id);
		m_entities.emplace(id);
		++m_entityCounter;
		// Assign Descriptor component
		Assign(id, GetComponentID<EntityDescriptor>());
		Get<EntityDescriptor>(id).name = "GameObject";
		Get<EntityDescriptor>(id).name += std::to_string(id);
		Get<EntityDescriptor>(id).sceneID = id; // potentially in the future it will not be tied!!
		Get<EntityDescriptor>(id).oldID = id;
		return id;
	}

	EntityID EntityManager::NewEntity(EntityID id)
	{
		
		if (m_removed.count(id))
			m_removed.erase(id);
		else if (id == ULLONG_MAX || m_entities.count(id)) // if a prefab or the id alread is used
		{
			engine_logger.AddLog(false, "Allocating new ID for New Entity!", __FUNCTION__);
			id = (m_removed.empty()) ? m_entities.size() : *(m_removed.begin()); // re-assgin the id
			// if the removed set was not empty, that means an id was used, remove it.
			
			if (!m_removed.empty())
				m_removed.erase(id);
			else
				++m_entityCounter;
		}

		m_entities.emplace(id);

		if (id >= m_entities.size())
		{
			for (size_t i{}; i < id; ++i)
			{
				if (!m_entities.count(i))
				{
					m_removed.emplace(i);
				}
			}
		}


		// Assign Descriptor component
		Assign(id, GetComponentID<EntityDescriptor>());
		Get<EntityDescriptor>(id).name = "GameObject";
		Get<EntityDescriptor>(id).name += std::to_string(id);
		Get<EntityDescriptor>(id).sceneID = (Get<EntityDescriptor>(id).sceneID == ULLONG_MAX)? id : Get<EntityDescriptor>(id).sceneID; // potentially in the future it will not be tied!!
		Get<EntityDescriptor>(id).oldID = id;
		return id;
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

		EntityFactory::GetInstance().LoadComponent(dest, r_component, m_componentPools[r_component]->Get(src));
	}	

	bool EntityManager::Has(EntityID id, const ComponentID& r_component) const
	{
		return m_componentPools.at(r_component)->HasEntity(id);
	}

	void EntityManager::RemoveEntity(EntityID id)
	{
		if (m_entities.count(id))
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id).children.size())
			{
				std::vector<EntityID> tmp;
				for (auto cid : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
				{
					tmp.emplace_back(cid);
				}

				for (const auto& cid : tmp)
				{
					Hierarchy::GetInstance().DetachChild(cid);
					if (EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.has_value())
						Hierarchy::GetInstance().AttachChild(EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.value(), cid);
				}
				
			}
			Hierarchy::GetInstance().DetachChild(id);
			for (const ComponentID& r_pool : GetComponentIDs(id))
			{
				m_componentPools[r_pool]->Remove(id);
			}
			m_entities.erase(id);
			m_removed.emplace(id);
			
			UpdateVectors(id, false);
		}
	}

	std::vector<EntityID>& EntityManager::GetEntitiesInPool(const ComponentID& r_pool)
	{
		try
		{
			return m_poolsEntity.at(r_pool);
		}
		catch (const std::out_of_range& c_error)
		{
			engine_logger.AddLog(false, c_error.what(), __FUNCTION__);
			
			// make the pool?
			m_poolsEntity[r_pool];
			for (const auto& id : m_entities)
			{
				UpdateVectors(id);
			}
			return m_poolsEntity.at(r_pool);
		}
	}

	nlohmann::json EntityDescriptor::ToJson(size_t id) const
	{
		UNREFERENCED_PARAMETER(id);

		nlohmann::json j;
		j["name"] = name;

		if (parent.has_value())
		{
			j["parent"] = parent.value();
		}
		else
		{
			j["parent"] = nullptr;
		}

		j["children"] = children;
		j["sceneID"] = sceneID;
		j["isActive"] = isActive;

		j["Prefab Type"] = prefabType;
		j["Layer"] = layer;
		j["Old ID"] = oldID;

		return j;
	}

	EntityDescriptor EntityDescriptor::Deserialize(const nlohmann::json& j)
	{
		EntityDescriptor desc;
		desc.name = j["name"];

		if (j["parent"] != nullptr)
		{
			desc.parent = j["parent"].get<EntityID>();
		}
		else
		{
			desc.parent = std::nullopt;
		}

		if (j.contains("children"))
			desc.children = j["children"].get<std::set<EntityID>>();

		if (j.contains("sceneID"))
			desc.sceneID = j["sceneID"].get<EntityID>();


		if (j.contains("isActive"))
		{
			desc.isActive = j["isActive"];
		}

		if (j.contains("Prefab Type"))
		{
			desc.prefabType = j["Prefab Type"].get<std::string>();
		}

		if (j.contains("Layer"))
		{
			desc.layer = j["Layer"].get<int>();
		}

		if (j.contains("Old ID"))
		{
			if (j["Old ID"].get<EntityID>() != ULLONG_MAX)
				desc.oldID = j["Old ID"].get<EntityID>();
			
		}

		return desc;
	}
}
