#ifndef WINDOW_EVENTS_H
#define WINDOW_EVENTS_H

#include "Event.h"
#include <sstream>
#include <utility>

namespace temp {

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
	public:
		WindowResizeEvent() : Event<WindowEvents>(WindowEvents::WindowResize, "WindowResize") {}
		virtual ~WindowResizeEvent() {}
		//inline unsigned int GetWidth() const { return m_Width; }
		//inline unsigned int GetHeight() const { return m_Height; }

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		unsigned int m_Width = 0, m_Height = 0;
	};

	class WindowCloseEvent : public Event<WindowEvents>
	{
	public:
		WindowCloseEvent() : Event<WindowEvents>(WindowEvents::WindowClose, "WindowClose") {}

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window Closed";
			return ss.str();
		}

		virtual ~WindowCloseEvent() {}
	};

	class WindowFocusEvent : public Event<WindowEvents>
	{
	public:
		WindowFocusEvent() : Event<WindowEvents>(WindowEvents::WindowFocus, "WindowFocus") {}

		inline std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window in focus";
			return ss.str();
		}

		virtual ~WindowFocusEvent() {}
	};

	class WindowLostFocusEvent : public Event<WindowEvents>
	{
	public:
		WindowLostFocusEvent() : Event<WindowEvents>(WindowEvents::WindowLostFocus, "WindowLostFocus") {}

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
	public:
		WindowMovedEvent() : Event<WindowEvents>(WindowEvents::WindowMoved, "WindowMoved") {}
		virtual ~WindowMovedEvent() {}

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