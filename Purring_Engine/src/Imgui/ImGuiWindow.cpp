/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     ImGuiWindow.cpp
 \date     8/30/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	cpp file containing the definition of the imguiwindow class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "ImGuiWindow.h"
#include "MemoryManager.h"
#include "AudioManager.h"
namespace PE {
	//single static instance of imguiwindow 
	std::unique_ptr<ImGuiWindow> ImGuiWindow::s_Instance = nullptr;

	ImGuiWindow::ImGuiWindow() {
		//initializing variables 
		m_consoleActive = true;
		m_logsActive = true;
		m_objectActive = true;
		m_sceneViewActive = true;
		m_debugTests = true;
		//m_firstLaunch needs to be serialized 
		m_firstLaunch = true;
		//show the entire gui
		m_showEditor = true;
		//Subscribe to key pressed event 
		ADD_KEY_EVENT_LISTENER(temp::KeyEvents::KeyPressed, ImGuiWindow::OnKeyEvent, this)

			m_currentSelectedIndex = 0;

		m_items = {};
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

	bool ImGuiWindow::IsEditorActive()
	{
		return m_sceneViewActive;
	}

	void ImGuiWindow::ToggleEditor()
	{
		if (m_showEditor) {
			m_showEditor = !m_showEditor;
			m_consoleActive = false;
			m_logsActive = false;
			m_objectActive = false;
			m_sceneViewActive = false;
			m_debugTests = false;
		}
		else {
			m_showEditor = !m_showEditor;
			m_consoleActive = true;
			m_logsActive = true;
			m_objectActive = true;
			m_sceneViewActive = true;
			m_debugTests = true;
		}
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
		io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));

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

		if (m_showEditor) {
			//draw docking port
			SetDockingPort(&m_showEditor);
		}
		//render logs
		if (m_logsActive)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
			ShowLogs(&m_logsActive);
		}

		//render object list
		if (m_objectActive)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
			ShowObject(&m_objectActive);
		}

		//render console
		if (m_consoleActive)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
			ShowConsole(&m_consoleActive);
		}

		//draw scene view
		if (m_sceneViewActive)
			ShowSceneView(texture_id, &m_sceneViewActive);

		if (m_debugTests)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
			ShowDebugTests(&m_debugTests);
		}


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

	void ImGuiWindow::ShowLogs(bool* Active)
	{

		if (!ImGui::Begin("debugwindow", Active))
		{
			ImGui::End();
		}
		else
		{
			if (ImGui::Button("clear log"))
			{
				ClearLog();
				AddLog("Log Cleared");
			}
			ImGui::Separator();

			//show logs
			if (ImGui::BeginChild("log scroll area", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {

				ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x);
				for (std::string i : m_logOutput)
				{
					ImGui::Text(i.c_str());
				}
				ImGui::PopTextWrapPos();
				if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
					ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}

	void ImGuiWindow::ShowConsole(bool* Active)
	{
		if (!ImGui::Begin("consolewindow", Active))
		{
			ImGui::End();
		}
		else {
			if (ImGui::Button("clear console"))
			{
				ClearConsole();
				AddConsole("console Cleared");
			}
			ImGui::Separator();

			float const spacing = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
			if (ImGui::BeginChild("console scroll area", ImVec2(0, -spacing), true, ImGuiWindowFlags_HorizontalScrollbar)) {
				for (std::string i : m_consoleOutput)
				{
					ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x);
					ImVec4 color;
					bool has_color = false;
					if (i.find("[ERROR]") != std::string::npos) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
					//else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
					if (has_color)
						ImGui::PushStyleColor(ImGuiCol_Text, color);
					ImGui::TextUnformatted(i.c_str());
					if (has_color)
						ImGui::PopStyleColor();
					ImGui::PopTextWrapPos();

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
				AddConsole(m_input);
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

	void ImGuiWindow::ShowObject(bool* Active)
	{
		if (!ImGui::Begin("objectlistwindow", Active))
		{
			ImGui::End();
		}
		else
		{
			static int count = 0;
			if (ImGui::Button("Create Object"))
			{
				AddConsole("Object Created");

				std::stringstream ss;
				ss << "new object " << count++;
				m_items.push_back(ss.str().c_str());
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Object"))
			{
				if (m_currentSelectedIndex <= m_items.size() && !m_items.empty()) {
					AddConsole("Object Deleted");
					std::stringstream ss;
					ss << "deleted object " << m_currentSelectedIndex;
					m_items.erase(m_items.begin() + m_currentSelectedIndex);
					count--;
				}
			}
			ImGui::SameLine();
			ImGui::BeginDisabled(true);
			if (ImGui::Button("Clone Object"))
			{
			}
			ImGui::EndDisabled();
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

	void ImGuiWindow::ShowDebugTests(bool* Active)
	{
		if (!ImGui::Begin("debugTests", Active, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
		}
		else
		{
			static int allocated = 1;
			//test 1
			ImGui::Text("Memory Test");
			ImGui::Spacing();
			if (ImGui::Button("Allocating Memory"))
			{
				char* allocationtest = (char*)MemoryManager::GetInstance()->AllocateMemory("AllocationTest", 30);
				allocated++;
			}
			ImGui::SameLine();
			//test 2
			if (ImGui::Button("Create Out of Index Object on Stack"))
			{
				char* outofmemorytest = (char*)MemoryManager::GetInstance()->AllocateMemory("out of index test", 1000);
			}
			//test 3
			static bool buffertester = false;
			ImGui::BeginDisabled(buffertester);
			if (ImGui::Button("Written over Buffer Tests"))
			{
				buffertester = true;
				char* buffertest = (char*)MemoryManager::GetInstance()->AllocateMemory("buffertest", 7);

				strcpy(buffertest, "testtest");
				AddConsole("writing \"testtest\" 8byte into buffertest of 7 byte + 4 buffer bytes");
				AddConsole(buffertest);
				allocated++;
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			//test 4
			ImGui::BeginDisabled(!(allocated > 1));
			if (ImGui::Button("popback previous allocation"))
			{
				buffertester = false;
				MemoryManager::GetInstance()->Pop_BackMemory();
				AddConsole("delete and popping back previously allocated object");
				allocated--;

			}
			ImGui::EndDisabled();
			//print memory stuff
			if (ImGui::Button("Print Memory Data"))
			{
				MemoryManager::GetInstance()->PrintData();
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space

			ImGui::Separator();
			//audio
			ImGui::Text("Audio Test");
			if (ImGui::Button("Play Audio 1"))
			{
				AudioManager::GetInstance()->PlaySound("Audio/sound2.wav");
			}
			ImGui::SameLine();
			if (ImGui::Button("Play Audio 2"))
			{
				AudioManager::GetInstance()->PlaySound("Audio/sound1.wav");
			}
			ImGui::End();
		}
	}

	void ImGuiWindow::SetDockingPort(bool* Active)
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

		if (!ImGui::Begin("DockSpace", Active, window_flags))
		{
			ImGui::End();
		}
		else {
			ImGuiIO& io = ImGui::GetIO();
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

				//set default docking positions, may need to use serialization to set first launch
				if (m_firstLaunch)
				{
					//after setting first dock positions
					m_firstLaunch = false;

					//start dock
					ImGui::DockBuilderRemoveNode(dockspace_id);
					ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
					ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

					//Imgui docks right side by default
					ImGui::DockBuilderDockWindow("objectlistwindow", dockspace_id);

					//set the other sides
					ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.3f, nullptr, &dockspace_id);
					ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.7f, &dockspace_id, &dockspace_id);

					//setting the other dock locations
					ImGui::DockBuilderDockWindow("sceneview", dock_id_left);

					//set on the save location to dock ontop of eachother
					ImGui::DockBuilderDockWindow("consolewindow", dock_id_down);

					ImGuiID dock_id_down2 = ImGui::DockBuilderSplitNode(dock_id_down, ImGuiDir_Right, 0.5f, nullptr, &dock_id_down);

					ImGui::DockBuilderDockWindow("debugwindow", dock_id_down2);

					//end dock
					ImGui::DockBuilderFinish(dockspace_id);

				}

			}

			//docking port menu bar
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
					if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}
					ImGui::Separator();
					if (ImGui::MenuItem("Cut", "CTRL+X")) {}
					if (ImGui::MenuItem("Copy", "CTRL+C")) {}
					if (ImGui::MenuItem("Paste", "CTRL+V")) {}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Window"))
				{
					if (ImGui::MenuItem("console", "f1", m_consoleActive, !m_consoleActive))
					{
						m_consoleActive = !m_consoleActive;
					}
					if (ImGui::MenuItem("object list", "f2", m_objectActive, !m_objectActive))
					{
						m_objectActive = !m_objectActive;
					}
					if (ImGui::MenuItem("logs", "f3", m_logsActive, !m_logsActive))
					{
						m_logsActive = !m_logsActive;
					}
					if (ImGui::MenuItem("Scenes", "f4", m_sceneViewActive, !m_sceneViewActive))
					{
						m_sceneViewActive = !m_sceneViewActive;
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Close Editor", "esc", m_showEditor, true))
					{
						ToggleEditor();
					}
					if (ImGui::MenuItem("test", "test")) {}
					if (ImGui::MenuItem("test", "test")) {}
					ImGui::EndMenu();
				}
			}
			ImGui::EndMainMenuBar();
			ImGui::End();
		}

	}

	void ImGuiWindow::ShowSceneView(GLuint texture_id, bool* active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
		ImGui::Begin("sceneview", active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

		m_renderWindowWidth = ImGui::GetContentRegionAvail().x;
		m_renderWindowHeight = ImGui::GetContentRegionAvail().y;
		ImVec2 pos = ImGui::GetCursorScreenPos();

		ImGui::Image(
			reinterpret_cast<void*>(
				static_cast<intptr_t>(texture_id)),
			ImVec2(m_renderWindowWidth, m_renderWindowHeight),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::End();
	}

	void ImGuiWindow::AddLog(std::string text)
	{
		m_logOutput.push_back(text);
	}

	void ImGuiWindow::AddConsole(std::string text)
	{
		m_consoleOutput.push_back(text);
	}

	void ImGuiWindow::AddError(std::string text)
	{
		std::stringstream ss;
		ss << "[ERROR] " << text;
		m_consoleOutput.push_back(ss.str());

	}

	void ImGuiWindow::ClearLog()
	{
		m_logOutput.clear();
	}

	void ImGuiWindow::ClearConsole()
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

		//may want to change this to switch case to look cleaner

		if (KPE.keycode == GLFW_KEY_F1)
		{
			m_consoleActive = !m_consoleActive;
		}

		if (KPE.keycode == GLFW_KEY_F2)
		{
			m_objectActive = !m_objectActive;
		}

		if (KPE.keycode == GLFW_KEY_F3)
		{
			m_logsActive = !m_logsActive;
		}

		if (KPE.keycode == GLFW_KEY_F4)
		{
			m_sceneViewActive = !m_sceneViewActive;
		}

		if (KPE.keycode == GLFW_KEY_ESCAPE)
		{
			ToggleEditor();

		}
	}
}

