#pragma once

namespace Purr {

	class  Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		//inline static Application& Get() { return *s_Instance; }
		//inline Window& GetWindow() { return *m_Window; }
	private:

		//std::unique_ptr<Window> m_Window;
		//bool m_Running = true;
	private:
		//static Application* s_Instance;
	};

	//To be defined in client
	Application* CreateApplication();

}