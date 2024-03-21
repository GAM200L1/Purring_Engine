/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     WindowManager.cpp
 \date     28-08-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief	   This file contains the implementation of the WindowManager class.
		   WindowManager handles the initialization, maintenance, and cleanup
		   of a GLFW window, along with the relevant callbacks and window operations.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "WindowManager.h"
#include "Logging/Logger.h"
#include "Input/InputSystem.h"
#include "Math/Transform.h"
#include "Physics/PhysicsManager.h"
#include "Graphics/RendererManager.h"
#include "GUISystem.h"
#include "GameStateManager.h"
#include "Input/InputSystem.h"
#include "PauseManager.h"
#ifndef GAMERELEASE
#include "Editor/Editor.h"
#include "SceneManager/SceneManager.h"
#endif
//logger instantiation
Logger event_logger = Logger("EVENT");

namespace PE
{
	// Initialize static variables
	bool WindowManager::msepress = false;
	bool WindowManager::m_fullScreen = true;

	WindowManager::WindowManager()
	{
#ifndef GAMERELEASE
		m_showFps = true;
#else
		m_showFps = false;
#endif // !GAMERELEASE
		// Attempt to initialize GLFW
		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW." << std::endl;
			exit(-1);
		}

	}


	WindowManager::~WindowManager()
	{
		Cleanup();
	}


	GLFWwindow* WindowManager::InitWindow(int width, int height, const char* p_title)
	{
#ifndef GAMERELEASE
		GLFWwindow* window = glfwCreateWindow(width, height, p_title, nullptr, nullptr);
		p_monitor = glfwGetWindowMonitor(window);
		WindowManager::GetInstance().SetWindow(window);
#else
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, p_title, glfwGetPrimaryMonitor(), nullptr);

		p_monitor = glfwGetWindowMonitor(window);
		WindowManager::GetInstance().SetWindow(window);
		SetWindowFullScreen(m_fullScreen);
#endif

		if (!window)
		{
			std::cerr << "Failed to create GLFW window." << std::endl;
			glfwTerminate();

			exit(-1);
		}

		glfwMakeContextCurrent(window);

		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

		glfwSetCursorPosCallback(window, InputSystem::mouse_callback);				// For mouse movement
		glfwSetMouseButtonCallback(window, InputSystem::check_mouse_buttons);		// For mouse button presses
		glfwSetScrollCallback(window, InputSystem::scroll_callback);				// For scroll wheel events
		glfwSetKeyCallback(window, InputSystem::key_callback);						// For keyboard events

		glfwSetWindowSizeCallback(window, window_resize_callback);					// For p_window resizing
		glfwSetWindowCloseCallback(window, window_close_callback);					// For p_window closing
		glfwSetWindowFocusCallback(window, window_focus_callback);					// For p_window focus events
		glfwSetWindowPosCallback(window, window_pos_callback);						// For p_window position changes
		glfwSetWindowIconifyCallback(window, window_iconify_callback);						// For p_window position changes

		ADD_ALL_WINDOW_EVENT_LISTENER(WindowManager::OnWindowEvent, this)
		ADD_ALL_MOUSE_EVENT_LISTENER(WindowManager::OnMouseEvent, this)
		ADD_ALL_KEY_EVENT_LISTENER(WindowManager::OnKeyEvent, this)

		REGISTER_UI_FUNCTION(TestFunction, WindowManager);
		REGISTER_UI_FUNCTION(CloseWindow, WindowManager);

		return window;
	}

	GLFWwindow* WindowManager::GetWindow()
	{
		return p_currWindow;
	}

	void WindowManager::SetWindow(GLFWwindow* p_win)
	{
		p_currWindow = p_win;
	}
	void WindowManager::CloseWindow(EntityID)
	{
		glfwSetWindowShouldClose(WindowManager::GetInstance().GetWindow(), true);
	}


	void WindowManager::TestFunction(EntityID)
	{
		std::cout << "hi im a test function" << std::endl;
	}


	void WindowManager::UpdateTitle(double fps)
	{
		std::ostringstream titleStream;
#ifndef GAMERELEASE
		std::string sceneName = SceneManager::GetInstance().GetActiveScene();
		titleStream << "Purring Engine | " << sceneName.substr(0, sceneName.find_last_of('.')) << " | FPS: " << static_cast<int>(fps);
#else
		titleStream << "March Of The Meows";
		
		if (m_showFps)
		{
			titleStream << " | FPS: " << static_cast<int>(fps);
		}
#endif
		glfwSetWindowTitle(WindowManager::GetInstance().GetWindow(), titleStream.str().c_str());
	}




	void WindowManager::Cleanup()
	{
		glfwTerminate();
	}

	void WindowManager::SetFullScreen(bool fullScreen)
	{
		m_fullScreen = fullScreen;
	}

	bool WindowManager::GetFullScreen()
	{
		return m_fullScreen;
	}

	void WindowManager::SetWindowFullScreen(bool fs)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(p_monitor);

		if (!fs)
		{
			glfwSetWindowMonitor(WindowManager::GetInstance().GetWindow(), NULL, 30, 30, 1920, 1080, 0);
			HWND windowHandle = GetActiveWindow();
			long Style = GetWindowLong(windowHandle, GWL_STYLE);
			Style &= ~WS_MAXIMIZEBOX; //this makes it still work when WS_MAXIMIZEBOX is actually already toggled off
			SetWindowLong(windowHandle, GWL_STYLE, Style);
			glfwSetWindowAttrib(WindowManager::GetInstance().GetWindow(), GLFW_RESIZABLE, false);
			m_fullScreen = false;
		}
		else
		{
			glfwSetWindowMonitor(WindowManager::GetInstance().GetWindow(), p_monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			m_fullScreen = true;
		}
	}


	/*                                                                                          Window Callback Functions
	--------------------------------------------------------------------------------------------------------------------- */

	void WindowManager::window_resize_callback(GLFWwindow* p_window, int width, int height)
	{
		p_window;
		PE::WindowResizeEvent WRE;
		WRE.width = width;
		WRE.height = height;

		PE::SEND_WINDOW_EVENT(WRE)
	}


	void WindowManager::window_close_callback(GLFWwindow* p_window)
	{
		p_window;
		PE::WindowCloseEvent WCE;
		PE::SEND_WINDOW_EVENT(WCE)
	}


	void WindowManager::window_focus_callback(GLFWwindow* p_window, int focus)
	{
		p_window;
		if (focus) {
			PE::WindowFocusEvent WFE;
			PE::SEND_WINDOW_EVENT(WFE)
		}
		else
		{
			PE::WindowLostFocusEvent WLFE;
			PE::SEND_WINDOW_EVENT(WLFE)
		}
	}


	void WindowManager::window_pos_callback(GLFWwindow* p_window, int xpos, int ypos)
	{
		p_window;
		PE::WindowMovedEvent WME;
		WME.xpos = xpos;
		WME.ypos = ypos;

		PE::SEND_WINDOW_EVENT(WME)
	}

	void WindowManager::window_iconify_callback(GLFWwindow* p_window, int iconified) {
		if (iconified == GLFW_TRUE) {
			// The window is being minimized
			// You can perform any actions you want when the window is minimized here

		}
		else {
			// The window is being restored
			// You can perform any actions you want when the window is restored here
#ifdef GAMERELEASE
			if(!msepress)
			glfwIconifyWindow(p_window);

			msepress = false;
#endif
		}
	}


	/*                                                                                          Event Handling Functions
	--------------------------------------------------------------------------------------------------------------------- */

	void WindowManager::OnWindowEvent(const PE::Event<PE::WindowEvents>& r_event)
	{
#ifndef GAMERELEASE
			//Editor::GetInstance().AddEventLog(r_event.ToString());
#else
			if (r_event.GetType() == WindowEvents::WindowLostFocus)
			{
					PauseManager::GetInstance().SetPaused(true);
					if (msepress)
							glfwIconifyWindow(WindowManager::GetInstance().GetWindow());

					msepress = true;
			}
			else if (r_event.GetType() == WindowEvents::WindowFocus)
			{
					PauseManager::GetInstance().SetPaused(false);
			}
#endif
	}


	void WindowManager::OnMouseEvent(const PE::Event<PE::MouseEvents>& r_event)
	{
#ifndef GAMERELEASE
			//Editor::GetInstance().AddEventLog(r_event.ToString());
#endif
	}


	void WindowManager::OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event)
	{
#ifndef GAMERELEASE
			//Editor::GetInstance().AddEventLog(r_event.ToString());
#endif

			//dynamic cast
			if (r_event.GetType() == KeyEvents::KeyTriggered)
			{
					KeyTriggeredEvent ev;
					ev = dynamic_cast<const KeyTriggeredEvent&>(r_event);
					//do step by step here
#ifndef GAMERELEASE
					if (PhysicsManager::GetStepPhysics())
					{

							if (ev.keycode == GLFW_KEY_N)
							{
									PhysicsManager::GetAdvanceStep() = true;
									Editor::GetInstance().AddEventLog("Advanced Step.\n");

							}

					}
#endif
#ifdef GAMERELEASE
					static bool fs{};
					//only on game release be able to change fullscreen
					if (ev.keycode == GLFW_KEY_F11)
					{
						SetWindowFullScreen(!m_fullScreen);
					}

					if (ev.keycode == GLFW_KEY_F4)
					{
						m_showFps = !m_showFps;
					}
#endif
			}
#ifdef GAMERELEASE
			else if (r_event.GetType() == KeyEvents::KeyRelease)
			{
					KeyReleaseEvent ev;
					ev = dynamic_cast<const KeyReleaseEvent&>(r_event);
					if (ev.keycode == GLFW_KEY_LEFT_ALT)
					{
							msepress = true;
					}
			}
			if (InputSystem::IsKeyTriggered(GLFW_KEY_LEFT_ALT))
					msepress = true;
#endif
	}
}