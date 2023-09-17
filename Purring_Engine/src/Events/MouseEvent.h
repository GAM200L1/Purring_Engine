/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MouseEvent.h
 \creation date:       30-08-2023
 \last updated:        16-09-2023
 \author:              Jarran TAN Yan Zhi

 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief    Header file for the EventHandler class, which manages event dispatching within
		   the Purring Engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#ifndef MOUSE_EVENTS_H
#define MOUSE_EVENTS_H

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Event.h"
#include <sstream>
#include <utility>

namespace temp
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
		MouseMovedEvent() : Event<MouseEvents>(MouseEvents::MouseMoved, "MouseMoved") {}
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
		MouseButtonPressedEvent() : Event<MouseEvents>(MouseEvents::MouseButtonPressed, "MouseButtonPressed") {}
		virtual ~MouseButtonPressedEvent() {}

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Button Pressed: " << button;
			return ss.str();
		}

		int button = -1; //lmb 1, mmb 2, rmb 3
	};

	class MouseButtonReleaseEvent : public Event<MouseEvents>
	{
	public:
		MouseButtonReleaseEvent() : Event<MouseEvents>(MouseEvents::MouseButtonReleased, "MouseButtonPressed") {}
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
		MouseScrolledEvent() : Event<MouseEvents>(MouseEvents::MouseScrolled, "MouseScrolled") {}
		virtual ~MouseScrolledEvent() {}

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Button Scrolled: " << xOffset << "," << yOffset;
			return ss.str();
		}

		int xOffset = 0, yOffset = 0;
	};

}

#endif
