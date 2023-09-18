/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Prefabs.h
 \date     18-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 	Contains the Prefabs for use for default Entity compositions

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once


// INCLUDES
#include "prpch.h"
#include "Entity.h"
#include "Components.h"
#include "EntityFactory.h"
#include "../Physics/Colliders.h"
#include "../Physics/RigidBody.h"

namespace
{
	typedef bool(*fnptrVoidptrConstruct)(const EntityID& id, void* data);
	typedef bool(*fntprInitialize)(const EntityID& id);
}

namespace PE
{

	bool InitializeRigidBody(const EntityID& id, void* data);
	bool InitializeCollider(const EntityID& id, void* data);
	bool InitializeTransform(const EntityID& id, void* data);

	namespace Prefabs
	{
		std::initializer_list<const char*> GameObject{ "RigidBody", "Collider", "Transform" };
	}

	std::map<std::string, std::initializer_list<const char*>> g_prefabs;
	std::map<std::string, fnptrVoidptrConstruct> g_initializeComponent;

	void LoadComponents()
	{
		g_initializeComponent.emplace(std::make_pair("RigidBody", &InitializeRigidBody));
		g_initializeComponent.emplace(std::make_pair("Collider", &InitializeCollider));
		g_initializeComponent.emplace(std::make_pair("Transform", &InitializeTransform));
	}
	
	void LoadPrefabs()
	{
		LoadComponents();
		g_prefabs.emplace(std::make_pair("GameObject", Prefabs::GameObject));
		//g_prefabConstructor.emplace(std::make_pair("GameObject", &InitializeMemoryGO));

	}

	EntityID CreatePrefab(const char* prefab)
	{
		EntityID id = g_entityFactory->CreateEntity();

		// if the prefab exists in the current list
		if (g_prefabs.count(prefab))
		{
			g_entityFactory->Assign(id, g_prefabs[prefab]);
			for (const ComponentID& componentID : g_prefabs[prefab])
			{
				g_initializeComponent[componentID](id, nullptr);
			}
		}
		return id;
	}


	bool InitializeRigidBody(const EntityID& id, void* data)
	{
		(data == nullptr) ?
			new (g_entityManager->GetPointer<RigidBody>(id)) RigidBody()
			:
			new (g_entityManager->GetPointer<RigidBody>(id)) RigidBody(*static_cast<RigidBody*>(data));
		return true;
	}

	bool InitializeCollider(const EntityID& id, void* data)
	{
		(data == nullptr) ?
			new (g_entityManager->GetPointer<Collider>(id)) Collider()
			:
			new (g_entityManager->GetPointer<Collider>(id)) Collider(*static_cast<Collider*>(data));
		return true;
	}

	bool InitializeTransform(const EntityID& id, void* data)
	{
		(data == nullptr) ?
			new (g_entityManager->GetPointer<Transform>(id)) Transform()
			:
			new (g_entityManager->GetPointer<Transform>(id)) Transform(*static_cast<Transform*>(data));
		return true;
	}
}