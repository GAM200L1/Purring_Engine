#pragma once

#include "System.h"
#include <vector>

#include "LayerStack.h"

//tmp
#include "WindowManager.h"
#include "FrameRateTargetControl.h"
#include "Graphics/RendererManager.h"

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
		void InitSystems();
			
		// Destroys all systems
		void DestroySystems();

		// OnEvent?

		void AddLayer(Layer* layer);
		void AddOverlay(Layer* overlay);

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

		// layerstack
		LayerStack m_LayerStack;

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