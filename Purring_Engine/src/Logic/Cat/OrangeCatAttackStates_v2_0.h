/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     OrangeCatAttackStates_v2_0.h
 \date     21-11-2023

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Orange Cat Attack class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Logic/LogicSystem.h"
#include "Logic/StateManager.h"

#include "Events/MouseEvent.h"
#include "Events/CollisionEvent.h"

#include "Logic/GameStateController_v2_0.h"
#include "CatAttackBase_v2_0.h"

namespace PE
{
	struct OrangeCatAttackVariables
	{
		EntityID seismicID{ 0 }; // id of the seismic attack
		EntityID telegraphID{ 0 }; // id of the UI

		int damage{ 0 };

		// projectile variables
		float stompRadius{ 20.f };
		float stompLifeTime{ 1.f };
		float stomopForce{ 1000.f };

		/*!***********************************************************************************
		 \brief Creates Stomp Telegraph and actual seismic

		 \param[in] catID - EntityID of the entity to create stomp telegraph for
		*************************************************************************************/
		void CreateSeismicAndTelegraph(EntityID catID);
	};

	class OrangeCatAttack_v2_0PLAN : public CatAttackBase_v2_0
	{
	public:

		// ----- Destructor ----- //
		virtual ~OrangeCatAttack_v2_0PLAN() { p_attackData = nullptr; }

		/*!***********************************************************************************
			\brief Set up the state and subscribe to the collision events

			\param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void Enter(EntityID id);

		/*!***********************************************************************************
			\brief Checks if its state should change

			\param[in,out] id - ID of instance of script
			\param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		*************************************************************************************/
		virtual void CleanUp();

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events

		 \param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void Exit(EntityID id);

		/*!***********************************************************************************
		 \brief Resets the currently selected cat

		 \param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void ResetSelection(EntityID id);

		/*!***********************************************************************************
		 \brief Toggles the attack telegraphs on or off accordingly

		 \param setToggle - toggle telegraphs or not
		 \param ignoreSelected - ignore telegraphs selected or not
		*************************************************************************************/
		virtual void ToggleTelegraphs(bool setToggle, bool ignoreSelected);

		/*!***********************************************************************************
		 \brief Forces number of mouse clicks to 0
		*************************************************************************************/
		virtual void ForceZeroMouse() { m_mouseClick = false; }

	private:
		// ----- Private Variables ----- //
		// pointer to the game state controller
		GameStateController_v2_0* p_gsc;
		// data
		OrangeCatAttackVariables* p_attackData;

		// Telegraph colors
		vec3 const m_defaultColor{ 0.545f, 1.f, 0.576f };
		vec3 const m_hoverColor{ 1.f, 0.859f, 0.278f };
		vec3 const m_selectColor{ 1.f, 0.784f, 0.f };

		// checks
		bool m_mouseClick{ false }; // Set to true when the mouse is pressed, false otherwise
		bool m_mouseClickedPrevious{ false }; // Set to true if the mouse was pressed in the previous frame, false otherwise
		int m_mouseClickEventListener; // Stores the handler for the mouse click event
		int m_mouseReleaseEventListener; // Stores the handler for the mouse release event

	private:
		// ----- Private Functions ----- //
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

	class OrangeCatAttack_v2_0EXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~OrangeCatAttack_v2_0EXECUTE() { p_attackData = nullptr; }

		virtual void StateEnter(EntityID id);

		virtual void StateUpdate(EntityID id, float deltaTime);

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id);

		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		OrangeCatAttackVariables* p_attackData;

		int m_collisionEnterEventListener; // stores the handler for collision enter event

		void SlamHitCat(const Event<CollisionEvents>& r_CE);

		void SlamHitRat(const Event<CollisionEvents>& r_CE);
	};
}