/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatAttackScript.h
 \date     21-11-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations for functions used for a grey cat's movement states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "CatScript.h"

namespace PE
{
	// ----- CAT MOVEMENT PLAN STATE ----- //
	class CatMovementPLAN : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~CatMovementPLAN() override { p_data = nullptr; }

		// ----- Public Functions ----- //
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateExit(EntityID id) override;

		/*!***********************************************************************************
		 \brief Resets the energy level of the cat and disables all the path nodes.
		*************************************************************************************/
		void ResetValues();

		// ----- Getter ----- //
		virtual std::string_view GetName() override { return "MovementPLAN"; }

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
	};


	// ----- CAT MOVEMENT EXECUTE STATE ----- //
	class CatMovementEXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~CatMovementEXECUTE() override { p_data = nullptr; }

		// ----- Public Functions ----- //
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateExit(EntityID id) override;

		// ----- Public Getters ----- //
		virtual std::string_view GetName() override { return "MovementEXECUTE"; }

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
	};
}