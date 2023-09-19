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



namespace PE
{
	// The pointer to the current instance
	EntityFactory* g_entityFactory{ nullptr };

	
	EntityFactory::EntityFactory() : p_entityManager(PE::g_entityManager)
	{ 
		if (g_entityFactory != nullptr)
			throw;
		g_entityFactory = this;
		LoadComponents();
		m_prefabs.LoadPrefabs();
	};


	EntityFactory::~EntityFactory()
	{
		for (std::pair<const ComponentID, ComponentCreator*>& cmt : m_componentMap)
		{
			delete cmt.second;
		}
	}


	EntityID EntityFactory::Clone(EntityID id)
	{
		if (p_entityManager->IsEntityValid(id))
		{
			EntityID clone = CreateEntity();
			for (std::pair<const ComponentID, ComponentCreator*>& componentCreator : m_componentMap)
			{
				if (p_entityManager->Has(id, componentCreator.first))
				{
					p_entityManager->CopyComponent(id, clone, componentCreator.first);
				}
			}
			return clone;
		}
		else
		{
			throw;
		}
	}

	void EntityFactory::AddComponentCreator(const ComponentID& name, ComponentCreator* creator)
	{
		m_componentMap[name] = creator;
	}

	// Hans
	void EntityFactory::AssignComponent(EntityID id, const std::string& name, int componentData)
	{
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
	}

	bool EntityFactory::InitializeRigidBody(const EntityID& id, void* data)
	{
		(data == nullptr) ?
			new (g_entityManager->GetPointer<RigidBody>(id)) RigidBody()
			:
			new (g_entityManager->GetPointer<RigidBody>(id)) RigidBody(*static_cast<RigidBody*>(data));
		return true;
	}

	bool EntityFactory::InitializeCollider(const EntityID& id, void* data)
	{
		(data == nullptr) ?
			new (g_entityManager->GetPointer<Collider>(id)) Collider()
			:
			new (g_entityManager->GetPointer<Collider>(id)) Collider(*static_cast<Collider*>(data));
		return true;
	}

	bool EntityFactory::InitializeTransform(const EntityID& id, void* data)
	{
		(data == nullptr) ?
			new (g_entityManager->GetPointer<Transform>(id)) Transform()
			:
			new (g_entityManager->GetPointer<Transform>(id)) Transform(*static_cast<Transform*>(data));
		return true;
	}

	bool EntityFactory::InitializePlayerStats(const EntityID& id, void* data)
	{
		(data == nullptr) ?
			new (g_entityManager->GetPointer<PlayerStats>(id)) PlayerStats()
			:
			new (g_entityManager->GetPointer<PlayerStats>(id)) PlayerStats(*static_cast<PlayerStats*>(data));
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
				std::invoke(g_initializeComponent[componentID], this, id, nullptr);
			}
		}
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