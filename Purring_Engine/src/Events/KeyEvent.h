#ifndef KEY_EVENTS_H
#define KEY_EVENTS_H

#include "Event.h"
#include <sstream>

namespace temp {

	enum class KeyEvents
	{
		KeyPressed,
		KeyRelease
	};

	class KeyPressedEvent : public Event<KeyEvents>
	{
	public:
		KeyPressedEvent() : Event<KeyEvents>(KeyEvents::KeyPressed, "KeyPressed") {}
		virtual ~KeyPressedEvent() {}

		inline std::string ToString() const override
		{
			std::stringstream ss;
			if (keycode >= 32 && keycode <= 96)
			ss << "Key Pressed: " << (char)keycode;
			else
			{
				ss << "GLFW KeyCode of Key Pressed: " << keycode;
			}

			if(repeat)
			ss << " (Key is being held)";
				
			return ss.str();
		}

		int keycode = -1; // ascii keycode
		bool repeat = 0;
	};

	class KeyReleaseEvent : public Event<KeyEvents>
	{
	public:
		KeyReleaseEvent() : Event<KeyEvents>(KeyEvents::KeyRelease, "KeyReleaseE") {}
		virtual ~KeyReleaseEvent() {}

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

		int keycode = -1; // ascii keycode
	};










}









#endif