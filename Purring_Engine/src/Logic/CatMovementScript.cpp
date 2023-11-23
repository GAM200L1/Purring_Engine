/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatMovementScript.cpp
 \date     21-11-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains definitions for functions used for a grey cat's movement state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "CatMovementScript.h"

namespace PE
{
		// ----- Movement Plan Functions ----- //
		void CatMovementPLAN::StateEnter(EntityID id)
		{
			p_data = GETSCRIPTDATA(CatScript, id);
			// Create a start node where the player is 
		}

		void CatMovementPLAN::StateUpdate(EntityID id, float deltaTime)
		{ 
			id; deltaTime; 
			// IF  the path is already being drawn
				// IF  the mouse is being held down
				// Get  the position of the mouse from the last drawn node

			// ELSE  if the path isn't being drawn
				// IF  the mouse is held down and it's within the bounds of the player
						// if std::holds_alternative<AABB> == true then use different logic
						// get the player collider and call the PointCollision to check if the mouse is within the player
						


		}

		void CatMovementPLAN::StateExit(EntityID id)
		{ 
			id; 
			// teleport the player to the end of the path
			//p_data->pathQuads.back();
		}

		
		void CatMovementPLAN::ResetValues()
		{
				p_data->catEnergy = p_data->catMaxEnergy; // reset to max energy

				// Disable all the path nodes
				for (EntityID const& nodeId : p_data->pathQuads)
				{
						CatScript::ToggleEntity(nodeId, false);
				}
		}


		// ----- Movement Execution Functions ----- //
		void CatMovementEXECUTE::StateEnter(EntityID id)  
		{
		  p_data = GETSCRIPTDATA(CatScript, id);
			// Reset the position of the player to the start of the path
			// 
		}

		void CatMovementEXECUTE::StateUpdate(EntityID id, float deltaTime)  
		{ 
			id; deltaTime; 
			// Check if pause state

			// IF  the cat is within a certain threshold from the current node
				// IF  this is the last node, stop the movement of the cat
				// ELSE  change the node to move towards to the next node
						// Deactivate the next node

			// Check where the cat is in relation to the end of the last node
			// Start moving the cat

		}

		void CatMovementEXECUTE::StateExit(EntityID id)  
		{ 
			id; 
		}
}