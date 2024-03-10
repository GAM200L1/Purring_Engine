/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     SniperRatAttack_v2_0.cpp
 \date     09-03-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains definitions for functions for the Sniper Rat's attacks.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "RatScript_v2_0.h"
#include "SniperRatAttack_v2_0.h"

#define DEBUG_PRINT

namespace PE
{
	void SniperRatAttack_v2_0::InitAttack()
	{
		m_bulletImpulse = vec2{ 0.f, 0.f };

		// calculate the impulse of the buller
		m_direction = shotTargetPosition - RatScript_v2_0::GetEntityPosition(mainID);
		m_bulletImpulse = m_direction.GetNormalized() * bulletForce;
		
		RatScript_v2_0::PositionEntity(spikeballID, RatScript_v2_0::GetEntityPosition(mainID) + m_direction.GetNormalized() * RatScript_v2_0::GetEntityScale(mainID).x * 0.5f);
		EntityManager::GetInstance().Get<RigidBody>(spikeballID).velocity.Zero();

		attackFeedbackOnce = false;
	}


	bool SniperRatAttack_v2_0::ExecuteAttack(float deltaTime)
	{
		if (!attackFeedbackOnce)
		{
			attackFeedbackOnce = true;
			RatScript_v2_0::PlayAttackAudio();
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(mainID, EnumRatAnimations::ATTACK);
			attackDuration = GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetAnimationDuration(mainID);
		}
		else if (attackFeedbackOnce && EntityManager::GetInstance().Get<AnimationComponent>(mainID).GetCurrentFrameIndex() == 3)
		{
			// make the rat face the direction it is shooting
			vec2 newScale{ RatScript_v2_0::GetEntityScale(mainID) };
			newScale.x = std::abs(newScale.x) * ((m_direction.Dot(vec2{ 1.f, 0.f }) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the rat is facing left
			RatScript_v2_0::ScaleEntity(mainID, newScale.x, newScale.y);

			// shoots the projectile
			RatScript_v2_0::ToggleEntity(spikeballID, true);
			EntityManager::GetInstance().Get<RigidBody>(spikeballID).ApplyLinearImpulse(m_bulletImpulse);
			
		}
		else if (attackDuration <= 0.f)
		{
			RatScript_v2_0::ToggleEntity(spikeballID, false);
			EntityManager::GetInstance().Get<RigidBody>(spikeballID).velocity.Zero();
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(this->mainID, EnumRatAnimations::IDLE);
			return true;
		}

		attackDuration -= deltaTime;
		return false;
	}


	void SniperRatAttack_v2_0::CreateAttackObjects()
	{
		spikeballID = SerializationManager{}.LoadFromFile("RatSpikeball.prefab");

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
		return false;
	}


	vec2 SniperRatAttack_v2_0::PickTargetPosition()
	{
		// @TODO - Krystal
		return RatScript_v2_0::GetEntityPosition(mainID);
	}
} // end of namespace PE
