/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatAttackScript.h
 \date     15-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains definitions for functions used for a grey cat's attack state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "CatScript.h"

namespace PE
{
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
		bool m_showBoxes{ false };
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
		virtual void StateEnter(EntityID id) { id; }

		virtual void StateUpdate(EntityID id, float deltaTime) { id; deltaTime; }

		virtual void StateExit(EntityID id) { id; }

		// ----- Getter ----- //
		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
	};
}