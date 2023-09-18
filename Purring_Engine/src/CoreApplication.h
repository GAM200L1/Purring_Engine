#pragma once

#include "System.h"
#include <vector>

#include "LayerStack.h"

//tmp
#include "WindowManager.h"
#include "FrameRateTargetControl.h"
#include "Graphics/RendererManager.h"
#include "AudioManager.h"

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

		//inline static Application& Get() { return *s_Instance; }
		//inline Window& GetWindow() { return *m_Window; }
	private:

		//std::unique_ptr<Window> m_Window;
		bool m_Running{};
		double m_lastFrameTime{};

		// holds all the systems in the engine
		std::vector<System*> m_systemList;

		// layerstack
		LayerStack m_LayerStack;

		// tmp
		WindowManager m_windowManager;
		GLFWwindow* m_window;
		FrameRateTargetControl m_fpsController;
		Graphics::RendererManager* m_rendererManager;
		float m_time;


	private:
		//static Application* s_Instance;

		// Audio Stuff - HANS
		//AudioManager m_audioManager;
	};

	// defined by client
	CoreApplication* CreateApplication();

}