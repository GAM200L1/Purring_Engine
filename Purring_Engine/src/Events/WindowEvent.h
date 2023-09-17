/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     WindowEvent.h
 \date     8/30/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Header file containing the declaration and definition of the Window Events

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#ifndef WINDOW_EVENTS_H
#define WINDOW_EVENTS_H

#include "Event.h"
#include <sstream>
#include <utility>

namespace temp {

	//Window Event Types
	enum class WindowEvents
	{
		WindowResize,
		WindowClose,
		WindowFocus,
		WindowLostFocus,
		WindowMoved
	};

	class WindowResizeEvent : public Event<WindowEvents>
	{
		// ----- Constructors ----- // 
	public:
		WindowResizeEvent() : Event<WindowEvents>(WindowEvents::WindowResize, "WindowResize") {}
		virtual ~WindowResizeEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief					implicit conversion to a std::string, used for output streams

		 \return std::string	Returns information about the event
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << width << ", " << height;
			return ss.str();
		}

		// ----- Public variables ----- // 
	public:
		unsigned int width = 0, height = 0;
	};

	class WindowCloseEvent : public Event<WindowEvents>
	{
		// ----- Constructors ----- // 
	public:
		WindowCloseEvent() : Event<WindowEvents>(WindowEvents::WindowClose, "WindowClose") {}
		virtual ~WindowCloseEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief					implicit conversion to a std::string, used for output streams

		 \return std::string	Returns information about the event
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window Closed";
			return ss.str();
		}
	};

	class WindowFocusEvent : public Event<WindowEvents>
	{
		// ----- Constructors ----- // 
	public:
		WindowFocusEvent() : Event<WindowEvents>(WindowEvents::WindowFocus, "WindowFocus") {}
		virtual ~WindowFocusEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief					implicit conversion to a std::string, used for output streams

		 \return std::string	Returns information about the event
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window in focus";
			return ss.str();
		}

	};

	class WindowLostFocusEvent : public Event<WindowEvents>
	{
		// ----- Constructors ----- // 
	public:
		WindowLostFocusEvent() : Event<WindowEvents>(WindowEvents::WindowLostFocus, "WindowLostFocus") {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief					implicit conversion to a std::string, used for output streams

		 \return std::string	Returns information about the event
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window no longer in focus";
			return ss.str();
		}

		virtual ~WindowLostFocusEvent() {}
	};

	class WindowMovedEvent : public Event<WindowEvents>
	{
		// ----- Constructors ----- // 
	public:
		WindowMovedEvent() : Event<WindowEvents>(WindowEvents::WindowMoved, "WindowMoved") {}
		virtual ~WindowMovedEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief					implicit conversion to a std::string, used for output streams

		 \return std::string	Returns information about the event
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << xpos << ", " << ypos;
			return ss.str();
		}

		int xpos = 0, ypos = 0;
	};



};





#endif