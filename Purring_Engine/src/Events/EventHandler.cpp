/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     EventHandler.cpp
 \date     8/30/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
Header file containing the declaration and definition of the event and event dispatcher template

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "EventHandler.h"

namespace temp
{
	std::unique_ptr<EventHandler> EventHandler::s_Instance = nullptr;

	EventHandler* EventHandler::GetInstance()
	{
		if (!s_Instance)
			s_Instance = std::make_unique<EventHandler>();

		return s_Instance.get();
	}
}