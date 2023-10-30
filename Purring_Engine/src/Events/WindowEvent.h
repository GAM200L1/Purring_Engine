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


namespace PE
{
	// Enumeration representing different window event types
	enum class WindowEvents
	{
		WindowResize,
		WindowClose,
		WindowFocus,
		WindowLostFocus,
		WindowMoved
	};

	// Class for handling window resize events
	class WindowResizeEvent : public Event<WindowEvents>
	{

		// ----- Public variables ----- // 
	public:
		unsigned int width = 0, height = 0;
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		WindowResizeEvent() : Event<WindowEvents>(WindowEvents::WindowResize, "WindowResize") {}

		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~WindowResizeEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << width << ", " << height;
			return ss.str();
		}

	};

	class WindowCloseEvent : public Event<WindowEvents>
	{
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		WindowCloseEvent() : Event<WindowEvents>(WindowEvents::WindowClose, "WindowClose") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~WindowCloseEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
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
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		WindowFocusEvent() : Event<WindowEvents>(WindowEvents::WindowFocus, "WindowFocus") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~WindowFocusEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
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
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		WindowLostFocusEvent() : Event<WindowEvents>(WindowEvents::WindowLostFocus, "WindowLostFocus") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~WindowLostFocusEvent() {}
		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window no longer in focus";
			return ss.str();
		}
	};

	class WindowMovedEvent : public Event<WindowEvents>
	{
	public:
		int xpos = 0, ypos = 0;
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief			Constructor of event
		*************************************************************************************/
		WindowMovedEvent() : Event<WindowEvents>(WindowEvents::WindowMoved, "WindowMoved") {}
		/*!***********************************************************************************
		\brief			Destructor of event
		*************************************************************************************/
		virtual ~WindowMovedEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Prints out data from the event
		 \return    std::string - Returns the data of the event as a string
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << xpos << ", " << ypos;
			return ss.str();
		}

	};

};

