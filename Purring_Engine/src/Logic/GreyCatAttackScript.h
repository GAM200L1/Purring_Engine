/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackScript.h
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
	enum EnumGreyCatAttackDirection
	{
		EAST = 0b0001,
		NORTH = 0b0010,
		WEST = 0b0100,
		SOUTH = 0b1000
	};

	struct GreyCatAttackScriptData
	{
		// reference entities
		EntityID catID{ 1 };
		EntityID projectileID{ 0 };

		// cat stats
		int catHealth{ 0 };

		// planning stage variables
		std::set<std::pair<EnumGreyCatAttackDirection, EntityID>> selectBoxIDs;

		// attack variables
		int attackDamage{ 0 };
		int requiredAttackPoints{ 0 };
		unsigned attackDirection{ 0 };
		
		// bullet variables
		float bulletRange{ 3.f };
		float bulletLifeTime{ 0.f };
		float bulletForce{ 0.f };

		// state manager
		StateMachine* m_stateManager;
	};

	class GreyCatAttackScript : public Script
	{
	public:
		virtual void Init(EntityID id);

		virtual void Update(EntityID id, float deltaTime);

		virtual void Destroy(EntityID id);

		virtual void OnAttach(EntityID id);

		virtual void OnDetach(EntityID id);

		std::map<EntityID, GreyCatAttackScriptData>& GetScriptData();

		rttr::instance GetScriptData(EntityID id);

		virtual ~GreyCatAttackScript();

	public:
		std::map<EntityID, GreyCatAttackScriptData> m_scriptData;
	
	private:
		void CreateAttackSelectBoxes(EntityID id, bool isSide, bool isNegative);
	};

	class GreyCatAttackPLAN : public State
	{
	public:
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateExit(EntityID id) override;

		virtual std::string_view GetName() override;

		virtual ~GreyCatAttackPLAN() { p_data = nullptr; }

	private:
		GreyCatAttackScriptData* p_data;
		bool m_showBoxes{ true };
		bool m_mouseClick{ false };

		void OnMouseClick(const Event<MouseEvents>& r_ME);
	};

	class GreyCatAttackEXECUTE : public State
	{
	public:
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateExit(EntityID id) override;

		virtual std::string_view GetName() override;

		virtual ~GreyCatAttackEXECUTE() { p_data = nullptr; }

	private:
		GreyCatAttackScriptData* p_data;
	};
}