/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     GreyCatAttackStates_v2_0.h
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Attack Base class

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
	//! Enum denoting cardinal directions for attack
	enum class EnumCatAttackDirection_v2_0 : int
	{
		NONE = 0,
		EAST = 1,
		NORTH = 2,
		WEST = 3,
		SOUTH = 4
	};

	struct GreyCatAttackVariables
	{
		// id of the projectile
		EntityID projectileID{ 0 };

		// damage of the attack
		int damage{ 1 };

		// Telegraph variables
		EnumCatAttackDirection_v2_0 attackDirection{ EnumCatAttackDirection_v2_0::NONE }; // Direction of attack chosen
		std::map<EnumCatAttackDirection_v2_0, EntityID> telegraphIDs{}; // IDs of entities used to visualise the directions the player can attack in

		// projectile variables
		float bulletDelay{ 0.f }; // for after the bullet fire frame if slight 
		float bulletRange{ 3.f };
		float bulletLifeTime{ 1.f };
		float bulletForce{ 1000.f };

		// for syncing
		unsigned int bulletFireAnimationIndex{ 3 };

		/*!***********************************************************************************
			\brief Creates Entities assigned to the grey/main cat as its telegraphs and projectile

			\param[in] catID - id of the cat which projectile and telegraphs are assigned to
			\param[in] catType - main or grey cat, to decide for piercing projectile
		*************************************************************************************/
		void CreateProjectileAndTelegraphs(EntityID catID, bool isMainCat);
	};

	class GreyCatAttack_v2_0PLAN : public CatAttackBase_v2_0
	{
	public:
		
		// ----- Destructor ----- //
		~GreyCatAttack_v2_0PLAN() { p_attackData = nullptr; p_gsc = nullptr; }

		// ----- Public Functions ----- //
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
		 \brief Activate all attack telegraphs

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		 \param[in] bulletRange - Distance projectile travels.
		*************************************************************************************/
		static void CreateProjectileTelegraphs(EntityID id, float bulletRange, std::map<EnumCatAttackDirection_v2_0, EntityID>& r_telegraphIDs);

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
		GameStateController_v2_0* p_gsc; // pointer to the game state controller
		GreyCatAttackVariables* p_attackData; // attack data for the cat

		bool m_showTelegraphs{ false }; // True if telegraphs are to be displayed

		// Telegraph colors
		vec4 const m_defaultColor{ 0.545f, 1.f, 0.576f, 1.f };
		vec4 const m_hoverColor{ 1.f, 0.859f, 0.278f, 1.f };
		vec4 const m_selectColor{ 1.f, 0.784f, 0.f, 1.f };

		bool m_rightMouseClick{ false }; // set to true when mouse is right clicked
		bool m_mouseClick{ false }; // set to true when mouse is clicked
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

	class GreyCatAttack_v2_0EXECUTE : public State
	{
	public:

		// ----- Destructor ----- //
		virtual ~GreyCatAttack_v2_0EXECUTE() { p_attackData = nullptr; }

		/*!***********************************************************************************
			\brief Set up the state and subscribe to the collision events

			\param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void StateEnter(EntityID id);

		/*!***********************************************************************************
			\brief Checks if its state should change

			\param[in,out] id - ID of instance of script
			\param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
			\brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id);

		// ----- Getter ----- //
		/*!***********************************************************************************
		 \brief Gets the name of the state

		 \param[out] std::string_view - name of the state
		*************************************************************************************/
		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:

		// cat attacks
		GreyCatAttackVariables* p_attackData;

		EntityID m_catID{ 0 };

		vec2 m_bulletImpulse;
		float m_bulletDelay;
		float m_bulletLifetime; // how long the bullet will persist
		int m_collisionEventListener{}, m_triggerEventListener;

		bool m_projectileFired{ false };

		/*!***********************************************************************************
		 \brief Handles collision events when non-trigger projectile collides with rat or cat

		 \param[in] r_CE - Collision event data.
		*************************************************************************************/
		void ProjectileCollided(const Event<CollisionEvents>& r_CE);

		/*!***********************************************************************************
		 \brief Handles collision events when trigger projectile collides with rat or cat

		 \param[in] r_CE - Collision event data.
		*************************************************************************************/
		void TriggerHit(const Event<CollisionEvents>& r_CE);

		/*!***********************************************************************************
		 \brief Function to check if entities involved are cats or rats and to damage cat or rat
				accordingly

		 \param[in] id1 - ID of the first entity to check
		 \param[in] id2 - ID of the second entity to check
		*************************************************************************************/
		bool CollideCatOrRat(EntityID id1, EntityID id2);
	};
}