#pragma once

#include "System.h"
#include <vector>

//tmp
#include "WindowSystem.h"
#include "FrameRateTargetControl.h"

namespace PE {

	class  CoreApplication
	{
	public:
		CoreApplication();
		virtual ~CoreApplication();

		// main app loop
		void Run();
		
		// Add system to application
		void AddSystem(System* system);

		// Initializes all systems
		void Initialize();

		// tmp
		void PrintSpecs();

		//inline static Application& Get() { return *s_Instance; }
		//inline Window& GetWindow() { return *m_Window; }
	private:

		//std::unique_ptr<Window> m_Window;
		bool m_Running;
		float m_lastFrameTime;

		// holds all the systems in the engine
		std::vector<System*> m_systemList;

		// layer

		// layerstack

		// tmp
		WindowManager m_windowManager;
		GLFWwindow* m_window;
		FrameRateTargetControl m_fpsController;
		float m_time;


	private:
		//static Application* s_Instance;
	};

	// defined by client
	CoreApplication* CreateApplication();

}