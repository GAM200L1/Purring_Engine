#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "MouseEvent.h"
#include "KeyEvent.h"
#include "WindowEvent.h"
#include <memory>

namespace temp {
	class EventHandler
	{
	public:
		EventHandler() : WindowEventDispatcher(), MouseEventDispatcher(), KeyEventDispatcher() {}
		static EventHandler* GetInstance();

		//WindowEvents
		EventDispatcher<WindowEvents> WindowEventDispatcher;

		EventDispatcher<MouseEvents> MouseEventDispatcher;

		EventDispatcher<KeyEvents> KeyEventDispatcher;

	private:
		static std::unique_ptr<EventHandler> s_Instance;
	};

#define ADD_WINDOW_EVENT_LISTENER(eventType,func,arg) temp::EventHandler::GetInstance()->WindowEventDispatcher.AddListener(eventType, std::bind(&func,arg,std::placeholders::_1));
#define ADD_MOUSE_EVENT_LISTENER(eventType,func,arg) temp::EventHandler::GetInstance()->MouseEventDispatcher.AddListener(eventType, std::bind(&func,arg,std::placeholders::_1));
#define ADD_KEY_EVENT_LISTENER(eventType,func,arg) temp::EventHandler::GetInstance()->KeyEventDispatcher.AddListener(eventType, std::bind(&func,arg,std::placeholders::_1));

#define ADD_ALL_WINDOW_EVENT_LISTENER(func,arg) temp::EventHandler::GetInstance()->WindowEventDispatcher.AddListener(temp::WindowEvents::WindowClose, std::bind(&func,arg,std::placeholders::_1));\
											    temp::EventHandler::GetInstance()->WindowEventDispatcher.AddListener(temp::WindowEvents::WindowFocus, std::bind(&func,arg,std::placeholders::_1));\
												temp::EventHandler::GetInstance()->WindowEventDispatcher.AddListener(temp::WindowEvents::WindowLostFocus, std::bind(&func,arg,std::placeholders::_1));\
												temp::EventHandler::GetInstance()->WindowEventDispatcher.AddListener(temp::WindowEvents::WindowMoved, std::bind(&func,arg,std::placeholders::_1));\
												temp::EventHandler::GetInstance()->WindowEventDispatcher.AddListener(temp::WindowEvents::WindowResize, std::bind(&func,arg,std::placeholders::_1));

#define ADD_ALL_MOUSE_EVENT_LISTENER(func,arg) temp::EventHandler::GetInstance()->MouseEventDispatcher.AddListener(temp::MouseEvents::MouseMoved, std::bind(&func,arg,std::placeholders::_1));\
											    temp::EventHandler::GetInstance()->MouseEventDispatcher.AddListener(temp::MouseEvents::MouseButtonPressed, std::bind(&func,arg,std::placeholders::_1));\
												temp::EventHandler::GetInstance()->MouseEventDispatcher.AddListener(temp::MouseEvents::MouseButtonReleased, std::bind(&func,arg,std::placeholders::_1));\
												temp::EventHandler::GetInstance()->MouseEventDispatcher.AddListener(temp::MouseEvents::MouseScrolled, std::bind(&func,arg,std::placeholders::_1));

#define ADD_ALL_KEY_EVENT_LISTENER(func,arg) temp::EventHandler::GetInstance()->KeyEventDispatcher.AddListener(temp::KeyEvents::KeyPressed, std::bind(&func,arg,std::placeholders::_1));\
												temp::EventHandler::GetInstance()->KeyEventDispatcher.AddListener(temp::KeyEvents::KeyRelease, std::bind(&func,arg,std::placeholders::_1));




	//if engine but we not doing this yet
#define SEND_WINDOW_EVENT(_event) EventHandler::GetInstance()->WindowEventDispatcher.SendEvent(_event);
#define SEND_MOUSE_EVENT(_event) EventHandler::GetInstance()->MouseEventDispatcher.SendEvent(_event);
#define SEND_KEY_EVENT(_event) EventHandler::GetInstance()->KeyEventDispatcher.SendEvent(_event);
}



#endif