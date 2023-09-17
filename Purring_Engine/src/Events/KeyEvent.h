/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     KeyEvent.h
 \date     8/30/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
Header file containing the declaration and definition of the event and event dispatcher template

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#ifndef KEY_EVENTS_H
#define KEY_EVENTS_H

#include "Event.h"
#include <sstream>

namespace temp {

	//Key Event Types
	enum class KeyEvents
	{
		KeyPressed,
		KeyRelease
	};


	class KeyPressedEvent : public Event<KeyEvents>
	{
		// ----- Constructors ----- // 
	public:
		KeyPressedEvent() : Event<KeyEvents>(KeyEvents::KeyPressed, "KeyPressed") {}
		virtual ~KeyPressedEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief					implicit conversion to a std::string, used for output streams

		 \return std::string	Returns information about the event
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			if (keycode >= 32 && keycode <= 96)
				ss << "Key Pressed: " << (char)keycode;
			else
			{
				ss << "GLFW KeyCode of Key Pressed: " << keycode;
			}

			if (repeat)
				ss << " (Key is being held)";

			return ss.str();
		}

		// ----- Public variables ----- // 
	public:
		int keycode = -1; // ascii keycode
		bool repeat = 0; //is the key on repeat
	};

	class KeyReleaseEvent : public Event<KeyEvents>
	{
		// ----- Constructors ----- // 
	public:
		KeyReleaseEvent() : Event<KeyEvents>(KeyEvents::KeyRelease, "KeyReleaseE") {}
		virtual ~KeyReleaseEvent() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief					implicit conversion to a std::string, used for output streams

		 \return std::string	Returns information about the event
		*************************************************************************************/
		inline std::string ToString() const override
		{
			std::stringstream ss;
			if (keycode >= 32 && keycode <= 96)
				ss << "Key Released: " << (char)keycode;
			else
			{
				ss << "GLFW KeyCode of Key Released: " << keycode;
			}
			return ss.str();
		}

		// ----- Public variables ----- // 
	public:
		int keycode = -1; // ascii keycode
	};










}









#endif