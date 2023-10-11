/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     WindowEvent.h
 \creation date:       30-08-2023
 \last updated:        16-09-2023
 \author:              Jarran TAN Yan Zhi

 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Header file containing the declaration and definition of the Window Events

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Event.h"
#include <sstream>
#include <utility>
#include "ECS/EntityFactory.h"
namespace PE
{
	// Enumeration representing different window event types
	enum class CollisionEvents
	{
		OnCollisionEvent,
		OnTriggerEnterEvent,
		OnTriggerExitEvent,
		OnTriggerStayEvent
	};

	// Class for handling window resize events
	class OnCollisionEvent : public Event<CollisionEvents>
	{

		// ----- Public variables ----- // 
	public:
		EntityID Entity1{ -1 }, Entity2{ -1 };
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		OnCollisionEvent() : Event<CollisionEvents>(CollisionEvents::OnCollisionEvent, "OnCollision") {}

		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~OnCollisionEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "OnCollisionEvent: " << Entity1 << " with " << Entity2;
			return ss.str();
		}

	};

};

