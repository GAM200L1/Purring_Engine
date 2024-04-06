/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatPlanningState_v2_0.h
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Planning State.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "ECS/Entity.h"

namespace PE
{
	struct CatAttackBase_v2_0
	{
		// ----- Destructor ----- //
		virtual ~CatAttackBase_v2_0() {}

		/*!***********************************************************************************
		\brief Set up the state and subscribe to the collision events

		\param[in,out] id - ID of instance of script
		\param[in] p_planMouseClick - pointer to mouse click bool in plan state
		\param[in] p_planMouseClickPrev - pointer to mouse click previous bool in plan state
		*************************************************************************************/
		virtual void Enter(EntityID id) = 0;

		/*!***********************************************************************************
		\brief Checks if its state should change

		\param[in,out] id - ID of instance of script
		\param[in,out] deltaTime - delta time to update the state with
		\param[in] r_cursorPosition - current cursor position
		\param[in] mouseClicked - is mouse clicked
		\param[in] mouseClickedPrevious - was mouse clicked in previous frame
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime, vec2 const& r_cursorPosition, bool mouseClicked, bool mouseClickedPrevious) = 0;

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		*************************************************************************************/
		virtual void CleanUp() = 0;

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events

		 \param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void Exit(EntityID id) = 0;

		/*!***********************************************************************************
		 \brief Resets the currently selected cat

		 \param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void ResetSelection(EntityID id) = 0;
		
		/*!***********************************************************************************
		 \brief Toggles the attack telegraphs on or off accordingly

		 \param setToggle - toggle telegraphs or not
		 \param ignoreSelected - ignore telegraphs selected or not
		*************************************************************************************/
		virtual void ToggleTelegraphs(bool setToggle, bool ignoreSelected) = 0;
	};
}