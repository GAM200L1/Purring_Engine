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
		EntityID orangeCatID{ 0 }; // id of the seismic attack
		EntityID seismicID{ 0 }; // id of the seismic attack
		EntityID telegraphID{ 0 }; // id of the UI

		int damage{ 2 };

		// projectile variables
		float seismicRadius{ 3.f };
		float seismicDelay{ 0.f };
		//float seismicLifeTime{ 1.f };
		float seismicForce{ 500.f };

		// for syncing animation with firing
		unsigned int seismicSlamAnimationIndex{ 4 };

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
		/*!***********************************************************************************
		\brief Destructor for OrangeCatAttakc_v2_0PLAN
		*************************************************************************************/
		virtual ~OrangeCatAttack_v2_0PLAN() 
		{ 
			p_attackData = nullptr;
			/*p_mouseClick = nullptr;
			p_mouseClickedPrevious = nullptr;*/
		}

		/*!***********************************************************************************
		\brief Set up the state and subscribe to the collision events

		\param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void Enter(EntityID id);

		/*!***********************************************************************************
		\brief Checks if its state should change

		\param[in,out] id - ID of instance of script
		\param[in,out] deltaTime - delta time to update the state with
		\param[in] r_cursorPosition - current cursor position
		\param[in] mouseClicked - is mouse clicked
		\param[in] mouseClickedPrevious - was mouse clicked in previous frame
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime, vec2 const& r_cursorPosition, bool mouseClicked, bool mouseClickedPrevious);

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
				Implemented just to follow parent virtual

		 \param setToggle - toggle telegraphs or not
		 \param ignoreSelected - not used for this type
		*************************************************************************************/
		virtual void ToggleTelegraphs(bool setToggle, bool ignoreSelected);

	private:
		// ----- Private Variables ----- //
		// data
		OrangeCatAttackVariables* p_attackData = nullptr;

		// Telegraph colors
		vec4 const m_defaultColor{ 0.545f, 1.f, 0.576f, 1.f };
		vec4 const m_hoverColor{ 1.f, 0.859f, 0.278f, 1.f };
		vec4 const m_selectColor{ 1.f, 0.784f, 0.f, 1.f };

		// checks
		bool m_firstUpdate{ true };
	};

	class OrangeCatAttack_v2_0EXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		/*!***********************************************************************************
		\brief Destructor for OrangeCatAttack_v2_0EXECUTE
		*************************************************************************************/
		virtual ~OrangeCatAttack_v2_0EXECUTE() 
		{ 
			p_attackData = nullptr;
		}

		/*!***********************************************************************************
		\brief Set up the state and subscribe to the collision events

		\param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void StateEnter(EntityID id);

		/*!***********************************************************************************
		\brief Updates state

		\param[in,out] id - ID of instance of script
		\param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime);

		/*!***********************************************************************************
		\brief Unsubscribes from the collision events
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events

		 \param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void StateExit(EntityID id);

		/*!***********************************************************************************
		 \brief Gets the name of the state

		 \param[out] std::string name of the state
		*************************************************************************************/
		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		
		// data
		EntityID m_catID;
		OrangeCatAttackVariables* p_attackData = nullptr;

		float m_seismicDelay;
		bool m_seismicSlammed{ false };
		bool m_seismicFinished{ false };

		unsigned int m_seismicPrevAnimationFrame{ 0 };

		int m_collisionEnterEventListener; // stores the handler for collision enter event

	private:
		// ----- Private Functions ----- //
		/*!***********************************************************************************
		 \brief Function to handle Collision Events

		 \param[in] r_CE - Collision event data.
		*************************************************************************************/
		void SeismicCollided(const Event<CollisionEvents>& r_CE);

		/*!***********************************************************************************
		 \brief Function to handle damaging of cats and rats

		 \param[in] id1 - first entity in collision event
			   [in] id2 - second entity in collision event
		*************************************************************************************/
		void SeismicHitCatOrRat(EntityID id1, EntityID id2);

		/*!***********************************************************************************
		 \brief Plays audio when seismic ruptures
		*************************************************************************************/
		void PlaySeismicAudio();
	};
}