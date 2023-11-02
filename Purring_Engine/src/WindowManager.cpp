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
#include "Editor/Editor.h"
#include "Input/InputSystem.h"
#include "Math/Transform.h"
#include "Physics/PhysicsManager.h"
#include "Graphics/RendererManager.h"
#include "GUISystem.h"
//logger instantiation
Logger event_logger = Logger("EVENT");

namespace PE
{

	/*!***********************************************************************************
	 \brief     Default constructor for WindowManager. Initializes GLFW for window creation.

	 \tparam T          This function does not use a template.
	 \return T          Does not return a value, as this is a constructor.
	*************************************************************************************/
	WindowManager::WindowManager()
	{
		// Attempt to initialize GLFW
		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW." << std::endl;
			exit(-1);
		}
	}



	/*!***********************************************************************************
	 \brief     Destructor for WindowManager. Calls Cleanup to release resources.

	 \tparam T          This function does not use a template.
	 \return void       Does not return a value, as this is a destructor.
	*************************************************************************************/
	WindowManager::~WindowManager()
	{
		Cleanup();
	}



	/*!***********************************************************************************
	 \brief     Initializes a new GLFW window with the specified dimensions and title,
				and sets up all necessary callbacks.

	 \tparam T          This function does not use a template.
	 \param[in] width   The width of the new window.
	 \param[in] height  The height of the new window.
	 \param[in] p_title The title to be displayed on the new window.
	 \return GLFWwindow* Pointer to the newly created GLFW window, or nullptr if initialization fails.
	*************************************************************************************/
	GLFWwindow* WindowManager::InitWindow(int width, int height, const char* p_title)
	{
		GLFWwindow* window = glfwCreateWindow(width, height, p_title, nullptr, nullptr);

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

		ADD_ALL_WINDOW_EVENT_LISTENER(WindowManager::OnWindowEvent, this)
		ADD_ALL_MOUSE_EVENT_LISTENER(WindowManager::OnMouseEvent, this)
		ADD_ALL_KEY_EVENT_LISTENER(WindowManager::OnKeyEvent, this)

		REGISTER_UI_FUNCTION(TestFunction, WindowManager);

		return window;
	}



	/*!***********************************************************************************
	 \brief     Handles window-related events by adding them to the event log.

	 \tparam T          This function does not use a template.
	 \param[in] e       Event of type PE::WindowEvents containing the details of the window event.
	 \return void       Does not return a value.
	*************************************************************************************/
	void WindowManager::OnWindowEvent(const PE::Event<PE::WindowEvents>& r_event)
	{
		Editor::GetInstance().AddEventLog(r_event.ToString());
		//commented so it stops flooding the console
		//event_logger.AddLog(false, r_event.ToString(), __FUNCTION__);
		//event_logger.FlushLog();
	}



	/*!***********************************************************************************
	 \brief     Handles mouse-related events by adding them to the event log.

	 \tparam T          This function does not use a template.
	 \param[in] e       Event of type PE::MouseEvents containing the details of the mouse event.
	 \return void       Does not return a value.
	*************************************************************************************/
	void WindowManager::OnMouseEvent(const PE::Event<PE::MouseEvents>& r_event)
	{
		Editor::GetInstance().AddEventLog(r_event.ToString());		

		//commented so it stops flooding the console
		//event_logger.AddLog(false, r_event.ToString(), __FUNCTION__);
		//event_logger.FlushLog();
	}



	/*!***********************************************************************************
	 \brief     Handles keyboard-related events and delegates actions to respective game systems.

	 \tparam T          This function does not use a template.
	 \param[in] r_event Event object of type PE::KeyEvents that encapsulates the details of the keyboard event.
	 \return void       Does not return a value.
	*************************************************************************************/
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

			//// Movement
			//if (ev.keycode == GLFW_KEY_W)
			//{
			//	EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 0.f,1.f } *5000.f);
			//}
			//if (ev.keycode == GLFW_KEY_A)
			//{
			//	EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ -1.f,0.f }*5000.f);
			//}
			//if (ev.keycode == GLFW_KEY_S)
			//{
			//	EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 0.f,-1.f }*5000.f);
			//}
			//if (ev.keycode == GLFW_KEY_D)
			//{
			//	EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 1.f,0.f }*5000.f);
			//}
			// dash
			if (EntityManager::GetInstance().GetComponentPool<RigidBody>().HasEntity(1))
			{
				if (ev.keycode == GLFW_KEY_E)
				{
					if (EntityManager::GetInstance().Get<RigidBody>(1).velocity.Dot(EntityManager::GetInstance().Get<RigidBody>(1).velocity) == 0.f)
						EntityManager::GetInstance().Get<RigidBody>(1).velocity = vec2{ 1.f, 0.f };
					EntityManager::GetInstance().Get<RigidBody>(1).ApplyLinearImpulse(EntityManager::GetInstance().Get<RigidBody>(1).velocity.GetNormalized() * 5000.f);
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
				//if (ev.keycode == GLFW_KEY_W)
				//{
				//	EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 0.f,1.f } *5000.f);
				//}
				//if (ev.keycode == GLFW_KEY_A)
				//{
				//	EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ -1.f,0.f }*5000.f);
				//}
				//if (ev.keycode == GLFW_KEY_S)
				//{
				//	EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 0.f,-1.f }*5000.f);
				//}
				//if (ev.keycode == GLFW_KEY_D)
				//{
				//	EntityManager::GetInstance().Get<RigidBody>(1).ApplyForce(vec2{ 1.f,0.f }*5000.f);
				//}

				// rotation
				if (ev.keycode == GLFW_KEY_X)
				{
					EntityManager::GetInstance().Get<RigidBody>(1).rotationVelocity = -PE_PI;
				}
				if (ev.keycode == GLFW_KEY_Z)
				{
					EntityManager::GetInstance().Get<RigidBody>(1).rotationVelocity = PE_PI;
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
	}

	void WindowManager::TestFunction()
	{
		std::cout << "hi im a test function" << std::endl;
	}



	/*!***********************************************************************************
	 \brief     Updates the title of the given GLFW window to display the current FPS.

	 \tparam T          This function does not use a template.
	 \param[in] window  Pointer to the GLFW window whose title is to be updated.
	 \param[in] fps     Current frames per second to be displayed in the window title.
	 \return void       Does not return a value.
	*************************************************************************************/
	void WindowManager::UpdateTitle(GLFWwindow* p_window, double fps)
	{
		std::ostringstream titleStream;
		titleStream << "Purring Engine | FPS: " << static_cast<int>(fps);
		glfwSetWindowTitle(p_window, titleStream.str().c_str());
	}



	/*!***********************************************************************************
	 \brief     Terminates the GLFW library, releasing any resources allocated by GLFW.

	 \tparam T          This function does not use a template.
	 \param[in]         This function does not use any input parameters.
	 \return void       Does not return a value.
	*************************************************************************************/
	void WindowManager::Cleanup()
	{
		glfwTerminate();
	}



	/*!***********************************************************************************
	 \brief     GLFW callback function that triggers when the window is resized.

	 \tparam T          This function does not use a template.
	 \param[in] window  Pointer to the GLFW window that is being resized.
	 \param[in] width   New width of the window.
	 \param[in] height  New height of the window.
	 \return void       Does not return a value.
	*************************************************************************************/
	void WindowManager::window_resize_callback(GLFWwindow* p_window, int width, int height)
	{
		p_window;
		PE::WindowResizeEvent WRE;
		WRE.width = width;
		WRE.height = height;

		PE::SEND_WINDOW_EVENT(WRE)
	}



	/*!***********************************************************************************
	 \brief     GLFW callback function that triggers when the window is closed.

	 \tparam T          This function does not use a template.
	 \param[in] window  Pointer to the GLFW window that is being closed.
	 \return void       Does not return a value.
	*************************************************************************************/
	void WindowManager::window_close_callback(GLFWwindow* p_window)
	{
		p_window;
		PE::WindowCloseEvent WCE;
		PE::SEND_WINDOW_EVENT(WCE)
	}



	/*!***********************************************************************************
	 \brief     GLFW callback function that triggers when the window gains or loses focus.

	 \tparam T          This function does not use a template.
	 \param[in] window  Pointer to the GLFW window whose focus state has changed.
	 \param[in] focus   An integer representing the focus state: 1 if gained focus, 0 if lost focus.
	 \return void       Does not return a value.
	*************************************************************************************/
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



	/*!***********************************************************************************
	 \brief     GLFW callback function that triggers when the window's position changes.

	 \tparam T          This function does not use a template.
	 \param[in] window  Pointer to the GLFW window whose position has changed.
	 \param[in] xpos    The new x-coordinate of the top-left corner of the window.
	 \param[in] ypos    The new y-coordinate of the top-left corner of the window.
	 \return void       Does not return a value.
	*************************************************************************************/
	void WindowManager::window_pos_callback(GLFWwindow* p_window, int xpos, int ypos)
	{
		p_window;
		PE::WindowMovedEvent WME;
		WME.xpos = xpos;
		WME.ypos = ypos;

		PE::SEND_WINDOW_EVENT(WME)
	}
}
