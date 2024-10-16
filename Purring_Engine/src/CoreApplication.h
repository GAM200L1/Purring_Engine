/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CoreApplication.h
 \date     28-08-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \co-author            Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \co-author            Jarran TAN Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief    This file contains the CoreApplication class, which serves as the entry point for
		   the engine. It handles the main application loop, initializes and updates all registered
		   systems, and manages application-level resources such as the window and FPS controller.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#pragma once

#include "System.h"

//tmp
#include "WindowManager.h"
#include "Time/FrameRateTargetControl.h"
#include "Graphics/RendererManager.h"

namespace PE
{
	// CoreApplication class definition
	class CoreApplication
	{
	public:
		// Default constructor and virtual destructor
		/*!***********************************************************************************
		 \brief Constructs CoreApplication object
		*************************************************************************************/
		CoreApplication();

		/*!***********************************************************************************
		 \brief Destroys CoreApplication object
		*************************************************************************************/
		virtual ~CoreApplication();

		/*!***********************************************************************************
		 \brief Loop for the main application
		*************************************************************************************/
		void Run();

		/*!***********************************************************************************
		 \brief     Initializes all the systems in the CoreApplication class.

		 \tparam T          This function does not use a template.
		 \return    void    This function returns void and performs its tasks by initializing
							each system in m_systemList.
		*************************************************************************************/
		void Initialize();

		/*!***********************************************************************************
		 \brief     Destroys all the systems in the CoreApplication class.

		 \tparam T          This function does not use a template.
		 \note              Memory is automatically deallocated by the Memory Manager, so the 'delete'
							operator is not used for the systems in this function.
		 \return    void    This function returns void and performs its tasks by destroying
							each system in m_systemList.
		*************************************************************************************/
		void DestroySystems();

		// Add a system to the application
		void AddSystem(System* system);

	private:

		/*!***********************************************************************************
		 \brief Initializes variables for the CoreApplication class.
		*************************************************************************************/
		void InitializeVariables();

		/*!***********************************************************************************
		 \brief Initializes logger.
		*************************************************************************************/
		void InitializeLogger();

		/*!***********************************************************************************
		 \brief Initializes variables for the CoreApplication class.
		*************************************************************************************/
		void InitializeAudio();

		/*!***********************************************************************************
		 \brief Initializes memory manager.
		*************************************************************************************/
		void InitializeMemoryManager();

		/*!***********************************************************************************
		 \brief Initializes systems.
		*************************************************************************************/
		void InitializeSystems();

		// Uncomment if window is managed as a unique_ptr
		//std::unique_ptr<Window> m_Window;

		// Running state flag
		bool m_Running;
		// Time of the last frame for FPS calculations
		double m_lastFrameTime;

		// Container for all the systems in the engine
		std::vector<System*> m_systemList;

		// Temporary (or additional) components
		WindowManager m_windowManager;						// Manages the application window
		GLFWwindow* m_window;								// Pointer to the GLFW window object
		float m_time;										// Placeholder for time value

		bool skipFrame{ false };										// Flag to skip a frame
	};

	// Function to create an instance of CoreApplication, defined by the client
	CoreApplication* CreateApplication();
}
