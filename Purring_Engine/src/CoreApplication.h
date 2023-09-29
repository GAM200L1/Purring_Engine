/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CoreApplication.h
 \date     28-08-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \co-author            Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \co-author            Jarran TAN Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu
 \par      code %:     <remove if sole author>
 \par      changes:    <remove if sole author>

 \brief    This file contains the CoreApplication class, which serves as the entry point for
		   the engine. It handles the main application loop, initializes and updates all registered
		   systems, and manages application-level resources such as the window and FPS controller.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#pragma once

#include "System.h"
#include <vector>

#include "LayerStack.h"

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
		CoreApplication();
		virtual ~CoreApplication();

		// Main application loop
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

		// Add a rendering layer to the application
		void AddLayer(Layer* layer);

		// Add an overlay layer to the application
		void AddOverlay(Layer* overlay);

		// Uncomment below if singletons are required
		//inline static Application& Get() { return *s_Instance; }
		//inline Window& GetWindow() { return *m_Window; }

	private:

		void InitializeVariables();
		void RegisterComponents();
		void InitializeLogger();
		void InitializeAudio();
		void InitializeMemoryManager();
		void InitializeSystems();

		// Uncomment if window is managed as a unique_ptr
		//std::unique_ptr<Window> m_Window;

		// Running state flag
		bool m_Running;
		// Time of the last frame for FPS calculations
		double m_lastFrameTime;

		// Container for all the systems in the engine
		std::vector<System*> m_systemList;

		// Layer stack to manage rendering layers
		LayerStack m_LayerStack;

		// Temporary (or additional) components
		WindowManager m_windowManager;						// Manages the application window
		GLFWwindow* m_window;								// Pointer to the GLFW window object
		FrameRateTargetControl m_fpsController;				// Controls the frame rate target
		float m_time;										// Placeholder for time value

	private:

	public:
	};

	// Function to create an instance of CoreApplication, defined by the client
	CoreApplication* CreateApplication();
}
