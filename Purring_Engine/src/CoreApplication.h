/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CoreApplication.h
 \creation date:       To check
 \last updated:        16-09-2023
 \author:              Brandon HO Jun Jie
 \co-author:           Hans (You Yang) ONG
 \co-author:           Jarran TAN Yan Zhi

 \par      email:      brandonjunjie.ho@digipen.edu
 \par      email:      youyang.o@digipen.edu
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
#include <vector>

#include "LayerStack.h"

//tmp
#include "WindowManager.h"
#include "FrameRateTargetControl.h"
#include "Graphics/RendererManager.h"
#include "AudioManager.h"

namespace PE
{
	// CoreApplication class definition
	class  CoreApplication
	{
	public:
		// Default constructor and virtual destructor
		CoreApplication();
		virtual ~CoreApplication();

		// Main application loop
		void Run();

		// Add a system to the application
		void AddSystem(System* system);

		// Initialize all systems in m_systemList
		void InitSystems();

		// Destroy all systems and release resources
		void DestroySystems();

		// Add a rendering layer to the application
		void AddLayer(Layer* layer);

		// Add an overlay layer to the application
		void AddOverlay(Layer* overlay);

		// Uncomment below if singletons are required
		//inline static Application& Get() { return *s_Instance; }
		//inline Window& GetWindow() { return *m_Window; }

	private:

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

		// Uncomment if a singleton instance is required
		//static Application* s_Instance;

		// Audio Stuff - HANS
		AudioManager m_audioManager;

	};

	// Function to create an instance of CoreApplication, defined by the client
	CoreApplication* CreateApplication();
}
