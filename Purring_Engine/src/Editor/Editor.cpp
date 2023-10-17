/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Editor.cpp
 \date     30-08-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	cpp file containing the definition of the editor class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "Editor.h"
#include "Memory/MemoryManager.h"
#include "AudioManager/AudioManager.h"
#include "Time/TimeManager.h"
#include "ResourceManager/ResourceManager.h"
#include <Windows.h>
#include <Commdlg.h>
#include "Data/SerializationManager.h"
#include "Physics/PhysicsManager.h"
#include "Logging/Logger.h"
#include "Logic/LogicSystem.h"
#include "Graphics/RendererManager.h"
#include "Logic/testScript.h"
#include "Logic/PlayerControllerScript.h"
#include <random>
# define M_PI           3.14159265358979323846 // temp definition of pi, will need to discuss where shld we leave this later on
SerializationManager serializationManager;  // Create an instance

extern Logger engine_logger;

namespace PE {
	Editor::Editor() {
		//initializing variables 
		//m_firstLaunch needs to be serialized 
		m_firstLaunch = true;
		//serialize based on what was deserialized
		m_showConsole = true;
		m_showLogs = true;
		m_showObjectList = true;
		m_showSceneView = true;
		m_showTestWindows = true;
		m_showComponentWindow = true;
		m_showResourceWindow = false;
		m_showPerformanceWindow = false;
		//show the entire gui 
		m_showEditor = true; // depends on the mode, whether we want to see the scene or the editor
		m_renderDebug = true; // whether to render debug lines
		//Subscribe to key pressed event 
		ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, Editor::OnKeyTriggeredEvent, this)
			//for the object list
			m_objectIsSelected = false;
		m_currentSelectedObject = 0;
		m_mouseInScene = false;
		//mapping commands to function calls
		m_commands.insert(std::pair<std::string_view, void(PE::Editor::*)()>("test", &PE::Editor::test));
		m_commands.insert(std::pair<std::string_view, void(PE::Editor::*)()>("ping", &PE::Editor::ping));
	}

	Editor::~Editor()
	{
	}

	void Editor::GetWindowSize(float& width, float& height)
	{
		width = m_renderWindowWidth;
		height = m_renderWindowHeight;
	}

	bool Editor::IsEditorActive()
	{
		return m_showEditor;
	}

	bool Editor::IsRenderingDebug()
	{
		return m_renderDebug;
	}

	bool Editor::IsRunTime()
	{
		return m_isRunTime;
	}

	bool Editor::IsMouseInScene()
	{
		return m_mouseInScene;
	}

	void Editor::ToggleDebugRender()
	{
		if (m_renderDebug)
		{
			AddInfoLog("Turning Debug lines off");
		}
		else
		{
			AddInfoLog("Turning Debug lines on");
		}
		m_renderDebug = !m_renderDebug;
	}

	void Editor::ping()
	{
		AddConsole("pong");
	}

	void Editor::test()
	{
		m_showTestWindows = true;
	}

	void Editor::ClearObjectList()
	{
		//make sure not hovering any objects as we are deleting
		m_currentSelectedObject = -1;
		//delete all objects
		for (size_t n = EntityManager::GetInstance().GetEntitiesInPool(ALL).size() - 1; n > 1; --n)
		{
			EntityManager::GetInstance().RemoveEntity(EntityManager::GetInstance().GetEntitiesInPool(ALL)[n]);
		}
	}

	void Editor::Init(GLFWwindow* p_window_)
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


		p_window = p_window_;
		//getting the full display size of glfw so that the ui know where to be in
		int width, height;
		glfwGetWindowSize(p_window, &width, &height);
		io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
		m_renderWindowWidth = static_cast<float>(width) * 0.1f;
		m_renderWindowHeight = static_cast<float>(height) * 0.1f;

		//looks nicer 
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//init imgui for glfw and opengl 
		ImGui_ImplGlfw_InitForOpenGL(p_window, true);

		ImGui_ImplOpenGL3_Init("#version 450");

	}

	void Editor::Render(GLuint texture_id)
	{
		//imgui start frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//hide the entire editor
		if (m_showEditor)
		{
			//show docking port
			SetDockingPort(&m_showEditor);

			//render logs
			if (m_showLogs) ShowLogsWindow(&m_showLogs);

			//render object list
			if (m_showObjectList) ShowObjectWindow(&m_showObjectList);

			//the components on the object
			if (m_showComponentWindow) ShowComponentWindow(&m_showComponentWindow);

			//render console
			if (m_showConsole) ShowConsoleWindow(&m_showConsole);

			//draw scene view
			if (m_showSceneView) ShowSceneView(texture_id, &m_showSceneView);

			//draw the stuff for ellie to test
			if (m_showTestWindows) ShowDemoWindow(&m_showTestWindows);

			//resource window for drag n drop
			if (m_showResourceWindow) ShowResourceWindow(&m_showResourceWindow);

			//performance window showing time used per system
			if (m_showPerformanceWindow) ShowPerformanceWindow(&m_showPerformanceWindow);
		}

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

	void Editor::ShowLogsWindow(bool* Active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
		//if active
		if (!ImGui::Begin("logwindow", Active))
		{
			ImGui::End();			//imgui syntax
		}
		else
		{
			//temp for now dk where to save this later
			static int startPrint = 0;
			if (ImGui::Button("Clear Logs")) // button
			{
				AddLog("Logs Cleared");
				startPrint = static_cast<int>(m_logOutput.size()) - 1;
			}
			ImGui::SameLine();

			//temp for now
			static bool warningfilter;
			static bool eventfilter;
			static bool infofilter;
			static bool errorfilter;
			static bool otherfilter;

			if (ImGui::BeginPopup("Filters"))
			{
				ImGui::Checkbox("Warning", &warningfilter);
				ImGui::Checkbox("Event", &eventfilter);
				ImGui::Checkbox("Info", &infofilter);
				ImGui::Checkbox("Error", &errorfilter);
				ImGui::Checkbox("Others", &otherfilter);
				ImGui::EndPopup();
			}

			if (ImGui::Button("Filters"))
				ImGui::OpenPopup("Filters");

			ImGui::SameLine();
			ImGui::PushItemWidth(200); // set a fixed width
			ImGui::InputText("Text Finder ", &m_findText); //inpux box
			ImGui::SetItemTooltip("(Case Sensitive)");
			ImGui::PopItemWidth();
			ImGui::Separator(); // line


			//show the text in logs
			if (ImGui::BeginChild("log scroll area", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar))
			{

				ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x); //set text wrap
				for (std::vector<std::string>::iterator i = m_logOutput.begin() + startPrint; i != m_logOutput.end(); i++)
				{
					if (m_findText != "")
					{
						//can make this case insensitive but i dont know how expensive it will be
						//also since max vector size hasnt been implemented itll definitely cause lag
						if (i->find(m_findText) == std::string::npos) continue;
					}
					ImVec4 color;
					bool has_color = false;
					//check for specific words to color and filter
					if (i->find("[ERROR]") != std::string::npos)
					{
						if (errorfilter)
							continue;
						color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true;
					}
					else if (i->find("[INFO]") != std::string::npos)
					{
						if (infofilter)
							continue;
						color = ImVec4(0.4f, 1.f, 1.f, 1.0f); has_color = true;
					}
					else if (i->find("[WARNING]") != std::string::npos)
					{
						if (warningfilter)
							continue;
						color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); has_color = true;
					}
					else if (i->find("[EVENT]") != std::string::npos)
					{
						if (eventfilter)
							continue;
						color = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); has_color = true;
					}
					else if (otherfilter)
						continue;
					if (has_color) ImGui::PushStyleColor(ImGuiCol_Text, color); // set color style
					ImGui::TextUnformatted(i->c_str()); //draw text
					if (has_color) ImGui::PopStyleColor(); //reset style for next text
				} // loop all the text
				ImGui::PopTextWrapPos(); // close text wrap
				if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) ImGui::SetScrollHereY(1.0f); //set auto scroll when at bottom
			}
			ImGui::EndChild(); // end child area
			ImGui::End(); //end drawing
		}
	}

	void Editor::ShowConsoleWindow(bool* Active)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("consolewindow", Active)) // start drawing
		{
			ImGui::End(); //imgui syntax if inactive
		}
		else {
			if (ImGui::Button("Clear Console")) //button
			{
				ClearConsole();
				AddConsole("Console Cleared");
			}
			ImGui::SameLine();
			if (ImGui::Button("Help"))
			{
				AddConsole("///////////////////////////");
				AddConsole("        Command List       ");
				AddConsole("ping: pong");
				AddConsole("test: to show test window");
				AddConsole("///////////////////////////");
			}
			ImGui::Separator(); // draw a line

			//frame height w spacing is the size of an element, item spacing is the spacing between objects
			float const spacing = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); //get enough spacing for a text bar at the bottom

			if (ImGui::BeginChild("console scroll area", ImVec2(0, -spacing), true, ImGuiWindowFlags_HorizontalScrollbar)) //start drawing child
			{
				ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x); //text wrap
				for (std::string i : m_consoleOutput) { ImGui::Text(i.c_str()); }
				ImGui::PopTextWrapPos(); //reset text wrap settings
				if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) ImGui::SetScrollHereY(1.0f); //setting auto scroll when at the bottom
			}
			ImGui::EndChild();

			ImGui::Separator();

			//to set if we are selecting textbox
			bool reclaim_focus = false;
			//flags for pressing enter, refocus, clearing textbox
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
			if (ImGui::InputText("Enter", &m_input, input_text_flags)) //inpux box
			{
				AddConsole(m_input);
				//can serialize these command into an array

				if (m_commands.find(m_input) != m_commands.end())
				{
					(this->*(m_commands[m_input]))();
				}
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

	void Editor::ShowObjectWindow(bool* Active)
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
				AddInfoLog("Object Created");
				serializationManager.LoadFromFile("../Assets/Prefabs/Render_Prefab.json");
			}
			ImGui::SameLine(); // set the buttons on the same line
			if (ImGui::Button("Delete Object")) // delete a string from the vector
			{
				if (m_currentSelectedObject > 1)  // if vector not empty and item selected not over index
				{
					AddInfoLog("Object Deleted");


					EntityManager::GetInstance().RemoveEntity(EntityManager::GetInstance().GetEntitiesInPool(ALL)[m_currentSelectedObject]);

					//if not first index
					m_currentSelectedObject != 1 ? m_currentSelectedObject -= 1 : m_currentSelectedObject = 0;

					//if object selected
					m_currentSelectedObject > -1 ? m_objectIsSelected = true : m_objectIsSelected = false;

					if (EntityManager::GetInstance().GetEntitiesInPool(ALL).empty()) m_currentSelectedObject = -1;//if nothing selected

					count--;

				}
				else
				{
					AddWarningLog("You are not allowed to delete the background or player as of now");
				}
			}

			if (ImGui::Button("Clone Object"))
			{
				if (m_currentSelectedObject)
					EntityFactory::GetInstance().Clone(EntityManager::GetInstance().GetEntitiesInPool(ALL)[m_currentSelectedObject]);
				else
					AddWarningLog("You are not allowed to clone the background");
			}

			ImGui::Separator();

			//loop to show all the items ins the vector
			if (ImGui::BeginChild("GameObjectList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {
				for (int n = 0; n < EntityManager::GetInstance().GetEntitiesInPool(ALL).size(); n++)
				{
					std::string name;
					const bool is_selected = (m_currentSelectedObject == n);
					if (n == 0) //hardcoding
					{
						name = "Background";
					}
					else if (n == 1)
					{
						name = "Player";
					}
					else {
						name = "GameObject";
						name += std::to_string(EntityManager::GetInstance().GetEntitiesInPool(ALL)[n]);
					}
					if (ImGui::Selectable(name.c_str(), is_selected)) //imgui selectable is the function to make the clickable bar of text
						m_currentSelectedObject = n; //seteting current index to check for selection
					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected) // to show the highlight if selected
						ImGui::SetItemDefaultFocus();

					m_currentSelectedObject > -1 ? m_objectIsSelected = true : m_objectIsSelected = false;
				}
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}

	//temporary for milestone 1
	void Editor::ShowDemoWindow(bool* Active)
	{
		if (!ImGui::Begin("debugTests", Active, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
		}
		else
		{
			static int allocated = 1;
			ImGui::Text("Memory Test");
			ImGui::Spacing();
			if (ImGui::Button("Allocating Memory"))
			{
				std::string ss("AllocationTest");
				ss += std::to_string(allocated);
				char* allocationtest = (char*)MemoryManager::GetInstance().AllocateMemory(ss, 30);
				allocated++;
				allocationtest;
			}
			ImGui::SameLine();
			if (ImGui::Button("Create Out of Index Object on Stack"))
			{
				char* outofmemorytest = (char*)MemoryManager::GetInstance().AllocateMemory("out of index test", 1000000);
				outofmemorytest;
			}
			static bool buffertester = false;
			ImGui::BeginDisabled(buffertester);
			if (ImGui::Button("Written over Buffer Tests"))
			{
				buffertester = true;
				char* buffertest = (char*)MemoryManager::GetInstance().AllocateMemory("buffertest", 7);
				//writing 8 bytes into the 7 i allocated
				strcpy_s(buffertest, 9, "testtest");
				AddWarningLog("writing \"testtest\" 9 byte into buffertest of 7 byte + 4 buffer bytes");
				AddConsole(buffertest);
				allocated++;
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::BeginDisabled(!(allocated > 1));
			if (ImGui::Button("popback previous allocation"))
			{
				buffertester = false;
				MemoryManager::GetInstance().Pop_BackMemory();
				AddInfoLog("delete and popping back previously allocated object");
				allocated--;
			}
			ImGui::EndDisabled();
			if (ImGui::Button("Print Memory Data"))
			{
				MemoryManager::GetInstance().PrintData();
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space

			ImGui::Separator();
			//audio
			ImGui::Text("Audio Test");
			if (ImGui::Button("Play Audio 1"))
			{
				AudioManager::GetInstance().PlaySound("audio_sound1");
			}
			ImGui::SameLine();
			if (ImGui::Button("Play Audio 2"))
			{
				AudioManager::GetInstance().PlaySound("audio_sound2");
			}
			ImGui::SameLine();
			if (ImGui::Button("Play Audio 3"))
			{
				AudioManager::GetInstance().PlaySound("audio_sound2");
			}
			if (ImGui::Button("Play BGM"))
			{
				AudioManager::GetInstance().PlaySound("audio_backgroundMusic");
			}
			ImGui::SameLine();

			if (ImGui::Button("Stop Audio"))
			{
				AudioManager::GetInstance().StopAllSounds();
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // add space

			ImGui::Separator();
			ImGui::Text("Physics Test");
			if (ImGui::Button("Random Object Test"))
			{
				ClearObjectList();
				std::random_device rd;
				std::mt19937 gen(rd());
				for (size_t i{ 2 }; i < 20; ++i)
				{
					EntityID id = EntityFactory::GetInstance().CreateFromPrefab("GameObject");

					std::uniform_int_distribution<>distr0(-550, 550);
					EntityManager::GetInstance().Get<Transform>(id).position.x = static_cast<float>(distr0(gen));
					std::uniform_int_distribution<>distr1(-250, 250);
					EntityManager::GetInstance().Get<Transform>(id).position.y = static_cast<float>(distr1(gen));
					std::uniform_int_distribution<>distr2(10, 200);
					EntityManager::GetInstance().Get<Transform>(id).width = static_cast<float>(distr2(gen));
					EntityManager::GetInstance().Get<Transform>(id).height = static_cast<float>(distr2(gen));
					EntityManager::GetInstance().Get<Transform>(id).orientation = 0.f;

					if (i % 3)
						EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);

					if (i % 2)
						EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider();
					else
						EntityManager::GetInstance().Get<Collider>(id).colliderVariant = AABBCollider();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Object List##1"))
			{
				ClearObjectList();
			}
			if (ImGui::Button("AABB AABB DYNAMIC STATIC"))
			{
				ClearObjectList();
				EntityManager::GetInstance().Get<Transform>(1).position.x = 0;
				EntityManager::GetInstance().Get<Transform>(1).position.y = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.x = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.y = 0;
				EntityManager::GetInstance().Get<Collider>(1).colliderVariant = AABBCollider();

				EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/AABBCollider_Prefab.json");
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::STATIC);
			}
			ImGui::SameLine();
			if (ImGui::Button("AABB AABB DYNAMIC DYNAMIC"))
			{
				ClearObjectList();
				EntityManager::GetInstance().Get<Transform>(1).position.x = 0;
				EntityManager::GetInstance().Get<Transform>(1).position.y = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.x = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.y = 0;
				EntityManager::GetInstance().Get<Collider>(1).colliderVariant = AABBCollider();

				EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/AABBCollider_Prefab.json");
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
			}
			if (ImGui::Button("CIRCLE CIRCLE DYNAMIC STATIC"))
			{
				ClearObjectList();
				EntityManager::GetInstance().Get<Transform>(1).position.x = 0;
				EntityManager::GetInstance().Get<Transform>(1).position.y = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.x = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.y = 0;
				EntityManager::GetInstance().Get<Collider>(1).colliderVariant = CircleCollider();

				EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/CircleCollider_Prefab.json");
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::STATIC);
			}
			ImGui::SameLine();
			if (ImGui::Button("CIRCLE CIRCLE DYNAMIC DYNAMIC"))
			{
				ClearObjectList();
				EntityManager::GetInstance().Get<Transform>(1).position.x = 0;
				EntityManager::GetInstance().Get<Transform>(1).position.y = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.x = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.y = 0;
				EntityManager::GetInstance().Get<Collider>(1).colliderVariant = CircleCollider();

				EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/CircleCollider_Prefab.json");
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
			}
			if (ImGui::Button("AABB CIRCLE DYNAMIC STATIC"))
			{
				ClearObjectList();
				EntityManager::GetInstance().Get<Transform>(1).position.x = 0;
				EntityManager::GetInstance().Get<Transform>(1).position.y = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.x = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.y = 0;
				EntityManager::GetInstance().Get<Collider>(1).colliderVariant = AABBCollider();

				EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/CircleCollider_Prefab.json");
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::STATIC);
			}
			ImGui::SameLine();
			if (ImGui::Button("AABB CIRCLE DYNAMIC DYNAMIC"))
			{
				ClearObjectList();
				EntityManager::GetInstance().Get<Transform>(1).position.x = 0;
				EntityManager::GetInstance().Get<Transform>(1).position.y = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.x = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.y = 0;
				EntityManager::GetInstance().Get<Collider>(1).colliderVariant = AABBCollider();

				EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/CircleCollider_Prefab.json");
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
			}
			if (ImGui::Button("CIRCLE AABB DYNAMIC STATIC"))
			{
				ClearObjectList();
				EntityManager::GetInstance().Get<Transform>(1).position.x = 0;
				EntityManager::GetInstance().Get<Transform>(1).position.y = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.x = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.y = 0;
				EntityManager::GetInstance().Get<Collider>(1).colliderVariant = CircleCollider();

				EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/AABBCollider_Prefab.json");
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::STATIC);
			}
			ImGui::SameLine();
			if (ImGui::Button("CIRCLE AABB DYNAMIC DYNAMIC"))
			{
				ClearObjectList();
				EntityManager::GetInstance().Get<Transform>(1).position.x = 0;
				EntityManager::GetInstance().Get<Transform>(1).position.y = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.x = 0;
				EntityManager::GetInstance().Get<RigidBody>(1).velocity.y = 0;
				EntityManager::GetInstance().Get<Collider>(1).colliderVariant = CircleCollider();

				EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/AABBCollider_Prefab.json");
				EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
			}
			if (ImGui::Button("Toggle Step Physics"))
			{
				PhysicsManager::GetStepPhysics() = !PhysicsManager::GetStepPhysics();

				if (PhysicsManager::GetStepPhysics())
					Editor::GetInstance().AddEventLog("Step-by-Step Physics Turned On.\n");
				else
					Editor::GetInstance().AddEventLog("Step-by-Step Physics Turned Off.\n");
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space

			ImGui::Separator();
			ImGui::Text("Object Test");
			if (ImGui::Button("Draw 2500 objects"))
			{
				ClearObjectList();
				for (size_t i{}; i < 2500; ++i)
				{
					EntityID id2 = EntityFactory::GetInstance().CreateEntity();
					EntityFactory::GetInstance().Assign(id2, { EntityManager::GetInstance().GetComponentID<Transform>(), EntityManager::GetInstance().GetComponentID<Graphics::Renderer>() });
					EntityManager::GetInstance().Get<Transform>(id2).position.x = 15.f * (i % 50) - 320.f;
					EntityManager::GetInstance().Get<Transform>(id2).position.y = 15.f * (i / 50) - 320.f;
					EntityManager::GetInstance().Get<Transform>(id2).width = 10.f;
					EntityManager::GetInstance().Get<Transform>(id2).height = 10.f;
					EntityManager::GetInstance().Get<Transform>(id2).orientation = 0.f;
					EntityManager::GetInstance().Get<Graphics::Renderer>(id2).SetColor(1.f, 1.f, 1.f, 1.f);

				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Toggle Debug Lines"))
			{
				ToggleDebugRender();
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Object List##2"))
			{
				ClearObjectList();
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space
			ImGui::Separator();
			ImGui::Text("Other Test");
			if (ImGui::Button("Crash Log"))
			{
				try
				{
					std::vector testVector = { 1 };
					testVector[0] = testVector.at(1); // force an out of range access exception
				}
				catch (const std::out_of_range& r_err)
				{
					engine_logger.AddLog(true, r_err.what(), __FUNCTION__);
					throw r_err; // pass the error along
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Performance Viewer"))
			{
				m_showPerformanceWindow = !m_showPerformanceWindow;
			}
			ImGui::SameLine();
			if (ImGui::Button("Add Debug Text"))
			{
				AddErrorLog("Debug text");
				AddInfoLog("Debug text");
				AddWarningLog("Debug text");
				AddEventLog("Debug text");
				AddLog("Debug text");
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
			if (ImGui::BeginChild("GameComponentList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar))
			{
				if (m_objectIsSelected)
				{
					EntityID entityID = EntityManager::GetInstance().GetEntitiesInPool(ALL)[m_currentSelectedObject];
					std::vector<ComponentID> components = EntityManager::GetInstance().GetComponentIDs(entityID);
					int componentCount = 0; //unique id for imgui objects
					bool hasScripts = false;
					for (const ComponentID& name : components)
					{
						++componentCount;//increment unique id
						ImGui::SetNextItemAllowOverlap(); // allow the stacking of buttons

						//search through each component, create a collapsible header if the component exist

						//transform component
						if (name == EntityManager::GetInstance().GetComponentID<Transform>())
						{
							//if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								//setting reset button to open a popup with selectable text
								ImGui::SameLine();
								std::string id = "options##", o = "o##";
								id += std::to_string(componentCount);
								o += std::to_string(componentCount);
								if (ImGui::BeginPopup(id.c_str()))
								{
									if (ImGui::Selectable("Reset")) {}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								//each variable in the component
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Text("Position: ");
								ImGui::Text("x: "); ImGui::SameLine(); ImGui::InputFloat("##x", &EntityManager::GetInstance().Get<Transform>(entityID).position.x, 1.0f, 100.f, "%.3f");								
								ImGui::Text("y: "); ImGui::SameLine(); ImGui::InputFloat("##y", &EntityManager::GetInstance().Get<Transform>(entityID).position.y, 1.0f, 100.f, "%.3f");
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Text("Scale: ");
								ImGui::Text("Width: "); ImGui::SameLine(); ImGui::InputFloat("##Width", &EntityManager::GetInstance().Get<Transform>(entityID).width, 1.0f, 100.f, "%.3f");
								ImGui::Text("Height: "); ImGui::SameLine(); ImGui::InputFloat("##Height", &EntityManager::GetInstance().Get<Transform>(entityID).height, 1.0f, 100.f, "%.3f");
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Text("Rotation: ");
								float rotation = static_cast<float>(EntityManager::GetInstance().Get<Transform>(entityID).orientation * (180 / M_PI));
								ImGui::Text("Orientation: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.f); ImGui::SliderFloat("##Orientation", &rotation, -180, 180, "%.3f");
								ImGui::Text("             "); ImGui::SameLine();  ImGui::SetNextItemWidth(200.f); ImGui::InputFloat("##Orientation2", &rotation, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
								ImGui::SetItemTooltip("In Radians");
								EntityManager::GetInstance().Get<Transform>(entityID).orientation = static_cast<float>(rotation * (M_PI / 180));
							}
						}

						//rigidbody component
						if (name == EntityManager::GetInstance().GetComponentID<RigidBody>())
						{
							if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								//setting reset button to open a popup with selectable text
								ImGui::SameLine();
								std::string id = "options##", o = "o##";
								id += std::to_string(componentCount);
								o += std::to_string(componentCount);
								if (ImGui::BeginPopup(id.c_str()))
								{
									if (ImGui::Selectable("Reset")) {}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								//open a combo box to select type of rb
								EnumRigidBodyType bt = EntityManager::GetInstance().Get<RigidBody>(entityID).GetType();
								int index = static_cast<int>(bt);
								//hard coded rigidbody types
								const char* types[] = { "STATIC","DYNAMIC" };
								ImGui::Text("Rigidbody Type: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.0f);
								//combo box of the different rigidbody types
								if (ImGui::Combo("##Rigidbody Type", &index, types, IM_ARRAYSIZE(types)))
								{
									//setting the rigidbody type when selected
									bt = static_cast<EnumRigidBodyType>(index);
									EntityManager::GetInstance().Get<RigidBody>(entityID).SetType(bt);
								}

								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Separator(); // add line to make it neater
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								//mass variable
								float mass = EntityManager::GetInstance().Get<RigidBody>(entityID).GetMass();
								ImGui::Text("Mass: "); ImGui::SameLine(); ImGui::InputFloat("##Mass", &mass, 1.0f, 100.f, "%.3f");
								EntityManager::GetInstance().Get<RigidBody>(entityID).SetMass(mass);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
							}
						}

						//collider component
						if (name == EntityManager::GetInstance().GetComponentID<Collider>())
						{
							//if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								//setting reset button to open a popup with selectable text
								ImGui::SameLine();
								std::string id = "options##", o = "o##";
								id += std::to_string(componentCount);
								o += std::to_string(componentCount);
								if (ImGui::BeginPopup(id.c_str()))
								{
									if (ImGui::Selectable("Reset")) {}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								//get the current collider type using the variant
								int index = static_cast<int>(EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant.index());
								//hardcoded collider types
								const char* types[] = { "AABB","CIRCLE" };
								ImGui::Text("Collider Type: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.0f);
								//set combo box for the different collider types
								if (ImGui::Combo("##Collider Types", &index, types, IM_ARRAYSIZE(types)))
								{
									//hardcode setting of variant using the current gotten index
									if (index)
									{
										EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant = CircleCollider();
									}
									else
									{
										EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant = AABBCollider();
									}
								}

								//depends on different collider, have different variables
								if (index)
								{
									ImVec2 offset;
									CircleCollider& r_cc = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant);

									offset.x = r_cc.positionOffset.x;
									offset.y = r_cc.positionOffset.y;

									//collider position
									ImGui::Text("Collider Position Offset: ");
									ImGui::Text("x pos offset: "); ImGui::SameLine(); ImGui::InputFloat("##xoffsetcircle", &offset.x, 1.0f, 100.f, "%.3f");
									ImGui::Text("y pos offset: "); ImGui::SameLine(); ImGui::InputFloat("##yoffsetcircle", &offset.y, 1.0f, 100.f, "%.3f");
									r_cc.positionOffset.y = offset.y;
									r_cc.positionOffset.x = offset.x;

									//colider scale from center
									float offset2 = r_cc.scaleOffset;
									ImGui::Text("Collider Scale Offset: ");
									ImGui::Text("sc x offset: "); ImGui::SameLine(); ImGui::InputFloat("##scaleOffsetcircle", &offset2, 1.0f, 100.f, "%.3f");

									r_cc.scaleOffset = std::abs(offset2);
								}
								else
								{
									ImVec2 offset;
									//aabb position offset
									offset.x = EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant._Storage()._Get().positionOffset.x;
									offset.y = EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant._Storage()._Get().positionOffset.y;
									ImGui::Text("Collider Position Offset: ");
									ImGui::Text("pos x offset: "); ImGui::SameLine(); ImGui::InputFloat("##xoffsetaabb", &offset.x, 1.0f, 100.f, "%.3f");
									ImGui::Text("pos y offset: "); ImGui::SameLine(); ImGui::InputFloat("##yoffsetaabb", &offset.y, 1.0f, 100.f, "%.3f");
									EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant._Storage()._Get().positionOffset.y = offset.y;
									EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant._Storage()._Get().positionOffset.x = offset.x;

									ImVec2 offset2;
									//aabb scale offset
									offset2.x = EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant._Storage()._Get().scaleOffset.x;
									offset2.y = EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant._Storage()._Get().scaleOffset.y;
									ImGui::Text("Collider Scale Offset: ");
									ImGui::Text("x scale offset: "); ImGui::SameLine(); ImGui::InputFloat("##xscaleOffsetaabb", &offset2.x, 1.0f, 100.f, "%.3f");
									ImGui::Text("y scale offset: "); ImGui::SameLine(); ImGui::InputFloat("##yscaleOffsetaabb", &offset2.y, 1.0f, 100.f, "%.3f");
									EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant._Storage()._Get().scaleOffset.y = std::abs(offset2.y);
									EntityManager::GetInstance().Get<Collider>(entityID).colliderVariant._Storage()._Get().scaleOffset.x = std::abs(offset2.x);
								}
								ImGui::Checkbox("Is Trigger", &EntityManager::GetInstance().Get<Collider>(m_currentSelectedObject).isTrigger);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
							}
						}

						//renderer component
						if (name == EntityManager::GetInstance().GetComponentID<Graphics::Renderer>())
						{
							//if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								//setting reset button to open a popup with selectable text
								ImGui::SameLine();
								std::string id = "options##", o = "o##";
								id += std::to_string(componentCount);
								o += std::to_string(componentCount);
								if (ImGui::BeginPopup(id.c_str()))
								{
									if (ImGui::Selectable("Reset")) {}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								//setting textures
								std::vector<const char*> key;
								key.push_back("");

								//to get all the keys
								for (std::map<std::string, std::shared_ptr<Graphics::Texture>>::iterator it = ResourceManager::GetInstance().Textures.begin(); it != ResourceManager::GetInstance().Textures.end(); ++it)
								{
									key.push_back(it->first.c_str());
								}
								int index{};
								for (std::string str : key)
								{
									if (str == EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetTextureKey())
										break;
									index++;
								}

								//create a combo box of texture ids
								ImGui::SetNextItemWidth(200.0f);
								if (!key.empty())
								{
									ImGui::Text("Textures: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									//set selected texture id
									if (ImGui::Combo("##Textures", &index, key.data(), static_cast<int>(key.size())))
									{
										EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).SetTextureKey(key[index]);
									}
								}
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								//setting colors

								//get and set color variable of the renderer component
								ImVec4 color;
								color.x = EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetColor().r;
								color.y = EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetColor().g;
								color.z = EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetColor().b;
								color.w = EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetColor().a;

								ImGui::Text("Change Color: "); ImGui::SameLine();
								ImGui::ColorEdit4("##Change Color", (float*)&color, ImGuiColorEditFlags_AlphaPreview);

								EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).SetColor(color.x, color.y, color.z, color.w);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
							}
						}

						//Script Component
						if (name == EntityManager::GetInstance().GetComponentID<ScriptComponent>())
						{
							hasScripts = true;
							if (ImGui::CollapsingHeader("ScriptComponent", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{							
								//setting reset button to open a popup with selectable text
								ImGui::SameLine();
								std::string id = "options##", o = "o##";
								id += std::to_string(componentCount);
								o += std::to_string(componentCount);
								if (ImGui::BeginPopup(id.c_str()))
								{
									if (ImGui::Selectable("Reset")) {}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								//setting textures
								std::vector<const char*> key;
								//to get all the keys
								for (std::map<std::string, Script*>::iterator it = LogicSystem::m_scriptContainer.begin(); it != LogicSystem::m_scriptContainer.end(); ++it)
								{
									key.push_back(it->first.c_str());
								}
								static int scriptindex{};
								//create a combo box of scripts
								ImGui::SetNextItemWidth(200.0f);
								if (!key.empty())
								{
									ImGui::Text("Scripts: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									//set selected texture id
									if (ImGui::Combo("##Scripts", &scriptindex, key.data(), static_cast<int>(key.size()))) {}
								}
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
								if (ImGui::Button("Add Script"))
								{
									EntityManager::GetInstance().Get<ScriptComponent>(entityID).addScript(key[scriptindex]);
									LogicSystem::m_scriptContainer[key[scriptindex]]->OnAttach(entityID);
								}
								ImGui::PopStyleColor(1);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								static int selectedScript{-1};
								static std::string selectedScriptName{};
								ImGui::Text("Scripts List");
								//loop to show all the items ins the vector
								if (ImGui::BeginChild("GameObjectList", ImVec2(-1, 200), true,  ImGuiWindowFlags_NoResize)) {
									for (int n = 0; n < EntityManager::GetInstance().Get<ScriptComponent>(entityID).m_scriptKeys.size(); n++)
									{
										const bool is_selected = (selectedScript == n);

										if (ImGui::Selectable(EntityManager::GetInstance().Get<ScriptComponent>(entityID).m_scriptKeys[n].c_str(), is_selected)) 
										{
											selectedScript = n; //seteting current index to check for selection
											selectedScriptName = EntityManager::GetInstance().Get<ScriptComponent>(entityID).m_scriptKeys[n].c_str();
										}//imgui selectable is the function to make the clickable bar of text
											
										// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
										if (is_selected) // to show the highlight if selected
											ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndChild();
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));
								if (ImGui::Button("Remove Script"))
								{
									if (selectedScript >= 0) 
									{
										EntityManager::GetInstance().Get<ScriptComponent>(entityID).removeScript(selectedScript);
										LogicSystem::m_scriptContainer[selectedScriptName]->OnDetach(entityID);
										selectedScript = -1;
									}
								}
								ImGui::PopStyleColor(1);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
							}
						}
                        
						if (hasScripts)
						{
							for (auto& [key, val] : LogicSystem::m_scriptContainer)
							{
								if (key == "test")
								{
									testScript* test = dynamic_cast<testScript*>(val);
									auto it = test->GetScriptData().find(m_currentSelectedObject);
									if(it != test->GetScriptData().end())
									if (ImGui::CollapsingHeader("testdata", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										ImGui::Text("rot speed: "); ImGui::SameLine(); ImGui::InputFloat("##rspeed", &it->second.m_rotationSpeed, 1.0f, 100.f, "%.3f");
									}
								}

								if (key == "PlayerControllerScript")
								{
									PlayerControllerScript* test = dynamic_cast<PlayerControllerScript*>(val);
									auto it = test->GetScriptData().find(m_currentSelectedObject);
									if (it != test->GetScriptData().end())
										if (ImGui::CollapsingHeader("PlayerControllerScriptData", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
										{
											ImGui::Text("speed: "); ImGui::SameLine(); ImGui::InputFloat("##movespeed", &it->second.speed, 1.0f, 100.f, "%.3f");
										}
								}
							}
						}

						// Camera component
						if (name == EntityManager::GetInstance().GetComponentID<Graphics::Camera>()) {
								if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
								{
										float viewportWidth{ EntityManager::GetInstance().Get<Graphics::Camera>(entityID).GetViewportWidth() };
										float viewportHeight{ EntityManager::GetInstance().Get<Graphics::Camera>(entityID).GetViewportHeight() };

										ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
										ImGui::Text("Viewport Dimensions: ");
										ImGui::Text("Width: "); ImGui::SameLine(); ImGui::InputFloat("##View Width", &viewportWidth, 1.0f, 100.f, "%.3f");
										ImGui::Text("Height: "); ImGui::SameLine(); ImGui::InputFloat("##View Height", &viewportHeight, 1.0f, 100.f, "%.3f");
										ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

										float zoom{ EntityManager::GetInstance().Get<Graphics::Camera>(entityID).GetMagnification() };
										ImGui::Text("Zoom: "); ImGui::SameLine(); ImGui::InputFloat("##Zoom", &zoom, 1.0f, 100.f, "%.3f");
										ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

										EntityManager::GetInstance().Get<Graphics::Camera>(entityID).SetViewDimensions(viewportWidth, viewportHeight);
										EntityManager::GetInstance().Get<Graphics::Camera>(entityID).SetMagnification(zoom);
								}
						}

					}


					ImGui::Dummy(ImVec2(0.0f, 10.0f));//add space
					ImGui::Separator();
					ImGui::Dummy(ImVec2(0.0f, 10.0f));//add space

					static bool isModalOpen;
					// Set the cursor position to the center of the window
					//the closest i can get to setting center the button x(
					//shld look fine
					ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 3.f, ImGui::GetCursorPosY()));

					//other than background, we can add components to objects
					if (m_currentSelectedObject)
						if (ImGui::Button("Add Components", ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 0)))
						{
							isModalOpen = true;
							ImGui::OpenPopup("Components");
						}

					//add different kind of components, however if it already has we cannot add
					if (ImGui::BeginPopup("Components"))
					{
						if (ImGui::Selectable("Add Collision"))
						{
							//not allowed to add collision without a rigidbody
							if (EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<RigidBody>()))
							{
								if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Collider>()))
									EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<Collider>() });
								else
									AddErrorLog("ALREADY HAS A COLLIDER");
							}
							else
							{
								AddErrorLog("ADD RIGIDBODY FIRST");
							}
						}
						if (ImGui::Selectable("Add Transform"))
						{
							if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Transform>()))
								EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<Transform>() });
							else
								AddErrorLog("ALREADY HAS A TRANSFORM");
						}
						if (ImGui::Selectable("Add RigidBody"))
						{
							if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<RigidBody>()))
								EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
							else
								AddErrorLog("ALREADY HAS A TRANSFORM");
						}
						if (ImGui::Selectable("Add Renderer"))
						{
							if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Graphics::Renderer>()))
								EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<Graphics::Renderer>() });
							else
								AddErrorLog("ALREADY HAS A RENDERER");
						}
						if (ImGui::Selectable("Add ScriptComponent"))
						{
							if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<ScriptComponent>()))
								EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<ScriptComponent>() });
							else
								AddErrorLog("ALREADY HAS A SCRIPTCOMPONENT");
						}
						ImGui::EndPopup();
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

			//if player is still holding the mouse down
			if (isDragging)
			{
				if (draggedItemIndex >= 0)
				{
					// Create a floating preview of the dragged item
					ImGui::SetNextWindowPos(ImGui::GetMousePos());
					ImGui::SetNextWindowSize(ImVec2(50, 50));
					std::string test = std::to_string(draggedItemIndex);
					ImGui::Begin(test.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
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
			// TEMPORARY HARD CODED
			std::vector<float> values{
				TimeManager::GetInstance().GetSystemFrameTime(0) / TimeManager::GetInstance().GetFrameTime(),
				TimeManager::GetInstance().GetSystemFrameTime(1) / TimeManager::GetInstance().GetFrameTime(),
				TimeManager::GetInstance().GetSystemFrameTime(2) / TimeManager::GetInstance().GetFrameTime(),
				TimeManager::GetInstance().GetSystemFrameTime(3) / TimeManager::GetInstance().GetFrameTime()
			};
			char* names[] = { "Input", "Physics", "Collision", "Graphics" };
			ImGui::PlotHistogram("##Test", values.data(), static_cast<int>(values.size()), 0, NULL, 0.0f, 1.0f, ImVec2(200, 80.0f));

			if (ImGui::IsItemHovered())
			{
				//current mouse position - the top left position of the rect to get your actual mouse
				float MousePositionX = ImGui::GetIO().MousePos.x - ImGui::GetItemRectMin().x;
				//so your mouseposition/ rect length * number of values to get your current index
				int hoveredIndex = static_cast<int>(MousePositionX / ImGui::GetItemRectSize().x * values.size());

				if (hoveredIndex > -1 && hoveredIndex < values.size())
				{
					ImGui::BeginTooltip();
					ImGui::Text("%s: %.2f%%", names[hoveredIndex], values[hoveredIndex] * 100);
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
					ImGui::DockBuilderDockWindow("resourcewindow", dock_id_down);
					ImGui::DockBuilderDockWindow("consolewindow", dock_id_down);


					//set on the save location to dock ontop of eachother
					ImGui::DockBuilderDockWindow("componentwindow", dock_id_left);

					//split the bottom into 2
					ImGuiID dock_id_down2 = ImGui::DockBuilderSplitNode(dock_id_down, ImGuiDir_Right, 0.5f, nullptr, &dock_id_down);

					ImGui::DockBuilderDockWindow("logwindow", dock_id_down2);

					//end dock
					ImGui::DockBuilderFinish(dockspace_id);

				}

			}

			nlohmann::json serializedEntity;  // Declaring at higher scope-hans

			//docking port menu bar
			if (ImGui::BeginMainMenuBar())
			{
				//mennu 1
				if (ImGui::BeginMenu("Scenes"))
				{
					if (ImGui::MenuItem("Save", "CTRL+S")) // the ctrl s is not programmed yet, need add to the key press event
					{
						if (m_currentSelectedObject)
						{
							serializationManager.SaveToFile("../Assets/Prefabs/Saved_Data_Testing.json", static_cast<int>(EntityManager::GetInstance().GetEntitiesInPool(ALL)[m_currentSelectedObject]));
						}

					}
					if (ImGui::MenuItem("Load"))
					{
						//to open file explorer
						OPENFILENAME ofn;
						wchar_t szFile[260];
						ZeroMemory(&ofn, static_cast<size_t>(sizeof(ofn)));
						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = NULL;
						ofn.lpstrFile = szFile;
						ofn.lpstrFile[0] = '\0';
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = L"JSON Files\0*.json\0All Files\0*.*\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

						if (GetOpenFileNameW(&ofn) == TRUE)
						{
							std::wstring wfp = ofn.lpstrFile;

							// Determine the required buffer size for the narrow string
							int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wfp.c_str(), -1, nullptr, 0, nullptr, nullptr);

							if (requiredSize > 0)
							{
								std::string fp(requiredSize, '\0');

								// Perform the actual conversion
								if (WideCharToMultiByte(CP_UTF8, 0, wfp.c_str(), -1, &fp[0], requiredSize, nullptr, nullptr) > 0)
								{
									serializationManager.LoadFromFile(fp);
								}
							}
						}
					}
					ImGui::Separator();
					//remove the false,false if using
					if (ImGui::MenuItem("Scene 1", "", false, false)) {}
					if (ImGui::MenuItem("Scene 2", "", false, false)) {}
					if (ImGui::MenuItem("Scene 3", "", false, false)) {}
					ImGui::EndMenu();
				}
				//does not work only for show
				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Undo", "")) {}
					if (ImGui::MenuItem("Redo", "", false, false)) {}
					ImGui::Separator();
					if (ImGui::MenuItem("Cut", "")) {}
					if (ImGui::MenuItem("Copy", "")) {}
					if (ImGui::MenuItem("Paste", "")) {}
					ImGui::EndMenu();
				}
				//all the different windows
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
					if (ImGui::MenuItem("ResourceList", "f5", m_showResourceWindow, !m_showResourceWindow))
					{
						m_showResourceWindow = !m_showResourceWindow;
					}
					if (ImGui::MenuItem("PerformanceWindow", "f6", m_showPerformanceWindow, !m_showPerformanceWindow))
					{
						m_showPerformanceWindow = !m_showPerformanceWindow;
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Close Editor", "esc", m_showEditor, true))
					{
						m_showEditor = !m_showEditor;
					}
					if (ImGui::MenuItem("Rubrics Test", "f7", m_showTestWindows, !m_showTestWindows))
					{
						m_showTestWindows = !m_showTestWindows;
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
		ImGuiStyle& style = ImGui::GetStyle();
		float size = ImGui::CalcTextSize("Play").x + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;

		float off = (float)((avail - size) * 0.5);
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off - (ImGui::CalcTextSize("Play").x + style.FramePadding.x)/2);
		//ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2.f, ImGui::GetCursorPosY()));
		if(ImGui::Button("Play"))
		{
			m_isRunTime = true;
		}
		ImGui::SameLine();
		if (
			ImGui::Button("Stop")
			) {
			m_isRunTime = false; 
		}
		if (ImGui::BeginChild("SceneViewChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar)) {
			//the graphics rendered onto an image on the imgui window
			ImGui::Image(
				reinterpret_cast<void*>(
					static_cast<intptr_t>(texture_id)),
				ImVec2(m_renderWindowWidth, m_renderWindowHeight),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
			m_mouseInScene = ImGui::IsWindowHovered();
		}
		ImGui::EndChild();

		//end the window
		ImGui::End();
	}

	void Editor::AddLog(std::string_view text)
	{
		m_logOutput.emplace_back(text);
		//might want to check a certain size on the vector if too much need to clear
	}

	void Editor::AddConsole(std::string_view text)
	{
		m_consoleOutput.emplace_back(text);
	}

	void Editor::AddErrorLog(std::string_view text)
	{
		std::string ss("[ERROR] ");
		ss += text;
		AddLog(ss);

	}

	void Editor::AddInfoLog(std::string_view text)
	{
		std::string ss("[INFO] ");
		ss += text;
		AddLog(ss);
	}

	void Editor::AddEventLog(std::string_view text)
	{
		std::string ss("[EVENT] ");
		ss += text;
		AddLog(ss);
	}

	void Editor::AddWarningLog(std::string_view text)
	{
		std::string ss("[WARNING] ");
		ss += text;
		AddLog(ss);
	}

	void Editor::ClearLog()
	{
		//not being used rn
		m_logOutput.clear();
	}

	void Editor::ClearConsole()
	{
		m_consoleOutput.clear();
	}

	void Editor::OnKeyTriggeredEvent(const PE::Event<PE::KeyEvents>& r_event)
	{
		PE::KeyTriggeredEvent KTE;

		//dynamic cast
		if (r_event.GetType() == PE::KeyEvents::KeyTriggered)
		{
			KTE = dynamic_cast<const PE::KeyTriggeredEvent&>(r_event);
		}

		if (KTE.keycode == GLFW_KEY_F1)
			m_showConsole = !m_showConsole;

		if (KTE.keycode == GLFW_KEY_F2)
			m_showObjectList = !m_showObjectList;

		if (KTE.keycode == GLFW_KEY_F3)
			m_showLogs = !m_showLogs;

		if (KTE.keycode == GLFW_KEY_F4)
			m_showSceneView = !m_showSceneView;

		if (KTE.keycode == GLFW_KEY_F7)
			m_showTestWindows = !m_showTestWindows;

		if (KTE.keycode == GLFW_KEY_F6)
			m_showPerformanceWindow = !m_showPerformanceWindow;

		if (KTE.keycode == GLFW_KEY_ESCAPE)
			m_showEditor = !m_showEditor;

		if (KTE.keycode == GLFW_KEY_F5)
			m_showResourceWindow = !m_showResourceWindow;

		if (KTE.keycode == GLFW_KEY_F10)
			ToggleDebugRender();
	}

}

