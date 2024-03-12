/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatPlanningState_v2_0.h
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Planning State

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Logic/LogicSystem.h"
#include "Logic/StateManager.h"

#include "Events/MouseEvent.h"
#include "Events/CollisionEvent.h"

#include "CatAttackBase_v2_0.h"
#include "CatMovementStates_v2_0.h"


namespace PE
{
	class Cat_v2_0PLAN : public State
	{
	public:
		
		// ----- Destructor ----- //
		Cat_v2_0PLAN() = delete;
		/*!***********************************************************************************
		\brief Constructor for Cat_v2_0PLAN

		\param[in] p_catAttackTypePLAN - grey or orange cat plan
		\param[in] p_movementPLAN - movement plan
		*************************************************************************************/
		Cat_v2_0PLAN(CatAttackBase_v2_0* p_catAttackTypePLAN, CatMovement_v2_0PLAN* p_catMovementPLAN) : p_catAttack{ p_catAttackTypePLAN }, p_catMovement{ p_catMovementPLAN }, p_data{ nullptr } {}

		/*!***********************************************************************************
		\brief Destructor for Cat_v2_0PLAN
		*************************************************************************************/
		virtual ~Cat_v2_0PLAN() 
		{
			delete p_catMovement;
			delete p_catAttack;
			p_data = nullptr;
		}

		/*!***********************************************************************************
			\brief Set up the state and subscribe to the collision events

			\param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;

		/*!***********************************************************************************
			\brief Checks if its state should change

			\param[in,out] id - ID of instance of script
			\param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		*************************************************************************************/
		virtual void StateCleanUp();
		
		/*!***********************************************************************************
			\brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;

		/*!***********************************************************************************
			\brief Get the name of the state

			\return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() { return "PLANNING"; }

	private:
		CatAttackBase_v2_0* p_catAttack{ nullptr }; 
		CatMovement_v2_0PLAN* p_catMovement{ nullptr }; 
		CatScript_v2_0Data* p_data{ nullptr };

		int m_mouseClickEventListener{};
		int m_mouseReleaseEventListener{};

		bool m_mouseRelease{ false };
		bool m_mouseClicked{ false };
		bool m_mouseClickPrevious{ false };
		bool m_collidedPreviously{ false };

		int m_doubleClick{ 0 };
		float m_timer{ 0.f };
		bool m_moving{ false };

		vec2 m_prevCursorPosition{ 0.f, 0.f };

		/*!***********************************************************************************
		 \brief Function to handle mouse click events for GreyCatPLAN

		 \param[in] r_ME - Mouse event data.
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

		/*!***********************************************************************************
		 \brief Function to handle mouse release events for GreyCatPLAN

		 \param[in] r_ME - Mouse event data.
		*************************************************************************************/
		void OnMouseRelease(const Event<MouseEvents>& r_ME);
	};
}