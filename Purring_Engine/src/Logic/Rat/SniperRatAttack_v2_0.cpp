/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     SniperRatAttack_v2_0.cpp
 \date     09-03-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu
 \par      code %:     80%
 \par      changes:    Definition of all the attack functions.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     20%
 \par      changes:    06-02-2024
											 Definition of movement functions.

 \brief
	This file contains definitions for functions for the Sniper Rat's attacks.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "RatScript_v2_0.h"
#include "SniperRatAttack_v2_0.h"
#include "ResourceManager/ResourceManager.h"

//#define DEBUG_PRINT

namespace PE
{
	void SniperRatAttack_v2_0::InitAttack()
	{
		m_bulletImpulse = vec2{ 0.f, 0.f };

		// calculate the impulse of the bullet
		m_direction = shotTargetPosition - RatScript_v2_0::GetEntityPosition(mainID);
		m_bulletImpulse = m_direction.GetNormalized() * bulletForce;
		
		RatScript_v2_0::PositionEntity(spikeballID, RatScript_v2_0::GetEntityPosition(mainID) + m_direction.GetNormalized() * std::abs(RatScript_v2_0::GetEntityScale(mainID).x) * 0.5f);
		EntityManager::GetInstance().Get<RigidBody>(spikeballID).velocity.Zero();

		attackFeedbackOnce = false;
	}


	bool SniperRatAttack_v2_0::ExecuteAttack(float deltaTime)
	{
		if (!attackFeedbackOnce)
		{
			attackFeedbackOnce = true;
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAttackAudio(this->mainID);
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(mainID, EnumRatAnimations::ATTACK);
			attackDuration = GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetAnimationDuration(mainID);
			timePassed = 0.f;
		}
		else if (attackFeedbackOnce && (timePassed >= projectileStartTime) && (timePassed < (projectileStartTime + 0.1f)))
		{
			RatScript_v2_0::UpdateEntityFacingdirection(mainID, m_direction);

			// shoots the projectile
			RatScript_v2_0::ToggleEntity(spikeballID, true);
			EntityManager::GetInstance().Get<RigidBody>(spikeballID).ApplyLinearImpulse(m_bulletImpulse);			
		}
		else if (timePassed >= attackDuration)
		{
			RatScript_v2_0::ToggleEntity(spikeballID, false);
			EntityManager::GetInstance().Get<RigidBody>(spikeballID).velocity.Zero();
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(this->mainID, EnumRatAnimations::IDLE);
			return true;
		}

		timePassed += deltaTime;
		return false;
	}


	void SniperRatAttack_v2_0::CreateAttackObjects()
	{
		spikeballID = ResourceManager::GetInstance().LoadPrefabFromFile("RatSpikeball.prefab");

		RatScript_v2_0::ToggleEntity(spikeballID, false); // deactivate spikeball
		RatScript_v2_0::PositionEntity(spikeballID, RatScript_v2_0::GetEntityPosition(mainID));
	}


	void SniperRatAttack_v2_0::DisableAttackObjects()
	{
		RatScript_v2_0::ToggleEntity(spikeballID, false); // deactivate spikeball
	}


	bool SniperRatAttack_v2_0::OnCollisionEnter(EntityID entity1, EntityID entity2)
	{
		// check if entity1 is rat or rat's attack and entity2 is cat
		if ((entity1 == this->mainID || entity1 == this->spikeballID) &&
			RatScript_v2_0::GetIsNonCagedCat(entity2))
		{
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(entity2, this->mainID);
			RatScript_v2_0::ToggleEntity(spikeballID, false);
			return true;
		}
		// check if entity2 is rat or rat's attack and entity1 is cat
		else if ((entity2 == this->mainID || entity2 == this->spikeballID) &&
			RatScript_v2_0::GetIsNonCagedCat(entity1))
		{
			// save the id of the cat that has been checked so that it wont be checked again
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(entity1, this->mainID);
			RatScript_v2_0::ToggleEntity(spikeballID, false);
			return true;
		}
		else if ((entity1 == this->spikeballID) && RatScript_v2_0::GetIsObstacle(entity2))
		{
				RatScript_v2_0::ToggleEntity(spikeballID, false);
		}
		else if ((entity2 == this->spikeballID) && RatScript_v2_0::GetIsObstacle(entity1))
		{
				RatScript_v2_0::ToggleEntity(spikeballID, false);
		}

		return false;
	}


	vec2 SniperRatAttack_v2_0::PickTargetPosition()
	{
		// Retrieve the rat data
		RatScript_v2_0_Data* p_data{ GETSCRIPTDATA(RatScript_v2_0, mainID) };				
		
		// Pick the closest cat to move 
		// ASSUMPTION: container of cats in detection radius has at least 1 element. 
		vec2 ratPosition{ RatScript_v2_0::GetEntityPosition(p_data->myID) };
		EntityID closestCat{ RatScript_v2_0::GetCloserTarget(ratPosition, p_data->catsInDetectionRadius) };


		// Set the shot target position
		shotTargetPosition = RatScript_v2_0::GetEntityPosition(closestCat);


		// Set the movement target position
		// Get the direction 75 deg from the direction that the 
		float anticlockwiseMultiplier{ ((std::rand() % 2) > 0) ? 1.f : -1.f};
		vec2 movementDirection{ shotTargetPosition - ratPosition };
		movementDirection = Rotate(movementDirection, anticlockwiseMultiplier * movementAngularOffset);
		movementDirection = movementDirection.GetNormalized();
		//vec2 movementDirection{ Rotate(shotTargetPosition - ratPosition, anticlockwiseMultiplier * 75.f).GetNormalized() };
		vec2 movementTarget{ movementDirection * p_data->maxMovementRange + ratPosition };

		// Set the position for the rat to move to
		GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, movementTarget, false);
		p_data->targetedCat = closestCat;


		return p_data->targetPosition;
	}
} // end of namespace PE
