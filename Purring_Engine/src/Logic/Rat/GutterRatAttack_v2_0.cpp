/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     GutterRatAttack_v2_0.cpp
 \date     09-03-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains definitions for functions for the Gutter Rat's attacks.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "RatScript_v2_0.h"
#include "GutterRatAttack_v2_0.h"
#include "ResourceManager/ResourceManager.h"

//#define DEBUG_PRINT

namespace PE
{
	void GutterRatAttack_v2_0::InitAttack()
	{
		// Position the attack telegraph where the rat is
		RatScript_v2_0::PositionEntity(this->attackTelegraphEntityID, RatScript_v2_0::GetEntityPosition(this->mainID));

		// Reset the attack feedback bool
		attackFeedbackOnce = false;
	}


	bool GutterRatAttack_v2_0::ExecuteAttack(float deltaTime)
	{
		RatScript_v2_0::PositionEntity(this->attackTelegraphEntityID, RatScript_v2_0::GetEntityPosition(this->mainID));
		RatScript_v2_0::ToggleEntity(this->attackTelegraphEntityID, true);

		if (!attackFeedbackOnce)
		{
			attackFeedbackOnce = true;
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAttackAudio(this->mainID);
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(this->mainID, EnumRatAnimations::ATTACK);
			attackDuration = GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetAnimationDuration(this->mainID);
		}
		else if (attackDuration <= 0.f)
		{
#ifdef DEBUG_PRINT
			std::cout << "GutterRatAttack_v2_0::ExecuteAttack(" << this->mainID << "): animation is done" << std::endl;
#endif // DEBUG_PRINT
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(this->mainID, EnumRatAnimations::IDLE);
			return true;
		}

		attackDuration -= deltaTime;
		return false;
	}


	void GutterRatAttack_v2_0::CreateAttackObjects()
	{
		// create cross attack telegraph
		this->attackTelegraphEntityID = ResourceManager::GetInstance().LoadPrefabFromFile("GutterRatAttackTelegraph.prefab");

		// Hierarchy::GetInstance().AttachChild(this->mainID, this->attackTelegraphEntityID);
		//  The attack needs to render under the rat, and childing the attack to the rat
		//  will cause it to adopt the render layer of the rat

		RatScript_v2_0::ToggleEntity(this->attackTelegraphEntityID, false); // set to inactive, it will only show during planning phase if the cat is in the area
		RatScript_v2_0::ScaleEntity(this->attackTelegraphEntityID, this->attackRadius, this->attackRadius);
		RatScript_v2_0::PositionEntity(this->attackTelegraphEntityID, vec2{0.f, 0.f});
		RatScript_v2_0::PositionEntityRelative(this->attackTelegraphEntityID, vec2{0.f, 0.f});
	}


	void GutterRatAttack_v2_0::DisableAttackObjects()
	{
		RatScript_v2_0::ToggleEntity(this->attackTelegraphEntityID, false);
	}


	bool GutterRatAttack_v2_0::OnCollisionEnter(EntityID entity1, EntityID entity2)
	{
#ifdef DEBUG_PRINT
			std::cout << "GutterRatAttack_v2_0::OnCollisionEnter(" << this->mainID << "): between ent " << entity1 << " and " << entity2 << std::endl;
#endif // DEBUG_PRINT

		// check if entity1 is rat or rat's attack and entity2 is cat
		if ((entity1 == this->mainID || entity1 == this->attackTelegraphEntityID) &&
			RatScript_v2_0::GetIsNonCagedCat(entity2))
		{
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(entity2, this->mainID);
			return true;
		}
		// check if entity2 is rat or rat's attack and entity1 is cat
		else if ((entity2 == this->mainID || entity2 == this->attackTelegraphEntityID) &&
				 RatScript_v2_0::GetIsNonCagedCat(entity1))
		{
			// save the id of the cat that has been checked so that it wont be checked again
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(entity1, this->mainID);
			return true;
		}

		return false;
	}


	vec2 GutterRatAttack_v2_0::PickTargetPosition()
	{
			// Retrieve the rat's data
			RatScript_v2_0_Data* p_data{GETSCRIPTDATA(RatScript_v2_0, mainID)};

			// Pick the closest cat to move 
			// ASSUMPTION: container of cats in detection radius has at least 1 element. 
			EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsInDetectionRadius) };

			// Set the position for the rat to move to
			GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, closestCat, false);
			return p_data->targetPosition;
	}
} // end of namespace PE
