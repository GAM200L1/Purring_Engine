/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     System.h
 \creation date:       30-08-2023
 \last updated:        16-09-2023
 \author:              Brandon HO Jun Jie

 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    Defines an interface for systems in the Purring Engine.
		   Each system should implement initialization, update, and destruction logic.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include <string>

namespace PE
{
	class System
	{
	public:
		/*!***********************************************************************************
		 \brief     Virtual destructor for proper cleanup of derived systems.
		*************************************************************************************/
		virtual ~System() {}

		/*!***********************************************************************************
		 \brief     Initialize the system. Should be called once before any updates.
		*************************************************************************************/
		virtual void InitializeSystem() {}

		/*!***********************************************************************************
		 \brief     Update the system each frame.
		 \param     deltaTime Time passed since the last frame, in seconds.
		*************************************************************************************/
		virtual void UpdateSystem(float deltaTime) = 0;

		/*!***********************************************************************************
		 \brief     Clean up the system resources. Should be called once after all updates.
		*************************************************************************************/
		virtual void DestroySystem() = 0;

		/*!***********************************************************************************
		 \brief     Get the system's name, useful for debugging and identification.
		 \return    std::string The name of the system.
		*************************************************************************************/
		virtual std::string GetName() = 0;
	};
}
