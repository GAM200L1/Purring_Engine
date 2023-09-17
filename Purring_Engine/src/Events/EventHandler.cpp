/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EventHandler.cpp
 \creation date:       30-08-2023
 \last updated:        16-09-2023
 \author:              Jarran TAN Yan Zhi

 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief    Source file for the EventHandler class, which manages event dispatching within
		   the Purring Engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "EventHandler.h"

namespace temp
{
	// Initialize static unique_ptr instance to nullptr
	std::unique_ptr<EventHandler> EventHandler::s_Instance = nullptr;

	/*!***********************************************************************************
	 \brief      Gets the singleton instance of EventHandler, creating it if it doesn't exist.

	 \return     EventHandler* - Pointer to the singleton instance of EventHandler.
	*************************************************************************************/
	EventHandler* EventHandler::GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = std::make_unique<EventHandler>();
		}
		return s_Instance.get();
	}
}
