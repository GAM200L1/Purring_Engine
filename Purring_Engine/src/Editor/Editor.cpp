/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     Editor.cpp
 \date     8/30/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	cpp file containing the definition of the editor class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "Editor.h"
#include "MemoryManager.h"
#include "AudioManager.h"
#include "Time/TimeManager.h"

namespace PE {
	//single static instance of imguiwindow 
	std::unique_ptr<Editor> Editor::s_Instance = nullptr;

	Editor::Editor() {
		//initializing variables 
		//m_firstLaunch needs to be serialized 
		m_firstLaunch = true;
		//if firstlaunch, everything is true and needs to be serialized to true, otherwise let imgui do it
		m_showConsole = true;
		m_showLogs = true;
		m_showObjectList = true;
		m_showSceneView = true;
		m_showTestWindows = true;
		m_showComponentWindow = true;
		m_showResourceWindow = true;
		m_showPerformanceWindow = false;
		//show the entire gui 
		m_showEditor = true; // depends on the mode, whether we want to see the scene or the editor

		//Subscribe to key pressed event 
		ADD_KEY_EVENT_LISTENER(temp::KeyEvents::KeyPressed, Editor::OnKeyPressedEvent, this)
		ADD_MOUSE_EVENT_LISTENER(temp::MouseEvents::MouseButtonPressed, Editor::OnMousePressedEvent, this)
		//for the object list
		m_objectIsSelected = false;
		m_currentSelectedIndex = 0;
		m_items = {};
	}

	Editor::~Editor()
	{
	}

	Editor* Editor::GetInstance()
	{
		if (!s_Instance)
			s_Instance = std::make_unique<Editor>();

		return s_Instance.get();
	}

	void Editor::GetWindowSize(float& width, float& height)
	{
		width = m_renderWindowWidth;
		height = m_renderWindowHeight;
	}

	bool Editor::IsEditorActive()
	{
		return m_showSceneView;
	}

	void Editor::ToggleEditor()
	{
		if (m_showEditor) {
			m_showEditor = !m_showEditor;
			m_showConsole = false;
			m_showLogs = false;
			m_showObjectList = false;
			m_showSceneView = false;
			m_showTestWindows = false;
		}
		else {
			m_showEditor = !m_showEditor;
			m_showConsole = true;
			m_showLogs = true;
			m_showObjectList = true;
			m_showSceneView = true;
			m_showTestWindows = true;
		}
	}

	void Editor::Init(GLFWwindow* m_window)
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


		p_window = m_window;
		//getting the full display size of glfw so that the ui know where to be in
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
	
	void Editor::Render(GLuint texture_id)
	{
		//imgui start frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//show docking port
		if (m_showEditor) SetDockingPort(&m_showEditor);

		//render logs
		if (m_showLogs) ShowLogs(&m_showLogs);

		//render object list
		if (m_showObjectList) ShowObject(&m_showObjectList);

		//the components on the object
		if (m_showComponentWindow) ShowComponentWindow(&m_showComponentWindow);

		//render console
		if (m_showConsole) ShowConsole(&m_showConsole);

		//draw scene view
		if (m_showSceneView) ShowSceneView(texture_id, &m_showSceneView);

		//draw the stuff for ellie to test
		if (m_showTestWindows) ShowDebugTests(&m_showTestWindows);

		if (m_showResourceWindow) ShowResourceWindow(&m_showResourceWindow);

		if (m_showPerformanceWindow) ShowPerformanceWindow(&m_showPerformanceWindow);

		//imgui end frame render functions
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

	void Editor::ShowLogs(bool* Active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
		//if active
		if (!ImGui::Begin("debugwindow", Active))
		{
			ImGui::End();			//imgui syntax
		}
		else
		{
			if (ImGui::Button("clear log")) // button
			{
				ClearLog();
				AddLog("Log Cleared");
			}
			ImGui::Separator(); // line

			//show the text in logs
			if (ImGui::BeginChild("log scroll area", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) 
			{

				ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x); //set text wrap
				for (std::string i : m_logOutput){ ImGui::Text(i.c_str());} // loop all the text
				ImGui::PopTextWrapPos(); // close text wrap
				if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) ImGui::SetScrollHereY(1.0f); //set auto scroll when at bottom
			}
			ImGui::EndChild(); // end child area
			ImGui::End(); //end drawing
		}
	}

	void Editor::ShowConsole(bool* Active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("consolewindow", Active)) // start drawing
		{
			ImGui::End(); //imgui syntax if inactive
		}
		else {
			if (ImGui::Button("clear console")) //button
			{
				ClearConsole();
				AddConsole("console Cleared");
			}
			ImGui::Separator(); // draw a line

			//frame height w spacing is the size of an element, item spacing is the spacing between objects
			float const spacing = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); //get enough spacing for a text bar at the bottom
			
			if (ImGui::BeginChild("console scroll area", ImVec2(0, -spacing), true, ImGuiWindowFlags_HorizontalScrollbar)) //start drawing child
			{
				for (std::string i : m_consoleOutput)
				{
					ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x); //text wrap
					ImVec4 color;
					bool has_color = false;
					if (i.find("[ERROR]") != std::string::npos) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
					if (has_color) ImGui::PushStyleColor(ImGuiCol_Text, color); // set color style
					ImGui::TextUnformatted(i.c_str()); //draw text
					if (has_color) ImGui::PopStyleColor(); //reset style for next text
					ImGui::PopTextWrapPos(); //reset text wrap settings

				}
				if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) ImGui::SetScrollHereY(1.0f); //setting auto scroll when at the bottom
			}
			ImGui::EndChild();

			ImGui::Separator();

			//to set if we are selecting textbox
			bool reclaim_focus = false;
			//flags for pressing enter, refocus, clearing textbox
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
			if (ImGui::InputText("Type stuff", &m_input, input_text_flags)) //inpux box
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

	void Editor::ShowObject(bool* Active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("objectlistwindow", Active)) // draw object list
		{
			ImGui::End(); //imgui close
		}
		else
		{
			//temporary here for counting created objects and add to name
			static int count = 0;
			if (ImGui::Button("Create Object")) // add a string into vector
			{
				AddConsole("Object Created");
				std::stringstream ss;
				ss << "new object " << count++;
				m_items.push_back(ss.str().c_str());
			}
			ImGui::SameLine(); // set the buttons on the same line
			if (ImGui::Button("Delete Object")) // delete a string from the vector
			{
				if (m_currentSelectedIndex <= m_items.size() && !m_items.empty())  // if vector not empty and item selected not over index
				{
					AddConsole("Object Deleted");
					std::stringstream ss;
					ss << "deleted object " << m_currentSelectedIndex;
					m_items.erase(m_items.begin() + m_currentSelectedIndex);

					//if not first index
					m_currentSelectedIndex != 1 ? m_currentSelectedIndex -= 1 : m_currentSelectedIndex = 0;

					//if object selected
					m_currentSelectedIndex > -1 ? m_objectIsSelected = true : m_objectIsSelected = false;
					
					if (m_items.empty()) m_currentSelectedIndex = -1;//if nothing selected
						
					count--;
				}
			}

			//disabled for now
			ImGui::BeginDisabled(true);
			if (ImGui::Button("Clone Object")){}
			ImGui::EndDisabled();

			ImGui::Separator();

			//loop to show all the items ins the vector
			if (ImGui::BeginChild("GameObjectList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {
				for (int n = 0; n < m_items.size(); n++)
				{
					const bool is_selected = (m_currentSelectedIndex == n);

					if (ImGui::Selectable(m_items[n].c_str(), is_selected)) //imgui selectable is the function to make the clickable bar of text
						m_currentSelectedIndex = n; //seteting current index to check for selection
					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected) // to show the highlight if selected
						ImGui::SetItemDefaultFocus();

					m_currentSelectedIndex > -1 ? m_objectIsSelected = true : m_objectIsSelected = false;
				}
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}

	void Editor::ShowDebugTests(bool* Active)
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
				std::stringstream ss;
				ss << "AllocationTest" << allocated;
				char* allocationtest = (char*)MemoryManager::GetInstance()->AllocateMemory(ss.str(), 30);
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
				//writing 8 bytes into the 7 i allocated
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
				AudioManager::GetInstance().PlaySound("sound1");
			}
			ImGui::SameLine();
			if (ImGui::Button("Play Audio 2"))
			{
				AudioManager::GetInstance().PlaySound("sound2");
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop Audio"))
			{
				AudioManager::GetInstance().StopAllSounds();
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space

			ImGui::Separator();
			ImGui::Text("Physics Test");
			if (ImGui::Button("Physics Scene 1"))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button("Physics Scene 2"))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button("Physics Scene 3"))
			{

			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space

			ImGui::Separator();
			ImGui::Text("Object Test");
			if (ImGui::Button("Object Scene 1"))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button("Object Scene 2"))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button("Object Scene 3"))
			{

			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space
			ImGui::Separator();
			ImGui::Text("Other Test");
			if (ImGui::Button("Crash Log"))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button("Performance Viewer"))
			{
				m_showPerformanceWindow = !m_showPerformanceWindow;
			}
			ImGui::SameLine();
			if (ImGui::Button("Object Scene 3"))
			{

			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space
			ImGui::End();
		}
	}

	void Editor::ShowComponentWindow(bool* Active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("componentwindow", Active))
		{
			ImGui::End();
		}
		else
		{
			if (ImGui::BeginChild("GameObjectList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {
				if (m_objectIsSelected)
				{
					//get index with m_currentSelectedIndex
					//loop through selected object's component
					//testing 3 components
						//maybe for renderer component checkbox to render or not
					for (int n = 0; n < 4; n++)
					{
						std::stringstream ss;
						ss << "headertest" << n;
						//testing ui
						if (ImGui::CollapsingHeader(ss.str().c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
						{ 	
						//need a switch case here to check what type of component

						//in each component need to confirm what are the different variables that i can reference and change

						//such as transform, i can change x, y, scale, rotation
						
										//////////////
										//testing ui//
										//////////////
							ImGui::Dummy(ImVec2(0.0f, 10.0f));
							static float f1 = 0.123f, f2 = 0.0f;
							ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
							ImGui::SliderFloat("slider float (log)", &f2, -10.0f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
							ImGui::Separator();

							static int i0 = 123;
							ImGui::InputInt("input int" , &i0);
							ImGui::Separator();

							static float f0 = 0.001f;
							ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");
							ImGui::Separator();

							static int i1 = 50, i2 = 42;
							ImGui::DragInt("drag int" , &i1, 1);
							ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);

							static float f3 = 1.00f, f4 = 0.0067f;
							ImGui::DragFloat("drag float" , &f3, 0.005f);
							ImGui::DragFloat("drag small float", &f4, 0.0001f, 0.0f, 0.0f, "%.06f ns");


							ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space
						}
					}

				}
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}

	void Editor::ShowResourceWindow(bool* Active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(150, 100), ImGuiCond_FirstUseEver);
		//testing for drag and drop
		if (!ImGui::Begin("resourcewindow", Active)) // draw resource list
		{
			ImGui::End(); //imgui close
		}
		else 
		{
			static int draggedItemIndex = -1;
			static bool isDragging = false;
			if (ImGui::BeginChild("resource list", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {
				for (int n = 0; n < 9; n++) // loop through resource list here
				{//resource list needs a list of icons for the texture for the image if possible
					//else just give a standard object icon here
						if (n % 3) // to keep it in rows where 3 is max 3 colums
						ImGui::SameLine();				
						ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)n), ImVec2(30, 20)); //child to hold image n text
						//ImGui::Image(itemTextures[i], ImVec2(20, 20)); image of resource
						ImGui::Text("test"); // text
						// Check if the mouse is over the content item
						if (ImGui::IsItemHovered()) {
							// Handle item clicks and drags
							if (ImGui::IsMouseClicked(0)) {
								draggedItemIndex = n; // Start dragging
								isDragging = true;
							}
						}
						ImGui::EndChild();
				}
			}
			ImGui::EndChild();

			if (isDragging) 
			{
				if (draggedItemIndex >= 0)
				{
					// Create a floating preview of the dragged item
					ImGui::SetNextWindowPos(ImGui::GetMousePos());
					ImGui::SetNextWindowSize(ImVec2(50, 50));
					std::string test = std::to_string(draggedItemIndex);
					ImGui::Begin(test.c_str(), nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
					//put image here
					ImGui::End();

					// Check if the mouse button is released
					if (!ImGui::IsMouseDown(0)) {
						isDragging = false;
						draggedItemIndex = -1;

						//do a function call here
					}
				}
			}


			ImGui::End(); //imgui close
		}


	}

	void Editor::ShowPerformanceWindow(bool* Active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
		//testing for drag and drop
		if (!ImGui::Begin("performanceWindow", Active, ImGuiWindowFlags_AlwaysAutoResize)) // draw resource list
		{
			ImGui::End(); //imgui close
		}
		else
		{
			// HARD CODED
			std::vector<float> values{
				TimeManager::GetInstance().GetSystemFrameTime(0) / TimeManager::GetInstance().GetFrameTime(),
				TimeManager::GetInstance().GetSystemFrameTime(1) / TimeManager::GetInstance().GetFrameTime(),
				TimeManager::GetInstance().GetSystemFrameTime(2) / TimeManager::GetInstance().GetFrameTime()
				//TimeManager::GetInstance().GetSystemFrameTime(3) / TimeManager::GetInstance().GetFrameTime()
			};
			char* names[] = { /*"Logics",*/ "Physics", "Collision", "Graphics" };
			ImGui::PlotHistogram("##Test",values.data(), values.size(), 0, NULL, 0.0f, 1.0f, ImVec2(200, 80.0f));
			
			if (ImGui::IsItemHovered())
			{
				//current mouse position - the top left position of the rect to get your actual mouse
				float MousePositionX = ImGui::GetIO().MousePos.x - ImGui::GetItemRectMin().x;
				//so your mouseposition/ rect length * number of values to get your current index
				int hoveredIndex = static_cast<int>(MousePositionX / ImGui::GetItemRectSize().x * values.size());

				if (hoveredIndex > -1 && hoveredIndex < values.size())
				{
					ImGui::BeginTooltip();
					ImGui::Text("%s: %.2f%%",names[hoveredIndex],values[hoveredIndex] * 100);
					ImGui::EndTooltip();
				}
			}
			
			
			ImGui::End(); //imgui close
		}


	}


	void Editor::SetDockingPort(bool* Active)
	{
		//initializing dockspace flags
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		//setting centralnode flag
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;

		//initializing window flags
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		//if passthru central node
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		//get the current viewport so that i can get the full size of the window
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		//making sure the dockspace will cover the entire window
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		//setting more window flags
		window_flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		//begin of dockspace
		if (!ImGui::Begin("DockSpace", Active, window_flags))
		{
			//if not active, just some weird imgui syntax
			ImGui::End();
		}
		else {
			//get io of imgui to ediit
			ImGuiIO& io = ImGui::GetIO();
			//get the id so that we can set the port node
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			//if docking is enabled
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				//initializing dockspace
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
					ImGui::DockBuilderDockWindow("sceneview", dockspace_id);

					//set the other sides
					ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.3f, nullptr, &dockspace_id);
					ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, &dockspace_id, &dockspace_id);
					ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, &dockspace_id, &dockspace_id);
					
					//setting the other dock locations
					ImGui::DockBuilderDockWindow("objectlistwindow", dock_id_right);

					//set on the save location to dock ontop of eachother
					ImGui::DockBuilderDockWindow("consolewindow", dock_id_down);
					
					//set on the save location to dock ontop of eachother
					ImGui::DockBuilderDockWindow("componentwindow", dock_id_left);

					//split the bottom into 2
					ImGuiID dock_id_down2 = ImGui::DockBuilderSplitNode(dock_id_down, ImGuiDir_Right, 0.5f, nullptr, &dock_id_down);

					ImGui::DockBuilderDockWindow("debugwindow", dock_id_down2);

					//end dock
					ImGui::DockBuilderFinish(dockspace_id);

				}

			}

			//docking port menu bar
			if (ImGui::BeginMainMenuBar())
			{
				//mennu 1
				if (ImGui::BeginMenu("Scenes"))
				{
					if (ImGui::MenuItem("Scene 1", "")) {} // have different graphical test here
					if (ImGui::MenuItem("Scene 2", "")) {} // set different booleans to controls as well
					if (ImGui::MenuItem("Scene 3", "")) {} // like maybe 1 for player controller
					// 1 for rendering 2500 objects
					// 1 for creation and deletion of objects
					//depending on different type of scene, i can activated and deactivate specific window so he cant break stuff as well
					ImGui::EndMenu();
				}
				//menu 2
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
				//menu 3
				if (ImGui::BeginMenu("Window"))
				{
					if (ImGui::MenuItem("console", "f1", m_showConsole, !m_showConsole))
					{
						m_showConsole = !m_showConsole;
					}
					if (ImGui::MenuItem("object list", "f2", m_showObjectList, !m_showObjectList))
					{
						m_showObjectList = !m_showObjectList;
					}
					if (ImGui::MenuItem("logs", "f3", m_showLogs, !m_showLogs))
					{
						m_showLogs = !m_showLogs;
					}
					if (ImGui::MenuItem("Sceneview", "f4", m_showSceneView, !m_showSceneView))
					{
						m_showSceneView = !m_showSceneView;
					}
					if (ImGui::MenuItem("Rubrics Test", "f5", m_showTestWindows, !m_showTestWindows))
					{
						m_showTestWindows = !m_showTestWindows;
					}
					if (ImGui::MenuItem("ResourceList", "f6", m_showResourceWindow, !m_showResourceWindow))
					{
						m_showResourceWindow = !m_showResourceWindow;
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Close Editor", "esc", m_showEditor, true))
					{
						ToggleEditor();
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndMainMenuBar(); // closing of menu begin function
			ImGui::End(); //finish drawing
		}

	}

	void Editor::ShowSceneView(GLuint texture_id, bool* active)
	{
		//set default size of the window
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
		
		//start the window
		ImGui::Begin("sceneview", active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

		//setting the current width and height of the window to be drawn on
		m_renderWindowWidth = ImGui::GetContentRegionAvail().x;
		m_renderWindowHeight = ImGui::GetContentRegionAvail().y;

		if (ImGui::IsMouseClicked(0))
		{
			//where i need to start doing the screen picking
			//get the mouse position relative to the top - left corner of the ImGui window.
			ImVec2 cursorToMainWindow = ImGui::GetCursorScreenPos(); // get current window position (top left corner)
			ImVec2 CurrentWindowPosition = ImGui::GetWindowPos(); // seems to get the same thing
			ImVec2 CursorToImGuiWindow = ImGui::GetMousePos();  // get mouse position but relative to your screen
			ImVec2 windowSize = ImGui::GetWindowSize();

			double glfwMouseX, glfwMouseY;
			glfwGetCursorPos(p_window, &glfwMouseX, &glfwMouseY); //glfw position

			std::cout << "[Get current window top left position w title] x screen: " << cursorToMainWindow[0] << " y screen: " << cursorToMainWindow[1] << std::endl;
			std::cout << "[Get Mouse Pos] x : " << CursorToImGuiWindow[0] << " y : " << CursorToImGuiWindow[1] << std::endl;
			std::cout << "[Get Current Window View Top left position] x : " << CurrentWindowPosition[0] << " y i: " << CurrentWindowPosition[1] << std::endl;
			//this tells you mouse position relative to imgui window seems the most useful for now
			std::cout << "[Gui mouse pos - cursorscreen pos] x:" << CursorToImGuiWindow[0] - cursorToMainWindow[0] << " y: " << CursorToImGuiWindow[1] - cursorToMainWindow[1] << std::endl;
			std::cout << "[GLFW] x:" << glfwMouseX << " y: " << glfwMouseY << std::endl;	
		}

		//the graphics rendered onto an image on the imgui window
		ImGui::Image(
			reinterpret_cast<void*>(
				static_cast<intptr_t>(texture_id)),
			ImVec2(m_renderWindowWidth, m_renderWindowHeight),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		//end the window
		ImGui::End();
	}

	void Editor::AddLog(std::string text)
	{
		m_logOutput.push_back(text);
	}

	void Editor::AddConsole(std::string text)
	{
		m_consoleOutput.push_back(text);
	}

	void Editor::AddError(std::string text)
	{
		std::stringstream ss;
		ss << "[ERROR] " << text;
		m_consoleOutput.push_back(ss.str());

	}

	void Editor::ClearLog()
	{
		m_logOutput.clear();
	}

	void Editor::ClearConsole()
	{
		m_consoleOutput.clear();
	}

	void Editor::OnKeyPressedEvent(const temp::Event<temp::KeyEvents>& e)
	{
		temp::KeyPressedEvent KPE;

		//dynamic cast
		if (e.GetType() == temp::KeyEvents::KeyPressed)
		{
			KPE = dynamic_cast<const temp::KeyPressedEvent&>(e);
		}

		//may want to change this to switch case to look cleaner

		if (KPE.keycode == GLFW_KEY_F1)
			m_showConsole = !m_showConsole;

		if (KPE.keycode == GLFW_KEY_F2)
			m_showObjectList = !m_showObjectList;

		if (KPE.keycode == GLFW_KEY_F3)
			m_showLogs = !m_showLogs;

		if (KPE.keycode == GLFW_KEY_F4)
			m_showSceneView = !m_showSceneView;

		if (KPE.keycode == GLFW_KEY_F5)
			m_showTestWindows = !m_showTestWindows;

		if (KPE.keycode == GLFW_KEY_ESCAPE)
			ToggleEditor();

		if (KPE.keycode == GLFW_KEY_F6)
			m_showResourceWindow = !m_showResourceWindow;
	}

	void Editor::OnMousePressedEvent(const temp::Event<temp::MouseEvents>& e)
	{
		temp::MouseButtonPressedEvent MBPE;
		if (e.GetType() == temp::MouseEvents::MouseButtonPressed)
		{
			MBPE = dynamic_cast<const temp::MouseButtonPressedEvent&>(e);
		}

		//oops i named the window the same as theirs
		//::Editor* currentWindow = ImGui::GetCurrentWindow();
		

		
	}

}

