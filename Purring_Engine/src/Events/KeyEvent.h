/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     KeyEvent.h
 \creation date:       30-08-2023
 \last updated:        16-09-2023
 \author:              Jarran TAN Yan Zhi

 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Header file containing the declaration and definition of the Key Events

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Event.h"
#include <sstream>

namespace PE
{

	//Key Event Types
	enum class KeyEvents
	{
		KeyTriggered,
		KeyPressed,
		KeyRelease
	};


	class KeyTriggeredEvent : public Event<KeyEvents>
	{
		// ----- Constructors ----- // 
	public:
		KeyTriggeredEvent() : Event<KeyEvents>(KeyEvents::KeyTriggered, "KeyTriggered") {}
		virtual ~KeyTriggeredEvent() {}

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
				ss << "Key Triggered: " << (char)keycode;
			else
			{
				ss << "GLFW KeyCode of Key Triggered: " << keycode;
			}
			return ss.str();
		}

		// ----- Public variables ----- // 
	public:
		int keycode = -1; // ascii keycode
		float repeat = 0; //is the key on repeat
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

			return ss.str();
		}

		// ----- Public variables ----- // 
	public:
		int keycode = -1; // ascii keycode
		float repeat = 0; //is the key on repeat
	};

	class KeyReleaseEvent : public Event<KeyEvents>
	{
		// ----- Constructors ----- // 
	public:
		KeyReleaseEvent() : Event<KeyEvents>(KeyEvents::KeyRelease, "KeyRelease") {}
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

