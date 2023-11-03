/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CollisionEvent.h
 \date     29-10-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \brief    Header file containing the declaration and definition of the Collision Events

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Event.h"
#include <sstream>

namespace PE {
	using EntityID = unsigned long long;
	//collision Event Types
	enum class CollisionEvents
	{
		OnCollisionEnter,
		OnCollisionStay,
		OnCollisionExit,
		OnTriggerEnter,
		OnTriggerStay,
		OnTriggerExit
	};

	class OnCollisionEnterEvent : public Event<CollisionEvents>
	{
		// ----- Public variables ----- // 
	public:
		EntityID Entity1, Entity2;

		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		OnCollisionEnterEvent() : Event<CollisionEvents>(CollisionEvents::OnCollisionEnter, "OnCollisionEnter") {}

		/*!***********************************************************************************
		\brief			default destructor of events
		*************************************************************************************/
		virtual ~OnCollisionEnterEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
				ss << "Entity: " << Entity1 << " has collided with " << Entity2 << ".";
			return ss.str();
		}
	};

	class OnCollisionStayEvent : public Event<CollisionEvents>
	{
		// ----- Public variables ----- // 
	public:
		EntityID Entity1, Entity2;

		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		OnCollisionStayEvent() : Event<CollisionEvents>(CollisionEvents::OnCollisionStay, "OnCollisionStay") {}

		/*!***********************************************************************************
		\brief			default destructor of events
		*************************************************************************************/
		virtual ~OnCollisionStayEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Entity " << Entity1 << " is colliding with Entity " << Entity2 << ".";
			return ss.str();
		}
	};

	class OnCollisionExitEvent : public Event<CollisionEvents>
	{
		// ----- Public variables ----- // 
	public:
		EntityID Entity1, Entity2;

		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		OnCollisionExitEvent() : Event<CollisionEvents>(CollisionEvents::OnCollisionExit, "OnCollisionExit") {}

		/*!***********************************************************************************
		\brief			default destructor of events
		*************************************************************************************/
		virtual ~OnCollisionExitEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Entity " << Entity1 << " has stop colliding with Entity " << Entity2 << ".";
			return ss.str();
		}
	};

	class OnTriggerEnterEvent : public Event<CollisionEvents>
	{
		// ----- Public variables ----- // 
	public:
		EntityID Entity1, Entity2;

		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		OnTriggerEnterEvent() : Event<CollisionEvents>(CollisionEvents::OnTriggerEnter, "OnTriggerEnter") {}

		/*!***********************************************************************************
		\brief			default destructor of events
		*************************************************************************************/
		virtual ~OnTriggerEnterEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Entity " << Entity1 << " has started triggering Entity " << Entity2 << ".";
			return ss.str();
		}
	};

	class OnTriggerStayEvent : public Event<CollisionEvents>
	{
		// ----- Public variables ----- // 
	public:
		EntityID Entity1, Entity2;

		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		OnTriggerStayEvent() : Event<CollisionEvents>(CollisionEvents::OnTriggerStay, "OnTriggerStay") {}

		/*!***********************************************************************************
		\brief			default destructor of events
		*************************************************************************************/
		virtual ~OnTriggerStayEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Entity " << Entity1 << " is triggering Entity " << Entity2 << ".";
			return ss.str();
		}
	};

	class OnTriggerExitEvent : public Event<CollisionEvents>
	{
		// ----- Public variables ----- // 
	public:
		EntityID Entity1, Entity2;

		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		OnTriggerExitEvent() : Event<CollisionEvents>(CollisionEvents::OnTriggerExit, "OnTriggerExit") {}

		/*!***********************************************************************************
		\brief			default destructor of events
		*************************************************************************************/
		virtual ~OnTriggerExitEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Entity " << Entity1 << " has stopped triggering Entity " << Entity2 << ".";
			return ss.str();
		}
	};
}