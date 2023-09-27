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
			for (const ComponentID& componentCreator : p_entityManager->GetComponentIDs(id))
			{
				LoadComponent(clone, componentCreator.c_str(),
					p_entityManager->GetComponentPoolPointer(componentCreator)->Get(id));
			}
			return clone;
		}
		else
		{
			throw;
		}
	}

	

	// Hans
	void EntityFactory::AssignComponent(EntityID id, const std::string& r_name, int componentData)
	{
		id; r_name; componentData;
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

	bool EntityFactory::InitializeRigidBody(const EntityID& r_id, void* data)
	{
		g_entityManager->Get<RigidBody>(r_id) =
			(data == nullptr) ?
			RigidBody()
			:
			*reinterpret_cast<RigidBody*>(data);
		return true;
	}

	bool EntityFactory::InitializeCollider(const EntityID& r_id, void* data)
	{
		g_entityManager->Get<Collider>(r_id) =
			(data == nullptr) ?
			Collider()
			:
			*reinterpret_cast<Collider*>(data);
		return true;
	}

	bool EntityFactory::InitializeTransform(const EntityID& r_id, void* data)
	{
		g_entityManager->Get<Transform>(r_id) =
			(data == nullptr) ?
			Transform()
			:
			*reinterpret_cast<Transform*>(data);
		return true;
	}

	bool EntityFactory::InitializePlayerStats(const EntityID& r_id, void* data)
	{
		g_entityManager->Get<PlayerStats>(r_id) = 
		(data == nullptr) ?
			PlayerStats()
			:
			*reinterpret_cast<PlayerStats*>(data);
		return true;
	}

	bool EntityFactory::InitializeRenderer(const EntityID& r_id, void* data)
	{
		g_entityManager->Get<Graphics::Renderer>(r_id) =
			(data == nullptr) ?
			Graphics::Renderer()
			:
			*reinterpret_cast<Graphics::Renderer*>(data);
		return true;
	}


	EntityID EntityFactory::CreateFromPrefab(const char* r_prefab)
	{
		EntityID id = CreateEntity();

		// if the prefab exists in the current list
		if (m_prefabs.prefabs.count(r_prefab))
		{
			Assign(id, m_prefabs.prefabs.at(r_prefab));
			for (const ComponentID& componentID : m_prefabs.prefabs[r_prefab])
			{
				LoadComponent(id, componentID.c_str(), nullptr);
			}
		}
		std::string str = "Created Entity-";
		str += std::to_string(id);
		engine_logger.AddLog(false, str, __FUNCTION__);
		return id;
	}

	bool EntityFactory::LoadComponent(EntityID id, const char* r_component, void* data)
	{
		if (!g_entityManager->IsEntityValid(id))
			return false;
		// if the prefab exists in the current list
		Assign(id, { r_component });
		return std::invoke(g_initializeComponent[r_component], this, id, data);
	}
}