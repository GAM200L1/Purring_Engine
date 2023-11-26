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
		/*!***********************************************************************************
		 \brief Subscribes to input and collision events and resets the variables of the state.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;

		/*!***********************************************************************************
		 \brief If the cat is selected, displays telegraphs indicating the directions 
						the player can attack in. If the player clicks on the telegraphs, the cat
						attack direction is stored for the execution phase.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		 \param[in] deltaTime - Time in seconds since the last frame.
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;

		/*!***********************************************************************************
		 \brief Unsubscribe from events.
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
		 \brief Deactivate all telegraphs

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;

		// ----- Getter ----- //
		virtual std::string_view GetName() { return "AttackPLAN"; }

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
		bool m_showBoxes{ false }; // True if all the telegraphs should be displayed
		bool m_mouseClick{ false }; // True if the mouse has been clicked this frame
		vec3 const m_defaultColor{ 0.545f, 1.f, 0.576f };
		vec3 const m_hoverColor{ 1.f, 0.859f, 0.278f };
		vec3 const m_selectColor{ 1.f, 0.784f, 0.f };
		std::set<EntityID> m_ignoresTelegraphs; // Telegraphs that have been disabled bc they're overlapping with another cat
		bool m_checkedIgnored{ false }; // True if we have checked if the telegraphs overlap with any other cats

		bool m_mouseClicked{ false }; // Set to true when the mouse is pressed, false otherwise
		bool m_mouseClickedPrevious{ false }; // Set to true if the mouse was pressed in the previous frame, false otherwise
		int m_mouseEventListener; // Stores the handler for the mouse click event
		int m_triggerEnterEventListener; // Stores the handler for the collision enter event
		int m_triggerStayEventListener; // Stores the handler for the collision stay event

		// ----- Private Functions ----- //
		void ResetSelection();
		void OnMouseClick(const Event<MouseEvents>& r_ME);
		void CatInTelegraph(const Event<CollisionEvents>& r_TE);
	};

	// ----- CAT ATTACK EXECUTE STATE ----- //
	class CatAttackEXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~CatAttackEXECUTE() { p_data = nullptr; }

		// ----- Public Functions ----- //
		/*!***********************************************************************************
		 \brief Subscribes to input and collision events and resets the variables of the state.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void StateEnter(EntityID id);

		/*!***********************************************************************************
		 \brief Update the visuals of the cat and projectile.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		 \param[in] deltaTime - Time in seconds since the last frame.
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Unsubscribe from all event listeners.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
		 \brief Disable the projectile and reset the player attack variables.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void StateExit(EntityID id);

		// ----- Getter ----- //
		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		// ----- Private Variables ----- //
		CatScriptData* p_data;
		float m_attackDuration; // Time in seconds the attack will last
		bool m_bulletCollided{ false }; // Set to true if the projectile has hit a rat
		bool m_projectileFired; // True if the projectile has been launched
		float m_bulletDelay{ 0.f }; // Time in seconds before the projectile should be launched
		vec2 m_bulletImpulse; // Direction and intensity of the projectile
		int m_collisionEnterEventListener; // Stores the handler for the collision enter event
		bool playShootOnce{false};
		
		// ----- Private Functions ----- //
		/*!***********************************************************************************
		 \brief Callback function for collision event. Determines if the projectile has 
						collided with a rat.

		 \param[in] r_CE - Collision event data
		*************************************************************************************/
		void ProjectileHitRat(const Event<CollisionEvents>& r_CE);

	};
}