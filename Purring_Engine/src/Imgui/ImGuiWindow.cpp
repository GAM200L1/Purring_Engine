/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     ImGuiWindow.cpp
 \date     9/5/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
Header file containing the declaration and definition of the event and event dispatcher template

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "ImGuiWindow.h"

//single static instance of imguiwindow
std::unique_ptr<ImGuiWindow> ImGuiWindow::s_Instance = nullptr;

ImGuiWindow::ImGuiWindow() {
	//initializing variables
	m_consoleActive = true;
	m_logsActive = true;
	m_ObjectActive = true;
	m_sceneViewActive = true;

	//Subscribe to key pressed event
	ADD_KEY_EVENT_LISTENER(temp::KeyEvents::KeyPressed, ImGuiWindow::OnKeyEvent, this)

	m_currentSelectedIndex = 0;

	m_items = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
}

ImGuiWindow::~ImGuiWindow()
{
}

ImGuiWindow* ImGuiWindow::GetInstance()
{
	//may need to make another function to manually allocate memory for this
	if (!s_Instance)
		s_Instance = std::make_unique<ImGuiWindow>();

	return s_Instance.get();
}

void ImGuiWindow::GetWindowSize(float& width, float& height)
{
	width = m_renderWindowWidth;
	height = m_renderWindowHeight;
}

bool ImGuiWindow::isSceneViewActive()
{
	return m_sceneViewActive;
}

void ImGuiWindow::Init(GLFWwindow* m_window)
{
	//check imgui's version
	IMGUI_CHECKVERSION();
	//create imgui context
	ImGui::CreateContext();
	//set to dark mode
	ImGui::StyleColorsDark();

	//setting the flags for imgui
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;


	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	io.DisplaySize = ImVec2(width, height);

	//looks nicer
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	//init imgui for glfw and opengl
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);

	ImGui_ImplOpenGL3_Init("#version 460");
}

void ImGuiWindow::Render(GLuint texture_id)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//draw docking port
	setDockingPort();

	//render logs
	if (m_logsActive)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
		showLogs(&m_logsActive);
	}

	//render object list
	if (m_ObjectActive)
	{
		showObject(&m_ObjectActive);
	}

	//render console
	if (m_consoleActive)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
		showConsole(&m_consoleActive);
	}

	//draw scene view
	if(m_sceneViewActive)
	showSceneView(texture_id, &m_sceneViewActive);

	//final imgui render functions
	ImGui::Render();

	ImGuiIO& io = ImGui::GetIO();

	float time = (float)glfwGetTime();
	io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
	m_time = time;

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ImGuiWindow::showLogs(bool* Active)
{

	if (!ImGui::Begin("Debug Log Window", Active))
	{
		ImGui::End();
	}
	else
	{
		if (ImGui::Button("clear log"))
		{
			clearLog();
			addLog("Log Cleared");
		}
		ImGui::Separator();

		//show logs
		if (ImGui::BeginChild("log scroll area", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {


			for (std::string i : m_logOutput)
			{
				ImGui::Text(i.c_str());
			}
			if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}
}

void ImGuiWindow::showConsole(bool* Active)
{
	if (!ImGui::Begin("console Window", Active))
	{
		ImGui::End();
	}
	else {
		if (ImGui::Button("clear console"))
		{
			clearConsole();
			addCommand("console Cleared");
		}
		ImGui::Separator();

		float const spacing = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("console scroll area", ImVec2(0, -spacing), true, ImGuiWindowFlags_HorizontalScrollbar)) {


			for (std::string i : m_consoleOutput)
			{
				ImGui::Text(i.c_str());
			}
			if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();

		ImGui::Separator();

		bool reclaim_focus = false;
		ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
		if (ImGui::InputText("Type stuff", &m_input, input_text_flags))
		{
			addCommand(m_input);
			m_input = "";
			reclaim_focus = true;
		}

		// Auto-focus on window apparition
		ImGui::SetItemDefaultFocus();
		if (reclaim_focus)
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
		ImGui::End();
	}
}

void ImGuiWindow::showObject(bool* Active)
{
	if (!ImGui::Begin("test", Active))
	{
		ImGui::End();
	}
	else
	{
		if (ImGui::Button("Create Object"))
		{
			addCommand("Object Created");
			static int count = 0;
			std::stringstream ss;
			ss << "new object " << count++;
			m_items.push_back(ss.str().c_str());
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Object"))
		{
			if (m_currentSelectedIndex <= m_items.size() && !m_items.empty()) {
				addCommand("Object Deleted");
				std::stringstream ss;
				ss << "deleted object " << m_currentSelectedIndex;
				m_items.erase(m_items.begin() + m_currentSelectedIndex);
			}
		}



		ImGui::Separator();

		if (ImGui::BeginChild("GameObjectList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {

			for (int n = 0; n < m_items.size(); n++)
			{
				const bool is_selected = (m_currentSelectedIndex == n);
				if (ImGui::Selectable(m_items[n].c_str(), is_selected))
				{
					m_currentSelectedIndex = n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

		}
		ImGui::EndChild();

		ImGui::End();
	}
}

void ImGuiWindow::setDockingPort()
{
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	window_flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("DockSpace Demo", nullptr, window_flags);

	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("console", NULL, m_consoleActive, !m_logsActive))
			{
				m_consoleActive = !m_consoleActive;
			}
			if (ImGui::MenuItem("logs", NULL, m_logsActive, !m_logsActive))
			{
				m_logsActive = !m_logsActive;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("test", "test", false, false)) {}
			if (ImGui::MenuItem("test", "test")) {}
			if (ImGui::MenuItem("test", "test")) {}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();


	ImGui::End();
}

void ImGuiWindow::showSceneView(GLuint texture_id, bool* active)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
	ImGui::Begin("My Scene", active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	m_renderWindowWidth = ImGui::GetContentRegionAvail().x;
	m_renderWindowHeight = ImGui::GetContentRegionAvail().y;
	ImVec2 pos = ImGui::GetCursorScreenPos();

	ImGui::Image(
		(void*)texture_id,
		ImVec2(m_renderWindowWidth, m_renderWindowHeight),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	ImGui::End();
}

void ImGuiWindow::addLog(std::string text)
{
	m_logOutput.push_back(text);
}

void ImGuiWindow::addCommand(std::string text)
{
	m_consoleOutput.push_back(text);
}

void ImGuiWindow::clearLog()
{
	m_logOutput.clear();
}

void ImGuiWindow::clearConsole()
{
	m_consoleOutput.clear();
}

void ImGuiWindow::OnKeyEvent(const temp::Event<temp::KeyEvents>& e)
{
	temp::KeyPressedEvent KPE;

	//dynamic cast
	if (e.GetType() == temp::KeyEvents::KeyPressed)
	{
		KPE = dynamic_cast<const temp::KeyPressedEvent&>(e);
	}

	if (KPE.keycode == GLFW_KEY_F1)
	{
		m_consoleActive = !m_consoleActive;
	}

	if (KPE.keycode == GLFW_KEY_F2)
	{
		m_ObjectActive = !m_ObjectActive;
	}

	if (KPE.keycode == GLFW_KEY_F3)
	{
		m_logsActive = !m_logsActive;
	}

	if (KPE.keycode == GLFW_KEY_ESCAPE)
	{
		m_items.erase(m_items.begin() + m_currentSelectedIndex);
	}
}