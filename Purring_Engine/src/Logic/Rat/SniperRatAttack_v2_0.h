/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     SniperRatAttack_v2_0.h
 \date     09-03-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains declarations of the struct for the Sniper Rat's attacks.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "RatScript_v2_0_Data.h"

namespace PE
{
	struct SniperRatAttack_v2_0 : AttackDataBase_v2_0
	{
		// ----- Public variables ----- //

		// --- Misc variables
		// Ensures that the attack feedback only plays once. Set to true once it has been played.
		bool attackFeedbackOnce{ false }; 
		float projectileStartTime{ 0.16f }; // Time in seconds since the anim starts playing that the projectile should get launched
		float attackDuration{ 0.f }; // Time in seconds before the attack is considered done
		float timePassed{ 0.f };		// Time in seconds since the attack first started

		// --- Movement variables
		float movementAngularOffset{ 1.309f }; // Angle in degrees from the direction of the cat from the rat to angle the movement direction of the rat
		// 1.309f rad = ~75 deg

		// --- Attack variables
		EntityID spikeballID{ 0 }; // id of the projectile
		vec2 shotTargetPosition{}; // Position to aim the bullet at. Set during the movement state.
		float bulletForce{ 100.f };
		float bulletRange{ 3.f };

		
		// ----- Constructors ----- //

		/*!***********************************************************************************
		\brief Constructs the base attack object.
					
		\param _mainID - ID of the rat that this attack belongs to.
		*************************************************************************************/
		SniperRatAttack_v2_0(EntityID _mainID) : AttackDataBase_v2_0{_mainID} { /* Empty by design */ }

		/*!***********************************************************************************
		\brief Destructor.
		*************************************************************************************/
		~SniperRatAttack_v2_0() override { /* Empty by design */ }

		// ----- Public methods ----- //

		/*!***********************************************************************************
		\brief Initializes the attack (e.g. position objects at the start position etc.).
				Does nothing by default.
		*************************************************************************************/
		virtual void InitAttack() override;

		/*!***********************************************************************************
		\brief Executes the attack. Called once every frame during update.
				Returns true when the attack is done executing.

		\param deltaTime - Time in seconds since the last frame.

		\return Returns true when the attack is done executing, false otherwise.
		*************************************************************************************/
		virtual bool ExecuteAttack(float deltaTime) override;

		/*!***********************************************************************************
		\brief Spawns the objects required for this attack.
		*************************************************************************************/
		virtual void CreateAttackObjects() override;

		/*!***********************************************************************************
		\brief Disable all the attack objects. Called when the attack state ends and
				when the rat dies.
		*************************************************************************************/
		virtual void DisableAttackObjects() override;

		/*!***********************************************************************************
		\brief Checks if the collision involved the attack objects and relevant entities and
				handles it (e.g. calls the "LoseHealth" function on the victim entity).

		\param entity1 - One of the entities involved in the collision event.
		\param entity2 - One of the entities involved in the collision event.

		\return Returns true if the collision involved the attack object and/or the rat and a cat, 
				false otherwise.
		*************************************************************************************/
		virtual bool OnCollisionEnter(EntityID entity1, EntityID entity2) override;

		/*!***********************************************************************************
		\brief Picks a target position to move towards during the movement state and returns it.
				Calls RatScript_v2_0::SetTarget();

		\return vec2 - Target position to move towards during the movement state.
		*************************************************************************************/
		virtual vec2 PickTargetPosition() override;

	private:
		vec2 m_bulletImpulse{};
		vec2 m_direction{};
	}; // end of SniperRatAttack_v2_0
} // end of namespace PE
