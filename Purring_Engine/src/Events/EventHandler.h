/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EventHandler.h
 \creation date:       30-08-2023
 \last updated:        16-09-2023
 \author:              Jarran TAN Yan Zhi

 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief    Header file for the EventHandler class, which manages event dispatching within 
           the Purring Engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "WindowEvent.h"
#include <memory>
#include "Singleton.h"

namespace PE
{
	class EventHandler : public Singleton <EventHandler>
	{
		// ----- Public variables ----- // 
	public:
		EventDispatcher<WindowEvents> WindowEventDispatcher;

		EventDispatcher<MouseEvents> MouseEventDispatcher;

		EventDispatcher<KeyEvents> KeyEventDispatcher;
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief     default constructor for event handler
		*************************************************************************************/
		EventHandler() : WindowEventDispatcher(), MouseEventDispatcher(), KeyEventDispatcher() {}

	};

#define ADD_WINDOW_EVENT_LISTENER(eventType,func,arg) PE::EventHandler::GetInstance().WindowEventDispatcher.AddListener(eventType, std::bind(&func,arg,std::placeholders::_1));
#define ADD_MOUSE_EVENT_LISTENER(eventType,func,arg) PE::EventHandler::GetInstance().MouseEventDispatcher.AddListener(eventType, std::bind(&func,arg,std::placeholders::_1));
#define ADD_KEY_EVENT_LISTENER(eventType,func,arg) PE::EventHandler::GetInstance().KeyEventDispatcher.AddListener(eventType, std::bind(&func,arg,std::placeholders::_1));

#define ADD_ALL_WINDOW_EVENT_LISTENER(func,arg) PE::EventHandler::GetInstance().WindowEventDispatcher.AddListener(PE::WindowEvents::WindowClose, std::bind(&func,arg,std::placeholders::_1));\
											    PE::EventHandler::GetInstance().WindowEventDispatcher.AddListener(PE::WindowEvents::WindowFocus, std::bind(&func,arg,std::placeholders::_1));\
												PE::EventHandler::GetInstance().WindowEventDispatcher.AddListener(PE::WindowEvents::WindowLostFocus, std::bind(&func,arg,std::placeholders::_1));\
												PE::EventHandler::GetInstance().WindowEventDispatcher.AddListener(PE::WindowEvents::WindowMoved, std::bind(&func,arg,std::placeholders::_1));\
												PE::EventHandler::GetInstance().WindowEventDispatcher.AddListener(PE::WindowEvents::WindowResize, std::bind(&func,arg,std::placeholders::_1));

#define ADD_ALL_MOUSE_EVENT_LISTENER(func,arg) PE::EventHandler::GetInstance().MouseEventDispatcher.AddListener(PE::MouseEvents::MouseMoved, std::bind(&func,arg,std::placeholders::_1));\
											    PE::EventHandler::GetInstance().MouseEventDispatcher.AddListener(PE::MouseEvents::MouseButtonPressed, std::bind(&func,arg,std::placeholders::_1));\
												PE::EventHandler::GetInstance().MouseEventDispatcher.AddListener(PE::MouseEvents::MouseButtonReleased, std::bind(&func,arg,std::placeholders::_1));\
												PE::EventHandler::GetInstance().MouseEventDispatcher.AddListener(PE::MouseEvents::MouseScrolled, std::bind(&func,arg,std::placeholders::_1));\
												PE::EventHandler::GetInstance().MouseEventDispatcher.AddListener(PE::MouseEvents::MouseButtonHold, std::bind(&func,arg,std::placeholders::_1));								

#define ADD_ALL_KEY_EVENT_LISTENER(func,arg) PE::EventHandler::GetInstance().KeyEventDispatcher.AddListener(PE::KeyEvents::KeyTriggered, std::bind(&func,arg,std::placeholders::_1));\
												PE::EventHandler::GetInstance().KeyEventDispatcher.AddListener(PE::KeyEvents::KeyRelease, std::bind(&func,arg,std::placeholders::_1));\
												PE::EventHandler::GetInstance().KeyEventDispatcher.AddListener(PE::KeyEvents::KeyPressed, std::bind(&func,arg,std::placeholders::_1));

#define REMOVE_WINDOW_EVENT_LISTENER(handle) PE::EventHandler::GetInstance().WindowEventDispatcher.RemoveListener(handle);
#define REMOVE_MOUSE_EVENT_LISTENER(handle) PE::EventHandler::GetInstance().MouseEventDispatcher.RemoveListener(handle);
#define REMOVE_KEY_EVENT_LISTENER(handle) PE::EventHandler::GetInstance().KeyEventDispatcher.RemoveListener(handle);



#define SEND_WINDOW_EVENT(_event) EventHandler::GetInstance().WindowEventDispatcher.SendEvent(_event);
#define SEND_MOUSE_EVENT(_event) EventHandler::GetInstance().MouseEventDispatcher.SendEvent(_event);
#define SEND_KEY_EVENT(_event) EventHandler::GetInstance().KeyEventDispatcher.SendEvent(_event);
}
