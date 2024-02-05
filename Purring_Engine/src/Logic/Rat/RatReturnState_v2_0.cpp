/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatReturnState_v2_0.cpp
 \date     24-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions for a temp rat state for testing purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatReturnState_v2_0.h"

namespace PE
{
	void RatReturn_v2_0::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(RatScript_v2_0, id);
	}


	void RatReturn_v2_0::StateUpdate(EntityID id, float deltaTime)
	{
		// Can conduct state checks and call for state changes here
	}


	void RatReturn_v2_0::StateExit(EntityID)
	{

	}

} // End of namespace PE