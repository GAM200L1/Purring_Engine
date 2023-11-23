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

		void ShowAttackSelection(EntityID id, vec2 const& r_cursorPosition);

		// ----- Getter ----- //
		virtual std::string_view GetName() { return "AttackPLAN"; }

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
		bool m_showBoxes{ false };
		bool m_mouseClick{ false };
		vec3 const m_defaultColor{ 0.545f, 1.f, 0.576f };
		vec3 const m_hoverColor{ 1.f, 0.859f, 0.278f };
		vec3 const m_selectColor{ 1.f, 0.784f, 0.f };

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
		virtual void StateEnter(EntityID id);

		virtual void StateUpdate(EntityID id, float deltaTime);

		virtual void StateExit(EntityID id);

		// ----- Getter ----- //
		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
		float m_attackDuration;
		bool m_bulletCollided{ false };

		// ----- Private Functions ----- //
		void ProjectileHitRat(const Event<CollisionEvents>& r_CE);

	};
}