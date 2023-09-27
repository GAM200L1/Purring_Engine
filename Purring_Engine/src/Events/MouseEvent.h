/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     MouseEvent.h
 \date     8/30/2023

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
#include <sstream>
#include <utility>

namespace PE
{
	enum class MouseEvents
	{
		MouseMoved,
		MouseButtonPressed,
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

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Button Pressed: " << button;
			return ss.str();
			//if (repeat < 0)
			//	ss << " (Button Being Held)";

		}

		int button = -1; //lmb 1, mmb 2, rmb 3
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

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Button Released: " << button;
			return ss.str();
		}

		int button = -1; //lmb 1, mmb 2, rmb 3
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

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse Scrolled: " << xOffset << "," << yOffset;
			return ss.str();
		}

		float xOffset = 0, yOffset = 0;
	};

}

