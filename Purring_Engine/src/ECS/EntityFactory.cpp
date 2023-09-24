/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EntityFactor.cpp
 \date     11-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 	This file contains function defenitions for Entity factory.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "EntityFactory.h"
#include "Logging/Logger.h"
extern Logger engine_logger;


namespace PE
{
	// The pointer to the current instance
	EntityFactory* g_entityFactory{ nullptr };

	
	EntityFactory::EntityFactory() : p_entityManager(PE::g_entityManager)
	{ 
		if (g_entityFactory != nullptr)
		{
			engine_logger.AddLog(true, "Another instance of Entity Factory was created!!", __FUNCTION__);
			engine_logger.FlushLog();
			throw;
		}
		g_entityFactory = this;
		LoadComponents();
	};


	EntityFactory::~EntityFactory()
	{
		
	}


	EntityID EntityFactory::Clone(EntityID id)
	{
		if (p_entityManager->IsEntityValid(id))
		{
			EntityID clone = CreateEntity();
			for (std::pair<const ComponentID, size_t>& componentCreator : m_componentMap)
			{	
				LoadComponent(clone, componentCreator.first.c_str(),
				static_cast<void*>(p_entityManager->GetComponentPoolPointer(componentCreator.first)->Get(id)));
			}
			return clone;
		}
		else
		{
			throw;
		}
	}

	

	// Hans
	void EntityFactory::AssignComponent(EntityID id, const std::string& name, int componentData)
	{
		id; name; componentData;
		// Here you will assign and initialize the component to the entity
		// For now, I'll leave this as a placeholder.
		// This might call something like:
		// entityManager->AssignComponent(id, name, componentData);
	}
	
	void EntityFactory::LoadComponents()
	{
		g_initializeComponent.emplace("RigidBody", &EntityFactory::InitializeRigidBody);
		g_initializeComponent.emplace("Collider", &EntityFactory::InitializeCollider);
		g_initializeComponent.emplace("Transform", &EntityFactory::InitializeTransform);
		g_initializeComponent.emplace("PlayerStats", &EntityFactory::InitializePlayerStats);
		g_initializeComponent.emplace("Renderer", &EntityFactory::InitializeRenderer);
	}

	bool EntityFactory::InitializeRigidBody(const EntityID& id, void* data)
	{
		g_entityManager->Get<RigidBody>(id) =
			(data == nullptr) ?
			RigidBody()
			:
			*reinterpret_cast<RigidBody*>(data);
		return true;
	}

	bool EntityFactory::InitializeCollider(const EntityID& id, void* data)
	{
		g_entityManager->Get<Collider>(id) =
			(data == nullptr) ?
			Collider()
			:
			*reinterpret_cast<Collider*>(data);
		return true;
	}

	bool EntityFactory::InitializeTransform(const EntityID& id, void* data)
	{
		g_entityManager->Get<Transform>(id) =
			(data == nullptr) ?
			Transform()
			:
			*reinterpret_cast<Transform*>(data);
		return true;
	}

	bool EntityFactory::InitializePlayerStats(const EntityID& id, void* data)
	{
		g_entityManager->Get<PlayerStats>(id) = 
		(data == nullptr) ?
			PlayerStats()
			:
			*reinterpret_cast<PlayerStats*>(data);
		return true;
	}

	bool EntityFactory::InitializeRenderer(const EntityID& id, void* data)
	{
		g_entityManager->Get<Graphics::Renderer>(id) =
			(data == nullptr) ?
			Graphics::Renderer()
			:
			*reinterpret_cast<Graphics::Renderer*>(data);
		return true;
	}


	EntityID EntityFactory::CreateFromPrefab(const char* prefab)
	{
		EntityID id = CreateEntity();

		// if the prefab exists in the current list
		if (m_prefabs.m_map.count(prefab))
		{
			Assign(id, m_prefabs.m_map.at(prefab));
			for (const ComponentID& componentID : m_prefabs.m_map[prefab])
			{
				LoadComponent(id, componentID.c_str(), nullptr);
			}
		}
		std::string str = "Created Entity-";
		str += std::to_string(id);
		engine_logger.AddLog(false, str, __FUNCTION__);
		return id;
	}

	bool EntityFactory::LoadComponent(EntityID id, const char* component, void* data)
	{
		if (!g_entityManager->IsEntityValid(id))
			return false;
		// if the prefab exists in the current list
		Assign(id, { component });
		return std::invoke(g_initializeComponent[component], this, id, data);
	}
}