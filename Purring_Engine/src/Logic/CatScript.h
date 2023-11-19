/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatAttackScript.h
 \date     15-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains declarations for functions used for a grey cat's attack state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "StateManager.h"
#include "Events/EventHandler.h"

namespace PE
{
	//! Enum denoting cardinal directions for attack
	enum EnumCatAttackDirection
	{
		EAST = 0b0001,
		NORTH = 0b0010,
		WEST = 0b0100,
		SOUTH = 0b1000
	};

	//! struct with variables needed by cat scripts
	struct CatScriptData
	{
		// reference entities
		EntityID catID{ 1 };
		EntityID projectileID{ 0 };

		// cat stats
		int catHealth{ 0 };		

		// attack variables
		int attackDamage{ 0 };
		int requiredAttackPoints{ 0 };
		unsigned attackDirection{ 0 };
		std::set<std::pair<EnumCatAttackDirection, EntityID>> selectBoxIDs;
		
		// bullet variables
		float bulletRange{ 3.f };
		float bulletLifeTime{ 0.f };
		float bulletForce{ 0.f };

		// state manager
		StateMachine* m_stateManager;
	};


	// ----- CAT SCRIPT ----- //
	class CatScript : public Script
	{
	public:
		// ----- Public Variables ------ //

		std::map<EntityID, CatScriptData> m_scriptData;

	public:

		// ----- Destructor ----- //
		virtual ~CatScript()
		{
			for (auto& [key, value] : m_scriptData)
			{
				delete value.m_stateManager;
			}
		}

		// ----- Public Functions ----- //
		virtual void Init(EntityID id)
		{
			m_scriptData[id].m_stateManager = new StateMachine{};
			m_scriptData[id].m_stateManager->ChangeState(new CatAttackPLAN{}, id);
		}

		virtual void Update(EntityID id, float deltaTime)
		{
			m_scriptData[id].m_stateManager->Update(id, deltaTime);
		}

		virtual void Destroy(EntityID id){}

		virtual void OnAttach(EntityID id)
		{
			// check if the given entity has transform, rigidbody, and collider. if it does not, assign it one
			if (!EntityManager::GetInstance().Has<Transform>(id))
			{
				EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Transform>() });
			}
			if (!EntityManager::GetInstance().Has<RigidBody>(id))
			{
				EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
			}
			if (!EntityManager::GetInstance().Has<Collider>(id))
			{
				EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Collider>() });
				EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider(); // cat default colliders is circle
			}

			if (m_scriptData.find(id) == m_scriptData.end())
			{
				m_scriptData[id] = CatScriptData{};
			}
			else
			{
				delete m_scriptData[id].m_stateManager;
				m_scriptData[id] = CatScriptData{};
			}

			CreateAttackSelectBoxes(id, true, false); // east box
			CreateAttackSelectBoxes(id, true, true); // west box
			CreateAttackSelectBoxes(id, false, false); // north box
			CreateAttackSelectBoxes(id, false, true); // south box
		}

		virtual void OnDetach(EntityID id)
		{
			if (m_scriptData.find(id) != m_scriptData.end())
			{
				delete m_scriptData[id].m_stateManager;
				m_scriptData.erase(id);
			}
			for (auto const& boxID : m_scriptData[id].selectBoxIDs)
			{
				EntityManager::GetInstance().RemoveEntity(boxID.second);
			}
		}

		// ----- Getters/RTTR ----- //
		std::map<EntityID, CatScriptData>& GetScriptData(){ return m_scriptData; }

		rttr::instance GetScriptData(EntityID id){ return rttr::instance(m_scriptData.at(id)); }
	
	private:

		// ----- Private Functions ----- //
		void CreateAttackSelectBoxes(EntityID id, bool isSide, bool isNegative);
	};



	// ========== CAT ATTACKS ========== //

	// ----- CAT ATTACK PLANNING STATE ----- //
	class CatAttackPLAN : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~CatAttackPLAN() { p_data = nullptr; }

		// ----- Public Functions ----- //
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateExit(EntityID id) override;

		// ----- Getter ----- //
		virtual std::string_view GetName() override;

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
		bool m_showBoxes{ true };
		bool m_mouseClick{ false };

		// ----- Private Functions ----- //
		void OnMouseClick(const Event<MouseEvents>& r_ME);
	};



	// ----- CAT ATTACK EXECUTE STATE ----- //
	class CatAttackEXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~CatAttackEXECUTE() { p_data = nullptr; }

		// ----- Public Functions ----- //
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateExit(EntityID id) override;

		// ----- Getter ----- //
		virtual std::string_view GetName() override;

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
	};
}