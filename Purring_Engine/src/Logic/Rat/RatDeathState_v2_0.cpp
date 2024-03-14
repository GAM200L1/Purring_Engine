/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatDeathState_v2_0.cpp
 \date     06-03-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for the rat's death state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "RatDeathState_v2_0.h"

namespace PE
{
	void RatDeathState_v2_0::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
		p_gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		
		// Play the rat death animation
		GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(p_data->myID, EnumRatAnimations::DEATH);
		m_animDurationLeft = GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetAnimationDuration(p_data->myID);

		// Play the death audio
		GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayDeathAudio(p_data->myID);
	}


	void RatDeathState_v2_0::StateUpdate(EntityID id, float deltaTime)
	{
		if (p_gameStateController->currentState == GameStates_v2_0::PAUSE)
		{
				return;
		}

		// Disable all telegraphs
		GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DisableAllSpawnedEntities(id);

		// Check if the animation is done
		if (m_animDurationLeft <= 0.2f) // @TODO - To set the check to 0.f. Currently not zero bc the first frame of the animation will flash for a moment, sometimes
		{
			// Disable the rat
			RatScript_v2_0::ToggleEntity(p_data->myID, false);  // Making the rat entity inactive
		}
		else
		{
			m_animDurationLeft -= deltaTime;
		}
	}


	void RatDeathState_v2_0::StateExit(EntityID)
	{
		p_data = nullptr;
		p_gameStateController = nullptr;
	}

} // End of namespace PE