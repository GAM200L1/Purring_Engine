/*!*****************************************************************************
	@file       WindowManager.cpp
	@author     Hans (You Yang) ONG
	@co-author
	@par        DP email: youyang.o\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       130823

	@brief      This file contains the implementation of the WindowManager class.
				WindowManager handles the initialization, maintenance, and cleanup
				of a GLFW window, along with the relevant callbacks and window operations.

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "WindowManager.h"
#include <iostream>
#include <sstream>
#include "Logging/Logger.h"
#include "imgui/ImGuiWindow.h"
//logger instantiation
Logger event_logger = Logger("EVENT");

namespace PE
{
	/*                                                                                                          class member implementations
	--------------------------------------------------------------------------------------------------------------------- */
	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Default constructor for the WindowManager class. Initializes GLFW.
	/// If GLFW initialization fails, prints an error message and exits the program.
	/// </summary>
	----------------------------------------------------------------------------- */
	WindowManager::WindowManager()
	{
		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW." << std::endl;
			exit(-1);
		}
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Destructor for the WindowManager class. Cleans up the resources.
	/// </summary>
	----------------------------------------------------------------------------- */
	WindowManager::~WindowManager()
	{
		Cleanup();
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Initializes a GLFW window with the given width, height, and title.
	/// If window creation fails, it prints an error message, terminates GLFW and exits the program.
	/// </summary>
	/// <param name="width">Width of the window in pixels.</param>
	/// <param name="height">Height of the window in pixels.</param>
	/// <param name="title">Title to be displayed on the window bar.</param>
	/// <returns>A pointer to the created GLFWwindow. Returns nullptr if the window creation fails.</returns>
	----------------------------------------------------------------------------- */
	GLFWwindow* WindowManager::InitWindow(int width, int height, const char* title)
	{
		GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!window)
		{
			std::cerr << "Failed to create GLFW window." << std::endl;
			glfwTerminate();
			exit(-1);
		}
		glfwMakeContextCurrent(window);

		// Set glfw callbacks from input
		glfwSetCursorPosCallback(window, mouse_callback);        // For mouse movement
		glfwSetMouseButtonCallback(window, check_mouse_buttons); // For mouse button presses
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetKeyCallback(window, key_callback);

		//required to set window user pointer to access the callback methods in a class
		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));
		glfwSetWindowSizeCallback(window, window_resize_callback);
		glfwSetWindowCloseCallback(window, window_close_callback);
		glfwSetWindowFocusCallback(window, window_focus_callback);
		glfwSetWindowPosCallback(window, window_pos_callback);

		//subscription to events
		//temp::ADD_WINDOW_EVENT_LISTENER(temp::WindowEvents::WindowResize, WindowManager::OnWindowEvent, this)
		//temp::ADD_WINDOW_EVENT_LISTENER(temp::WindowEvents::WindowClose, WindowManager::OnWindowEvent, this)
		//temp::ADD_WINDOW_EVENT_LISTENER(temp::WindowEvents::WindowFocus, WindowManager::OnWindowEvent, this)
		//temp::ADD_WINDOW_EVENT_LISTENER(temp::WindowEvents::WindowLostFocus, WindowManager::OnWindowEvent, this)
		//temp::ADD_WINDOW_EVENT_LISTENER(temp::WindowEvents::WindowMoved, WindowManager::OnWindowEvent, this)
		ADD_ALL_WINDOW_EVENT_LISTENER(WindowManager::OnWindowEvent, this)

			//ADD_MOUSE_EVENT_LISTENER(temp::MouseEvents::MouseMoved, WindowManager::OnMouseEvent, this)
			//ADD_MOUSE_EVENT_LISTENER(temp::MouseEvents::MouseButtonPressed, WindowManager::OnMouseEvent, this)
			//ADD_MOUSE_EVENT_LISTENER(temp::MouseEvents::MouseButtonRelease, WindowManager::OnMouseEvent, this)
			//ADD_MOUSE_EVENT_LISTENER(temp::MouseEvents::MouseScrolled, WindowManager::OnMouseEvent, this)
			ADD_ALL_MOUSE_EVENT_LISTENER(WindowManager::OnMouseEvent, this)

			//ADD_KEY_EVENT_LISTENER(temp::KeyEvents::KeyPressed, WindowManager::OnKeyEvent, this)
			//ADD_KEY_EVENT_LISTENER(temp::KeyEvents::KeyRelease, WindowManager::OnKeyEvent, this)
			ADD_ALL_KEY_EVENT_LISTENER(WindowManager::OnKeyEvent, this)

			return window;
	}

	void WindowManager::OnWindowEvent(const temp::Event<temp::WindowEvents>& e)
	{
		ImGuiWindow::GetInstance()->addLog(e.ToString());
		switch (e.GetType())
		{
		case temp::WindowEvents::WindowResize:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		case temp::WindowEvents::WindowClose:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		case temp::WindowEvents::WindowFocus:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		case temp::WindowEvents::WindowLostFocus:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		case temp::WindowEvents::WindowMoved:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		default:
			break;
		}

		event_logger.FlushLog();
	}

	void WindowManager::OnMouseEvent(const temp::Event<temp::MouseEvents>& e)
	{
		ImGuiWindow::GetInstance()->addLog(e.ToString());
		switch (e.GetType())
		{
		case temp::MouseEvents::MouseMoved:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		case temp::MouseEvents::MouseButtonPressed:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		case temp::MouseEvents::MouseButtonReleased:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		case temp::MouseEvents::MouseScrolled:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		default:
			break;
		}
	}

	void WindowManager::OnKeyEvent(const temp::Event<temp::KeyEvents>& e)
	{
		ImGuiWindow::GetInstance()->addLog(e.ToString());
		switch (e.GetType())
		{
		case temp::KeyEvents::KeyPressed:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		case temp::KeyEvents::KeyRelease:
			event_logger.AddLog(false, e.ToString(), __FUNCTION__);
			break;
		default:
			break;
		}
	}

	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Updates the window's title to display the current FPS.
	/// </summary>
	/// <param name="window">A pointer to the GLFWwindow whose title needs to be updated.</param>
	/// <param name="fps">Current frames per second value.</param>
	----------------------------------------------------------------------------- */
	void WindowManager::UpdateTitle(GLFWwindow* window, double fps)
	{
		std::ostringstream titleStream;
		titleStream << "GAM200 TempGameName | FPS: " << static_cast<int>(fps);
		glfwSetWindowTitle(window, titleStream.str().c_str());
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Cleans up the resources by terminating GLFW.
	/// </summary>
	/// ----------------------------------------------------------------------------- */
	void WindowManager::Cleanup()
	{
		glfwTerminate();
	}


	void WindowManager::window_resize_callback(GLFWwindow* window, int width, int height)
	{
		window;
		temp::WindowResizeEvent WRE;
		WRE.width = width;
		WRE.height = height;

		temp::SEND_WINDOW_EVENT(WRE)
	}

	void WindowManager::window_close_callback(GLFWwindow* window)
	{
		window;
		temp::WindowCloseEvent WCE;
		temp::SEND_WINDOW_EVENT(WCE)
	}

	void WindowManager::window_focus_callback(GLFWwindow* window, int focus)
	{
		window;
		if (focus) {
			temp::WindowFocusEvent WFE;
			temp::SEND_WINDOW_EVENT(WFE)
		}
		else {
			temp::WindowLostFocusEvent WLFE;
			temp::SEND_WINDOW_EVENT(WLFE)
		}
	}

	void WindowManager::window_pos_callback(GLFWwindow* window, int xPos, int yPos)
	{
		window;
		temp::WindowMovedEvent WME;
		WME.xpos = xPos;
		WME.ypos = yPos;

		temp::SEND_WINDOW_EVENT(WME)
	}

}