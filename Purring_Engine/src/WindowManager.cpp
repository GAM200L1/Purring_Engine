/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     WindowManager.cpp
 \creation date:       13-08-2023
 \last updated:        16-09-2023
 \author:              Hans (You Yang) ONG

 \par      email:      youyang.o@digipen.edu

 \brief    This file contains the implementation of the WindowManager class.
		   WindowManager handles the initialization, maintenance, and cleanup
		   of a GLFW p_window, along with the relevant callbacks and p_window operations.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "WindowManager.h"
#include <iostream>
#include <sstream>
#include "Logging/Logger.h"
#include "Editor/Editor.h"
#include "InputSystem.h"
#include "Math/Transform.h"
#include "Physics/PhysicsManager.h"
#include "Graphics/RendererManager.h"
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
		// Attempt to initialize GLFW
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
	/// Initializes a GLFW p_window with the given width, height, and p_title.
	/// If p_window creation fails, it prints an error message, terminates GLFW and exits the program.
	/// </summary>
	/// <param name="width">Width of the p_window in pixels.</param>
	/// <param name="height">Height of the p_window in pixels.</param>
	/// <param name="p_title">Title to be displayed on the p_window bar.</param>
	/// <returns>A pointer to the created GLFWwindow. Returns nullptr if the p_window creation fails.</returns>
	----------------------------------------------------------------------------- */
	GLFWwindow* WindowManager::InitWindow(int width, int height, const char* p_title)
	{
		// Create a GLFW p_window with the specified dimensions and p_title
		GLFWwindow* window = glfwCreateWindow(width, height, p_title, nullptr, nullptr);

		// Check if p_window creation was successful
		if (!window)
		{
			// Log the error and terminate the GLFW library
			std::cerr << "Failed to create GLFW window." << std::endl;
			glfwTerminate();

			// Exit the program with an error code
			exit(-1);
		}

		// Make the newly created p_window the current OpenGL context
		glfwMakeContextCurrent(window);

		// Required to set p_window user pointer for accessing callback methods in this class
		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

		// Set GLFW input callbacks
		glfwSetCursorPosCallback(window, InputSystem::mouse_callback);				// For mouse movement
		glfwSetMouseButtonCallback(window, InputSystem::check_mouse_buttons);		// For mouse button presses
		glfwSetScrollCallback(window, InputSystem::scroll_callback);					// For scroll wheel events
		glfwSetKeyCallback(window, InputSystem::key_callback);						// For keyboard events

		// Set GLFW p_window callbacks
		glfwSetWindowSizeCallback(window, window_resize_callback);		// For p_window resizing
		glfwSetWindowCloseCallback(window, window_close_callback);		// For p_window closing
		glfwSetWindowFocusCallback(window, window_focus_callback);		// For p_window focus events
		glfwSetWindowPosCallback(window, window_pos_callback);			// For p_window position changes

		// Add event listeners (presumably custom macros)
		ADD_ALL_WINDOW_EVENT_LISTENER(WindowManager::OnWindowEvent, this)
		ADD_ALL_MOUSE_EVENT_LISTENER(WindowManager::OnMouseEvent, this)
		ADD_ALL_KEY_EVENT_LISTENER(WindowManager::OnKeyEvent, this)

		// Return the created p_window pointer
		return window;
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Handles p_window-related events.
	/// Logs the events and takes appropriate actions depending on the event type.
	/// </summary>
	/// <param name="r_event">The event to be processed.</param>
	----------------------------------------------------------------------------- */
	void WindowManager::OnWindowEvent(const PE::Event<PE::WindowEvents>& r_event)
	{
		Editor::GetInstance().AddEventLog(r_event.ToString());
		//commented so it stops flooding the console
		//event_logger.AddLog(false, r_event.ToString(), __FUNCTION__);
		//event_logger.FlushLog();
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Handles mouse-related events.
	/// Logs the events and performs appropriate actions based on the event type.
	/// </summary>
	/// <param name="r_event">The mouse event to be processed.</param>
	----------------------------------------------------------------------------- */
	void WindowManager::OnMouseEvent(const PE::Event<PE::MouseEvents>& r_event)
	{
		Editor::GetInstance().AddEventLog(r_event.ToString());
		if (r_event.GetType() == MouseEvents::MouseScrolled)
		{
			MouseScrolledEvent mse;
			mse = dynamic_cast<const MouseScrolledEvent&>(r_event);
			Graphics::RendererManager::m_mainCamera.AdjustMagnification(-mse.yOffset);

		}


		//commented so it stops flooding the console
		//event_logger.AddLog(false, r_event.ToString(), __FUNCTION__);
		//event_logger.FlushLog();
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Handles keyboard-related events.
	/// Logs the events and takes appropriate actions based on the type of keyboard event.
	/// </summary>
	/// <param name="r_event">The keyboard event to be processed.</param>
	----------------------------------------------------------------------------- */
	void WindowManager::OnKeyEvent(const PE::Event<PE::KeyEvents>& r_event)
	{
		Editor::GetInstance().AddEventLog(r_event.ToString());
		//commented so it stops flooding the console
		//event_logger.AddLog(false, r_event.ToString(), __FUNCTION__);
		//event_logger.FlushLog();

		//dynamic cast
		if (r_event.GetType() == KeyEvents::KeyTriggered)
		{
			KeyTriggeredEvent ev;
			ev = dynamic_cast<const KeyTriggeredEvent&>(r_event);
			//do step by step here
			if (PhysicsManager::GetStepPhysics())
			{
				if (ev.keycode == GLFW_KEY_N)
				{
					PhysicsManager::GetAdvanceStep() = true;
					Editor::GetInstance().AddEventLog("Advanced Step.\n");
				}
			}

			// dash
			if (EntityManager::GetInstance().GetComponentPool<RigidBody>().HasEntity(1))
			{
				if (ev.keycode == GLFW_KEY_E)
				{
					if (EntityManager::GetInstance().Get<RigidBody>(1).m_velocity.Dot(EntityManager::GetInstance().Get<RigidBody>(1).m_velocity) == 0.f)
						EntityManager::GetInstance().Get<RigidBody>(1).m_velocity = vec2{ 1.f, 0.f };
					EntityManager::GetInstance().Get<RigidBody>(1).ApplyLinearImpulse(EntityManager::GetInstance().Get<RigidBody>(1).m_velocity.GetNormalized() * 1000.f);
				}
			}
		}
		else if (r_event.GetType() == KeyEvents::KeyPressed)
		{
			KeyPressedEvent ev;
			ev = dynamic_cast<const KeyPressedEvent&>(r_event);
			// ----- M1 Movement ----- //
			if (EntityManager::GetInstance().GetComponentPool<RigidBody>().HasEntity(1))
			{
				if (ev.keycode == GLFW_KEY_W)
				{
					EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 0.f,1.f } *5000.f);
				}
				if (ev.keycode == GLFW_KEY_A)
				{
					EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ -1.f,0.f }*5000.f);
				}
				if (ev.keycode == GLFW_KEY_S)
				{
					EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 0.f,-1.f }*5000.f);
				}
				if (ev.keycode == GLFW_KEY_D)
				{
					EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 1.f,0.f }*5000.f);
				}

				// rotation
				if (ev.keycode == GLFW_KEY_RIGHT)
				{
					EntityManager::GetInstance().Get<RigidBody>(1).m_rotationVelocity = -PE_PI;
				}
				if (ev.keycode == GLFW_KEY_LEFT)
				{
					EntityManager::GetInstance().Get<RigidBody>(1).m_rotationVelocity = PE_PI;
				}

				// scale
				if (ev.keycode == GLFW_KEY_EQUAL)
				{
					EntityManager::GetInstance().Get<Transform>(1).width *= 1.01f;
					EntityManager::GetInstance().Get<Transform>(1).height *= 1.01f;
				}
				if (ev.keycode == GLFW_KEY_MINUS)
				{
					EntityManager::GetInstance().Get<Transform>(1).width *= 0.99f;
					EntityManager::GetInstance().Get<Transform>(1).height *= 0.99f;
				}
			}
		}

		//may want to change this to switch case to look cleaner
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Updates the p_window's p_title to display the current FPS.
	/// </summary>
	/// <param name="p_window">A pointer to the GLFWwindow whose p_title needs to be updated.</param>
	/// <param name="fps">Current frames per second value.</param>
	----------------------------------------------------------------------------- */
	void WindowManager::UpdateTitle(GLFWwindow* p_window, double fps)
	{
		std::ostringstream titleStream;
		titleStream << "Purring Engine | FPS: " << static_cast<int>(fps);
		glfwSetWindowTitle(p_window, titleStream.str().c_str());
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Releases any resources allocated by GLFW to clean up before program termination.
	/// </summary>
	----------------------------------------------------------------------------- */
	void WindowManager::Cleanup()
	{
		glfwTerminate();
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Callback function to handle p_window resizing events.
	/// </summary>
	/// <param name="p_window">Pointer to the GLFWwindow that has been resized.</param>
	/// <param name="width">New width of the p_window in pixels.</param>
	/// <param name="height">New height of the p_window in pixels.</param>
	----------------------------------------------------------------------------- */
	void WindowManager::window_resize_callback(GLFWwindow* p_window, int width, int height)
	{
		p_window;
		PE::WindowResizeEvent WRE;
		WRE.width = width;
		WRE.height = height;

		PE::SEND_WINDOW_EVENT(WRE)
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Callback function to handle p_window close events.
	/// </summary>
	/// <param name="p_window">Pointer to the GLFWwindow that is being closed.</param>
	----------------------------------------------------------------------------- */
	void WindowManager::window_close_callback(GLFWwindow* p_window)
	{
		p_window;
		PE::WindowCloseEvent WCE;
		PE::SEND_WINDOW_EVENT(WCE)
	}



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Callback function to handle p_window focus change events.
	/// </summary>
	/// <param name="p_window">Pointer to the GLFWwindow that has changed focus.</param>
	/// <param name="focus">Integer representing the focus state. 1 for focus gained, 0 for focus lost.</param>
	----------------------------------------------------------------------------- */
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



	/*-----------------------------------------------------------------------------
	/// <summary>
	/// Callback function to handle p_window position change events.
	/// </summary>
	/// <param name="p_window">Pointer to the GLFWwindow that has changed position.</param>
	/// <param name="xpos">New X-coordinate position of the p_window.</param>
	/// <param name="ypos">New Y-coordinate position of the p_window.</param>
	----------------------------------------------------------------------------- */
	void WindowManager::window_pos_callback(GLFWwindow* p_window, int xpos, int ypos)
	{
		p_window;
		PE::WindowMovedEvent WME;
		WME.xpos = xpos;
		WME.ypos = ypos;

		PE::SEND_WINDOW_EVENT(WME)
	}
}
