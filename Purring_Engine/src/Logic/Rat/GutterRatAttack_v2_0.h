/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     GutterRatAttack_v2_0.h
 \date     09-03-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations of the struct for the Gutter Rat's attacks.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "RatScript_v2_0_Data.h"

namespace PE
{
	struct GutterRatAttack_v2_0 : AttackDataBase_v2_0
	{
		// ----- Public variables ----- //

		// --- Misc variables
		// Ensures that the attack feedback only plays once. Set to true once it has been played.
		bool attackFeedbackOnce{ false }; 
		float attackDuration{}; // Time in seconds before the attack is considered done

		// --- Attack variables
		float attackRadius{101.f};			 // Radius of attack collider
		EntityID attackTelegraphEntityID{0}; // id of cross attack telegraph

		// ----- Constructors ----- //

		/*!***********************************************************************************
		\brief Constructs the base attack object.
					
		\param _mainID - ID of the rat that this attack belongs to.
		*************************************************************************************/
		GutterRatAttack_v2_0(EntityID _mainID) : AttackDataBase_v2_0{_mainID} { /* Empty by design */ }

		/*!***********************************************************************************
		\brief Destructor.
		*************************************************************************************/
		~GutterRatAttack_v2_0() override { /* Empty by design */ }

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
		\brief Creates the attack range object for the Gutter Rat attack.
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
	}; // end of GutterRatAttack_v2_0
} // end of namespace PE
