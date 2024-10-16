/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     MouseEvent.h
 \date     30-8-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Header file containing the declaration and definition of the Mouse Events

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Event.h"


namespace PE
{
	enum class MouseEvents
	{
		MouseMoved,
		MouseButtonPressed,
		MouseButtonHold,
		MouseButtonReleased,
		MouseScrolled
	};

	class MouseMovedEvent : public Event<MouseEvents>
	{
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		MouseMovedEvent() : Event<MouseEvents>(MouseEvents::MouseMoved, "MouseMoved") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~MouseMovedEvent() {}

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse Moved: " << x << ", " << y;
			return ss.str();
		}

		//may change to private and add getters
		int x = -1, y = -1; // new mouse position, window coordinates
	};

	class MouseButtonPressedEvent : public Event<MouseEvents>
	{
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		MouseButtonPressedEvent() : Event<MouseEvents>(MouseEvents::MouseButtonPressed, "MouseButtonPressed") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~MouseButtonPressedEvent() {}
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Button Pressed: " << button;
			return ss.str();
		}

		int button = -1; //lmb 1, rmb 2, mmb 3
		float repeat = 0;
		int x = -1, y = -1;
		int transX = -1, transY = -1;
	};

	class MouseButtonHoldEvent : public Event<MouseEvents>
	{
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		MouseButtonHoldEvent() : Event<MouseEvents>(MouseEvents::MouseButtonHold, "MouseButtonPressed") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~MouseButtonHoldEvent() {}
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Button Held: " << button;
			return ss.str();
		}

		int button = -1; //lmb 1, rmb 2, mmb 3
		int x = -1, y = -1;
		float repeat = 0;
	};

	class MouseButtonReleaseEvent : public Event<MouseEvents>
	{
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		MouseButtonReleaseEvent() : Event<MouseEvents>(MouseEvents::MouseButtonReleased, "MouseButtonPressed") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~MouseButtonReleaseEvent() {}
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Button: " << button << " Released: "  << "At(" << x << ", "<< y << ")";
			return ss.str();
		}

		int button = -1; //lmb 1, rmb 2, mmb 3
		int x = -1, y = -1;
	};

	class MouseScrolledEvent : public Event<MouseEvents>
	{
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		MouseScrolledEvent() : Event<MouseEvents>(MouseEvents::MouseScrolled, "MouseScrolled") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~MouseScrolledEvent() {}
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse Scrolled: " << xOffset << "," << yOffset;
			return ss.str();
		}

		float xOffset = 0, yOffset = 0;
	};

}

