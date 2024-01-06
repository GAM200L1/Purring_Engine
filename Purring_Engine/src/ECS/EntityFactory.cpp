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
#include "Logic/LogicSystem.h"
#include "GUISystem.h"
#include "Graphics/GUIRenderer.h"
#include "Graphics/Text.h"
#include "AudioManager/AudioComponent.h"
#include "Hierarchy/HierarchyManager.h"
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
				if (r_componentCreator == EntityManager::GetInstance().GetComponentID<EntityDescriptor>())
					continue;
				LoadComponent(clone, r_componentCreator,
					p_entityManager->GetComponentPoolPointer(r_componentCreator)->Get(id));
			}
			EntityManager::GetInstance().Get<EntityDescriptor>(clone).children.clear();
			if (EntityManager::GetInstance().Get<EntityDescriptor>(clone).parent.has_value())
			{
				Hierarchy::GetInstance().AttachChild(EntityManager::GetInstance().Get<EntityDescriptor>(clone).parent.value(), clone);
			}
			return clone;
		}
		else
		{
			throw;
		}
	}
	
	void EntityFactory::LoadComponents()
	{
		m_initializeComponent.emplace(p_entityManager->GetComponentID<EntityDescriptor>(),		&EntityFactory::InitializeED);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<RigidBody>(),				&EntityFactory::InitializeRigidBody);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Collider>(),				&EntityFactory::InitializeCollider);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Transform>(),				&EntityFactory::InitializeTransform);
		//m_initializeComponent.emplace(p_entityManager->GetComponentID<PlayerStats>(),			&EntityFactory::InitializePlayerStats);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Graphics::Renderer>(),	&EntityFactory::InitializeRenderer);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<ScriptComponent>(),		&EntityFactory::InitializeScriptComponent);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Graphics::Camera>(),		&EntityFactory::InitializeCamera);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<GUI>(),					&EntityFactory::InitializeGUI);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<AnimationComponent>(),	&EntityFactory::InitializeAnimationComponent);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<Graphics::GUIRenderer>(), &EntityFactory::InitializeGUIRenderer);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<TextComponent>(),			&EntityFactory::InitializeTextComponent);
		m_initializeComponent.emplace(p_entityManager->GetComponentID<AudioComponent>(),		&EntityFactory::InitializeAudioComponent);
	}


	bool EntityFactory::InitializeED(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<EntityDescriptor>(r_id) =
			(p_data == nullptr) ?
			EntityDescriptor()
			:
			*reinterpret_cast<EntityDescriptor*>(p_data);
		//EntityManager::GetInstance().Get<EntityDescriptor>(r_id).name += "-Copy-" + std::to_string(r_id);
		return true;
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

	bool EntityFactory::InitializeScriptComponent(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<ScriptComponent>(r_id) =
			(p_data == nullptr) ?
			ScriptComponent()
			:
			*reinterpret_cast<ScriptComponent*>(p_data);

		for (auto [k, v] : EntityManager::GetInstance().Get<ScriptComponent>(r_id).m_scriptKeys)
		{
			try
			{
				PE::LogicSystem::m_scriptContainer[k]->GetScriptData(r_id);
			}
			catch (const std::out_of_range& err)
			{
				UNREFERENCED_PARAMETER(err);
				engine_logger.AddLog(false, "Script Data did not exist!! Attatching new one...", __FUNCTION__);
				PE::LogicSystem::m_scriptContainer[k]->OnAttach(r_id);
			}

			if (p_data != nullptr)
			{
				EntityID frm{ MAXSIZE_T };
				for (auto [id, idx] : EntityManager::GetInstance().GetComponentPool<ScriptComponent>().idxMap)
				{
					if (EntityManager::GetInstance().GetComponentPool<ScriptComponent>().Get(id) == p_data)
					{
						frm = id;
						break;
					}
				}
				

				rttr::instance to = PE::LogicSystem::m_scriptContainer[k]->GetScriptData(r_id);

				try
				{
					rttr::instance from = PE::LogicSystem::m_scriptContainer[k]->GetScriptData(frm);

					for (auto& prop : rttr::type::get_by_name(k).get_properties())
					{
						if (prop.get_type().get_name() == "float")
						{
							float val = prop.get_value(from).get_value<float>();
							prop.set_value(to, val);
						}
						else if (prop.get_type().get_name() == "enumPE::PlayerState")
						{
							PE::PlayerState val = prop.get_value(from).get_value<PE::PlayerState>();
							prop.set_value(to, val);
						}
						else if (prop.get_type().get_name() == "int")
						{
							int val = prop.get_value(from).get_value<int>();
							prop.set_value(to, val);
						}
						else if (prop.get_type().get_name() == "unsigned__int64")
						{
							EntityID val = prop.get_value(from).get_value<EntityID>();
							prop.set_value(to, val);
						}
						else if (prop.get_type().get_name() == "bool")
						{
							bool val = prop.get_value(from).get_value<bool>();
							prop.set_value(to, val);
						}
						else if (prop.get_type().get_name() == "classstd::vector<unsigned__int64,classstd::allocator<unsigned__int64> >")
						{
							std::vector<EntityID> val = prop.get_value(from).get_value<std::vector<EntityID>>();
							prop.set_value(to, val);
						}
						else if (prop.get_type().get_name() == "structPE::vec2")
						{
							PE::vec2 val = prop.get_value(from).get_value<PE::vec2>();
							prop.set_value(to, val);
						}
						else if (prop.get_type().get_name() == "classstd::vector<structPE::vec2,classstd::allocator<structPE::vec2> >")
						{
							std::vector<PE::vec2> val = prop.get_value(from).get_value<std::vector<PE::vec2>>();
							prop.set_value(to, val);
						}
					}
				}
				catch (const std::out_of_range& err)
				{
					UNREFERENCED_PARAMETER(err);
					engine_logger.AddLog(false, "Script Data did not exist!! skipping copy data...", __FUNCTION__);
					
				}
			}
		}
		return true;
	}
    
	bool EntityFactory::InitializeCamera(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<Graphics::Camera>(r_id) =
			(p_data == nullptr) ?
			Graphics::Camera()
			:
			*reinterpret_cast<Graphics::Camera*>(p_data);
		return true;
	}
    
	bool EntityFactory::InitializeGUIRenderer(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<Graphics::GUIRenderer>(r_id) =
			(p_data == nullptr) ?
			Graphics::GUIRenderer()
			:
			*reinterpret_cast<Graphics::GUIRenderer*>(p_data);
		return true;
	}

	bool EntityFactory::InitializeGUI(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<GUI>(r_id) =
			(p_data == nullptr) ?
			GUI()
			:
			*reinterpret_cast<GUI*>(p_data);
		return true;
	}

	bool EntityFactory::InitializeAnimationComponent(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<AnimationComponent>(r_id) =
			(p_data == nullptr) ?
			AnimationComponent()
			:
			*reinterpret_cast<AnimationComponent*>(p_data);
		return true;
	}

	bool EntityFactory::InitializeTextComponent(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<TextComponent>(r_id) =
			(p_data == nullptr) ?
			TextComponent()
			:
			*reinterpret_cast<TextComponent*>(p_data);
		return true;
	}

	bool EntityFactory::InitializeAudioComponent(const EntityID& r_id, void* p_data)
	{
		EntityManager::GetInstance().Get<AudioComponent>(r_id) =
			(p_data == nullptr) ?
			AudioComponent()
			:
			*reinterpret_cast<AudioComponent*>(p_data);
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
