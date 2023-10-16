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

	EntityFactory::EntityFactory() : p_entityManager(&EntityManager::GetInstance())
	{ 
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
			for (const ComponentID& r_componentCreator : p_entityManager->GetComponentIDs(id))
			{
				LoadComponent(clone, r_componentCreator,
					p_entityManager->GetComponentPoolPointer(r_componentCreator)->Get(id));
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
		m_initializeComponent.emplace(p_entityManager->GetComponentID<RigidBody>(), &EntityFactory::InitializeRigidBody);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Collider>(), &EntityFactory::InitializeCollider);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Transform>(), &EntityFactory::InitializeTransform);
		//m_initializeComponent.emplace(p_entityManager->GetComponentID<PlayerStats>(), &EntityFactory::InitializePlayerStats);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Graphics::Renderer>(), &EntityFactory::InitializeRenderer);
		//m_initializeComponent.emplace(p_entityManager->GetComponentID<ScriptComponent>(), &EntityFactory::InitializeScriptComponent);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Graphics::Camera>(), &EntityFactory::InitializeCamera);
	}

	bool EntityFactory::InitializeRigidBody(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<RigidBody>(r_id) =
			(p_data == nullptr) ?
			RigidBody()
			:
			*reinterpret_cast<RigidBody*>(p_data);
		return true;
	}

	bool EntityFactory::InitializeCollider(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<Collider>(r_id) =
			(p_data == nullptr) ?
			Collider()
			:
			*reinterpret_cast<Collider*>(p_data);
		return true;
	}

	bool EntityFactory::InitializeTransform(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<Transform>(r_id) =
			(p_data == nullptr) ?
			Transform()
			:
			*reinterpret_cast<Transform*>(p_data);
		return true;
	}

	bool EntityFactory::InitializePlayerStats(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<StructPlayerStats>(r_id) =
		(p_data == nullptr) ?
			StructPlayerStats()
			:
			*reinterpret_cast<StructPlayerStats*>(p_data);
		return true;
	}

	bool EntityFactory::InitializeRenderer(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<Graphics::Renderer>(r_id) =
			(p_data == nullptr) ?
			Graphics::Renderer()
			:
			*reinterpret_cast<Graphics::Renderer*>(p_data);
		return true;
	}

	//bool EntityFactory::InitializeScriptComponent(const EntityID& r_id, void* p_data)
	//{
	//	EntityManager::GetInstance().Get<ScriptComponent>(r_id) =
	//		(p_data == nullptr) ?
	//		ScriptComponent()
	//		:
	//		*reinterpret_cast<ScriptComponent*>(p_data);
	//	return true;
	//}
    
	bool EntityFactory::InitializeCamera(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<Graphics::Camera>(r_id) =
			(p_data == nullptr) ?
			Graphics::Camera()
			:
			*reinterpret_cast<Graphics::Camera*>(p_data);
		return true;
	}


	EntityID EntityFactory::CreateFromPrefab(const char* p_prefab)
	{
		EntityID id = CreateEntity();

		// if the prefab exists in the current list
		if (m_prefabs.prefabs.count(p_prefab))
		{
			//Assign(id, m_prefabs.prefabs.at(p_prefab));
			for (const ComponentID& componentID : m_prefabs.prefabs[p_prefab])
			{
				LoadComponent(id, componentID, nullptr);
			}
		}
		std::string str = "Created Entity-";
		str += std::to_string(id);
		engine_logger.AddLog(false, str, __FUNCTION__);
		return id;
	}

	bool EntityFactory::LoadComponent(EntityID id, const ComponentID& r_component, void* p_data)
	{
		if (!EntityManager::GetInstance().IsEntityValid(id))
			return false;
		// if the prefab exists in the current list
		Assign(id, { r_component });
		return std::invoke(m_initializeComponent[r_component], this, id, p_data);
	}

	void EntityFactory::Assign(EntityID id, const std::initializer_list<ComponentID>& var)
	{
		for (const ComponentID& r_type : var)
		{
			if (m_componentMap.find(r_type) != m_componentMap.end())
			{
				p_entityManager->Assign(id, r_type);
			}
		}
		p_entityManager->UpdateVectors(id);
	}
}
