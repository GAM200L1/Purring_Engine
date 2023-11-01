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
#include "Time/FrameRateTargetControl.h"
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
#include "Logic/EnemyTestScript.h"
#include "GUISystem.h"
#include "Utilities/FileUtilities.h"
#include <random>
#include <rttr/type.h>
#include "Graphics/CameraManager.h"
# define M_PI           3.14159265358979323846 // temp definition of pi, will need to discuss where shld we leave this later on
#define HEX(hexcode)    hexcode/255.f * 100.f // to convert colors
SerializationManager serializationManager;  // Create an instance

extern Logger engine_logger;

namespace PE {

	std::filesystem::path Editor::m_parentPath{ "../Assets" };
	bool Editor::m_fileDragged{ false };

	Editor::Editor() {
		//initializing variables 
		//m_firstLaunch needs to be serialized 
		m_firstLaunch = true;
		//serialize based on what was deserialized
		m_showConsole = true;
		m_showLogs = true;
		m_showObjectList = true;
		m_showSceneView = true;
		m_showTestWindows = false;
		m_showComponentWindow = true;
		m_showResourceWindow = true;
		m_showPerformanceWindow = false;
		//show the entire gui 
		m_showEditor = true; // depends on the mode, whether we want to see the scene or the editor
		m_renderDebug = true; // whether to render debug lines
		//Subscribe to key pressed event 
		ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, Editor::OnKeyTriggeredEvent, this)
		//for the object list
		m_objectIsSelected = false;
		m_currentSelectedObject = 0;
		m_mouseInObjectWindow = false;
		//mapping commands to function calls
		m_commands.insert(std::pair<std::string_view, void(PE::Editor::*)()>("test", &PE::Editor::test));
		m_commands.insert(std::pair<std::string_view, void(PE::Editor::*)()>("ping", &PE::Editor::ping));
		// loading for assets window
		GetFileNamesInParentPath(m_parentPath, m_files);
		m_mouseInScene = false;
		m_entityToModify = -1;

		REGISTER_UI_FUNCTION(PlayAudio1,PE::Editor);
		REGISTER_UI_FUNCTION(PlayAudio2,PE::Editor);
	}

	Editor::~Editor()
	{
		m_files.clear();
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

	void Editor::PlayAudio1()
	{
		AudioManager::GetInstance().PlaySound("audio_sound1");
	}

	void Editor::PlayAudio2()
	{
		AudioManager::GetInstance().PlaySound("audio_sound2");
	}

	void Editor::ClearObjectList()
	{
		//make sure not hovering any objects as we are deleting
		m_currentSelectedObject = -1;
		//delete all objects
		for (int n = static_cast<int>(EntityManager::GetInstance().GetEntitiesInPool(ALL).size()) - 1; n >= 0; --n)
		{
			if(EntityManager::GetInstance().GetEntitiesInPool(ALL)[n] != Graphics::CameraManager::GetUICameraId())
				EntityManager::GetInstance().RemoveEntity(EntityManager::GetInstance().GetEntitiesInPool(ALL)[n]);
		}
	}

	void Editor::Init(GLFWwindow* p_window_)
	{
		//check imgui's version 
		IMGUI_CHECKVERSION();
		//create imgui context 
		ImGui::CreateContext();
		SetImGUIStyle();

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

		ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Icons/Directory_Icon.png", "../Assets/Icons/Directory_Icon.png");
		ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Icons/Audio_Icon.png"	, "../Assets/Icons/Audio_Icon.png");
		ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Icons/Font_Icon.png", "../Assets/Icons/Font_Icon.png");
		ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Icons/Prefabs_Icon.png", "../Assets/Icons/Prefabs_Icon.png");
		ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Icons/Other_Icon.png", "../Assets/Icons/Other_Icon.png");
		ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Icons/Texture_Icon.png", "../Assets/Icons/Texture_Icon.png");

	}

	void Editor::Render(GLuint texture_id)
	{
		//hide the entire editor
			//imgui start frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

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


			//imgui end frame render functions
			ImGui::Render();

			ImGuiIO& io = ImGui::GetIO();
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
		//if active
		if (IsEditorActive())
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
		if (IsEditorActive())
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
		if (IsEditorActive())
		if (!ImGui::Begin("Object List Window", Active)) // draw object list
		{
			ImGui::End(); //imgui close
		}
		else
		{
			//temporary here for counting created objects and add to name
			static int count = 0;
			//loop to show all the items ins the vector
			bool isHoveringObject{ false };
			static bool drag = false;
			static std::string dragName;
			std::optional<EntityID> hoveredObject{};
			static std::optional<EntityID> dragID{};
			std::map<EntityID, std::vector<EntityID>> dispMap{};
			for (const auto& id : SceneView())
			{
				if (EntityManager::GetInstance().Get<EntityDescriptor>(id).parent)
				{
					dispMap.erase(id);
					dispMap[EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.value()].emplace_back(id);
				}
				else // it does not have a parent
				{
					dispMap[id];
				}
			}

			if (ImGui::BeginChild("GameObjectList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {
				m_mouseInObjectWindow = ImGui::IsWindowHovered();
				for (auto & n : dispMap)
				{
					if (n.first == Graphics::CameraManager::GetUICameraId())
						continue;
					
					std::string name = std::to_string(n.first);
					name += ". ";
					name += EntityManager::GetInstance().Get<EntityDescriptor>(n.first).name;
					bool is_selected = (m_currentSelectedObject == static_cast<int>(n.first));

					if (ImGui::Selectable(name.c_str(), is_selected)) //imgui selectable is the function to make the clickable bar of text
						m_currentSelectedObject = static_cast<int>(n.first);
					if (ImGui::IsItemHovered()) {
						isHoveringObject = true;
						hoveredObject = n.first;
						if (ImGui::IsMouseDragging(0) && drag == false)
						{
							//seteting current index to check for selection
							dragName = name;
							drag = true;
							dragID = n.first;
						}
					}
					if (ImGui::IsItemClicked(1))
					{
						//m_currentSelectedObject = static_cast<int>(hoveredObject.value());
						if (m_currentSelectedObject < 0)
							m_currentSelectedObject = static_cast<int>(hoveredObject.value());
						ImGui::OpenPopup("popup");
					}
					// if there are children attatched
					if (!n.second.empty())
					{
						ImGui::Indent();
						for (const auto& id : n.second)
						{
							
							std::string name2 = std::to_string(id);
							name2 += ". ";
							name2 += EntityManager::GetInstance().Get<EntityDescriptor>(id).name;
							is_selected = (m_currentSelectedObject == static_cast<int>(id));

							if (ImGui::Selectable(name2.c_str(), is_selected)) //imgui selectable is the function to make the clickable bar of text
								m_currentSelectedObject = static_cast<int>(id);
							if (ImGui::IsItemHovered()) {
								isHoveringObject = true;
								hoveredObject = id;
								if (ImGui::IsMouseDragging(0) && drag == false)
								{
									m_currentSelectedObject = static_cast<int>(id); //seteting current index to check for selection
									dragName = name2;
									drag = true;
									dragID = id;
								}
							}
							if (ImGui::IsItemClicked(1))
							{
								//m_currentSelectedObject = static_cast<int>(hoveredObject.value());
								if (m_currentSelectedObject < 0)
									m_currentSelectedObject = static_cast<int>(hoveredObject.value());
								ImGui::OpenPopup("popup");
							}
						}

						ImGui::Unindent();
					}


					

					
					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected) // to show the highlight if selected
						ImGui::SetItemDefaultFocus();

					m_currentSelectedObject > -1 ? m_objectIsSelected = true : m_objectIsSelected = false;					
				}

				if (drag)
				{
					ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x+1,ImGui::GetMousePos().y+1));
					ImGui::Begin("##test", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
					ImGui::Text(dragName.c_str());
					ImGui::End();

					if (!ImGui::IsMouseDown(0))
					{
						drag = false;
						if(!hoveredObject || dragID.value() != hoveredObject.value())
						{
							if (hoveredObject && EntityManager::GetInstance().Get<EntityDescriptor>(hoveredObject.value()).parent)
								EntityManager::GetInstance().Get<EntityDescriptor>(dragID.value()).parent = EntityManager::GetInstance().Get<EntityDescriptor>(hoveredObject.value()).parent.value();
							else
								EntityManager::GetInstance().Get<EntityDescriptor>(dragID.value()).parent = hoveredObject;
							if (EntityManager::GetInstance().Get<EntityDescriptor>(dragID.value()).parent && EntityManager::GetInstance().Has<Transform>(dragID.value()))
							{
								EntityManager::GetInstance().Get<Transform>(dragID.value()).relPosition = EntityManager::GetInstance().Get<Transform>(dragID.value()).position;
								EntityManager::GetInstance().Get<Transform>(dragID.value()).relOrientation = EntityManager::GetInstance().Get<Transform>(dragID.value()).orientation;
							}

							if (hoveredObject)
								for (const auto& id : SceneView())
								{
									if (EntityManager::GetInstance().Get<EntityDescriptor>(id).parent && EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.value() == dragID.value())
									{
										if (EntityManager::GetInstance().Get<EntityDescriptor>(hoveredObject.value()).parent)
											EntityManager::GetInstance().Get<EntityDescriptor>(id).parent = EntityManager::GetInstance().Get<EntityDescriptor>(hoveredObject.value()).parent.value();
										else
											EntityManager::GetInstance().Get<EntityDescriptor>(id).parent = hoveredObject;
										if (EntityManager::GetInstance().Has<Transform>(id))
										{
											EntityManager::GetInstance().Get<Transform>(id).relPosition = EntityManager::GetInstance().Get<Transform>(id).position;
											EntityManager::GetInstance().Get<Transform>(id).relOrientation = EntityManager::GetInstance().Get<Transform>(id).orientation;
										}
									}
								}
						}
						dragID.reset();
					}
				}
}
			if (ImGui::BeginPopup("popup"))
			{
				if (ImGui::Selectable("Delete Object"))
				{
						AddInfoLog("Object Deleted");

						for (const auto& id : SceneView())
						{
							if (EntityManager::GetInstance().Get<EntityDescriptor>(id).parent && EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.value() == m_currentSelectedObject)
								EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.reset();
						}
						EntityManager::GetInstance().RemoveEntity(m_currentSelectedObject);

						//if not first index
						//m_currentSelectedObject != 1 ? m_currentSelectedObject -= 1 : m_currentSelectedObject = 0;
						m_currentSelectedObject = -1; // just reset it
						//if object selected
						m_objectIsSelected = false;
						//m_currentSelectedObject > -1 ? m_objectIsSelected = true : m_objectIsSelected = false;

						if (EntityManager::GetInstance().GetEntitiesInPool(ALL).empty()) m_currentSelectedObject = -1;//if nothing selected

						count--;

				}
				if (ImGui::Selectable("Clone Object"))
				{
					if (m_currentSelectedObject)
						EntityFactory::GetInstance().Clone(m_currentSelectedObject);
					else
						AddWarningLog("You are not allowed to clone the background");
				}
				ImGui::EndPopup();
			}

			ImGui::EndChild();
			if (ImGui::IsItemClicked(1) && !isHoveringObject)
			{
				ImGui::OpenPopup("Object");
			}
			if (ImGui::BeginPopup("Object"))
			{
				if (ImGui::Selectable("Create Empty Object"))
				{
					serializationManager.LoadFromFile("../Assets/Prefabs/Empty_Prefab.json");
				}
				if (ImGui::Selectable("Create UI Object"))
				{
					serializationManager.LoadFromFile("../Assets/Prefabs/Button_Prefab.json");
				}
				if (ImGui::Selectable("Create Camera Object"))
				{
					serializationManager.LoadFromFile("../Assets/Prefabs/Camera_Prefab.json");
				}
				ImGui::EndPopup();
			}

			ImGui::End();
		}
	}


	//temporary for milestone 1
	void Editor::ShowDemoWindow(bool* Active)
	{
		//if (IsEditorActive())
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
			if (ImGui::Button("Play SFX 1"))
			{
				AudioManager::GetInstance().PlaySound("audio_sound1");
			}
			ImGui::SameLine();
			if (ImGui::Button("Play SFX 2"))
			{
				AudioManager::GetInstance().PlaySound("audio_sound2");
			}
			ImGui::SameLine();
			if (ImGui::Button("Play SFX 3"))
			{
				AudioManager::GetInstance().PlaySound("audio_sound3");
			}
			if (ImGui::Button("Play Background Music"))
			{
				AudioManager::GetInstance().PlaySound("audio_backgroundMusic");
			}
			ImGui::SameLine();

			if (ImGui::Button("Stop All Audio"))
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
					//EntityFactory::GetInstance().Clone(EntityManager::GetInstance().GetEntitiesInPool(ALL)[id]);
					EntityID id2 = EntityFactory::GetInstance().CreateEntity();
					EntityFactory::GetInstance().Assign(id2, { EntityManager::GetInstance().GetComponentID<Transform>(), EntityManager::GetInstance().GetComponentID<Graphics::Renderer>() });
					EntityManager::GetInstance().Get<Transform>(id2).position.x = 15.f * (i % 50) - 320.f;
					EntityManager::GetInstance().Get<Transform>(id2).position.y = 15.f * (i / 50) - 320.f;
					EntityManager::GetInstance().Get<Transform>(id2).width = 10.f;
					EntityManager::GetInstance().Get<Transform>(id2).height = 10.f;
					EntityManager::GetInstance().Get<Transform>(id2).orientation = 0.f;
					EntityManager::GetInstance().Get<Graphics::Renderer>(id2).SetColor(1.f, 1.f, 1.f, 1.f);
					EntityManager::GetInstance().Get<Graphics::Renderer>(id2).SetTextureKey("cat");
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
		if (IsEditorActive())
		if (!ImGui::Begin("componentwindow", Active, IsEditorActive() ? 0 : ImGuiWindowFlags_NoInputs))
		{
			ImGui::End();
		}
		else
		{
			if (ImGui::BeginChild("GameComponentList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | (IsEditorActive() ? 0 : ImGuiWindowFlags_NoInputs)))
			{
				if (m_objectIsSelected)
				{
					EntityID entityID = m_currentSelectedObject;
					std::vector<ComponentID> components = EntityManager::GetInstance().GetComponentIDs(entityID);
					int componentCount = 0; //unique id for imgui objects
					bool hasScripts = false;
					for (const ComponentID& name : components)
					{
						++componentCount;//increment unique id


						// ---------- ENTITY DESCRIPTOR ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<EntityDescriptor>())
						{
							ImGui::SetNextItemAllowOverlap(); // allow the stacking of buttons


							//search through each component, create a collapsible header if the component exist
							rttr::type currType = rttr::type::get_by_name(name.to_string());

							if (ImGui::CollapsingHeader("EntityDescriptor", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
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
								for (auto& prop : currType.get_properties())
								{

									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									std::string nm(prop.get_name());
									nm += ": ";
									ImGui::Text(nm.c_str());

									rttr::variant vp = prop.get_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID));

									// handle types
									if (vp.get_type().get_name() == "std::string")
									{
										std::string tmp = vp.get_value<std::string>();
										std::string str = "##" + prop.get_name().to_string();
										ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);  ImGui::InputText(str.c_str(), &tmp);
										prop.set_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID), tmp);

									}
									else if (vp.get_type().get_name() == "classstd::optional<unsigned__int64>")
									{
										std::optional<EntityID> tmp = vp.get_value<std::optional<EntityID>>();

										std::string str = "##" + prop.get_name().to_string();
										if (tmp.has_value())
										{
											EntityID tmp2{ tmp.value()};
											std::string tmpStr{std::to_string(tmp2)};
											ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);  ImGui::InputText(str.c_str(), &tmpStr);
											std::optional<EntityID> op;
											if (tmpStr != "")
											{
												tmp2 = strtoull(tmpStr.c_str(), NULL, 10);
												if (errno == ERANGE)
												{
													errno = 0;
													engine_logger.AddLog(false, "Invalid input in editor field!", __FUNCTION__);
												}
												else
												{
													if(tmp2 != entityID)
													{
														op = tmp2;
														if (!EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent && EntityManager::GetInstance().Has<Transform>(entityID))
														{
															EntityManager::GetInstance().Get<Transform>(entityID).relPosition = EntityManager::GetInstance().Get<Transform>(entityID).position;
															EntityManager::GetInstance().Get<Transform>(entityID).relOrientation = EntityManager::GetInstance().Get<Transform>(entityID).orientation;
														}
													}
												}
											}
											prop.set_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID), op);
										}
										else
										{
											EntityID tmp2{0};
											std::string tmpStr{};
											ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);  ImGui::InputText(str.c_str(), &tmpStr);
											std::optional<EntityID> op;
											if (tmpStr != "")
											{
												tmp2 = strtoull(tmpStr.c_str(), NULL, 10);
												if (errno == ERANGE)
												{
													errno = 0;
													engine_logger.AddLog(false, "Invalid input in editor field!", __FUNCTION__);
												}
												else
												{
													if (tmp2 != entityID)
													{
														op = tmp2;
														if (!EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent && EntityManager::GetInstance().Has<Transform>(entityID))
														{
															EntityManager::GetInstance().Get<Transform>(entityID).relPosition = EntityManager::GetInstance().Get<Transform>(entityID).position;
															EntityManager::GetInstance().Get<Transform>(entityID).relOrientation = EntityManager::GetInstance().Get<Transform>(entityID).orientation;
														}
													}
												}
											}
											prop.set_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID), op);
										}
									}
								}
							}
						}


						// ---------- TRANSFORM ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<Transform>())
						{
							ImGui::SetNextItemAllowOverlap(); // allow the stacking of buttons


							//search through each component, create a collapsible header if the component exist
							rttr::type currType = rttr::type::get_by_name(name.to_string());
						
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
								for (auto& prop : currType.get_properties())
								{
									if (prop.get_name() == "Position" && EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent ||
										prop.get_name() == "Relative Position" && !EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent)
										continue;
									if (prop.get_name() == "Orientation" && EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent ||
										prop.get_name() == "Relative Orientation" && !EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent)
										continue;
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									std::string nm(prop.get_name());
									nm += ": ";
									ImGui::Text(nm.c_str());
									rttr::variant vp = prop.get_value(EntityManager::GetInstance().Get<Transform>(entityID));
									
									// handle types
									if (vp.get_type().get_name() == "structPE::vec2")
									{
										PE::vec2 tmp = vp.get_value<PE::vec2>();
										ImGui::Text("x: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);  ImGui::DragFloat(("##x" + prop.get_name().to_string()).c_str(), &tmp.x, 1.0f);
										ImGui::Text("y: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);  ImGui::DragFloat(("##y" + prop.get_name().to_string()).c_str(), &tmp.y, 1.0f);
										prop.set_value(EntityManager::GetInstance().Get<Transform>(entityID), tmp);
									}
									else if (vp.get_type().get_name() == "float")
									{
										float tmp = vp.get_value<float>();
										std::string str = "##" + prop.get_name().to_string();
										ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);  ImGui::InputFloat(str.c_str(), &tmp, 1.0f, 100.f, "%.3f");
										prop.set_value(EntityManager::GetInstance().Get<Transform>(entityID), tmp);
									}
								}
							}
						}


						// ---------- RIGID BODY ---------- //


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

								if (EntityManager::GetInstance().Get<RigidBody>(entityID).GetType() == EnumRigidBodyType::DYNAMIC)
								{
									//linear drag variable
									float linearDrag = EntityManager::GetInstance().Get<RigidBody>(entityID).GetLinearDrag();
									ImGui::Text("Linear Drag: "); ImGui::SameLine(); ImGui::InputFloat("##Linear Drag", &linearDrag, 1.0f, 100.f, "%.3f");
									EntityManager::GetInstance().Get<RigidBody>(entityID).SetLinearDrag(linearDrag);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								}
							}
						}


						// ---------- COLLIDER ---------- //
						

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

						// ---------- RENDERER ---------- //


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

								// Vector of filepaths that have already been loaded - used to refer to later when needing to change the object's texture
								std::vector<std::filesystem::path> filepaths;
								int i{ 0 };
								int index{ 0 };
								for (auto it = ResourceManager::GetInstance().Textures.begin(); it != ResourceManager::GetInstance().Textures.end(); ++it, ++i)
								{
									filepaths.emplace_back(it->first);
									if (it->first == EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetTextureKey())
										index = i;
								}

								// Vector of the names of textures that have already been loaded
								std::vector<std::string> loadedTextureKeys;

								// get the keys of textures already loaded by the resource manager
								for (auto const& r_filepath : filepaths)
								{
									loadedTextureKeys.emplace_back(r_filepath.stem().string());
								}

								//create a combo box of texture ids
								ImGui::SetNextItemWidth(200.0f);
								if (!loadedTextureKeys.empty())
								{
									// Displays the current texture set on the object
									if (ImGui::BeginChild("currentTexture", ImVec2{116,116}, true))
									{
										if (EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetTextureKey() != "")
										{
											ImGui::Image((void*)(intptr_t)ResourceManager::GetInstance().GetTexture(EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetTextureKey())->GetTextureID(), ImVec2{ 100,100 }, {0,1}, {1,0});
										}
									}
									ImGui::EndChild();

									// checks if mouse if hovering the texture preview - to use for asset browser drag n drop
									if (ImGui::IsItemHovered())
									{
										m_entityToModify = static_cast<int>(entityID);
									}

									// Shows a drop down of selectable textures
									ImGui::Text("Textures: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									bool bl{};
									if (EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetTextureKey() != "")
									{
										bl = ImGui::BeginCombo("##Textures", loadedTextureKeys[index].c_str());
									}
									else
									{
										bl = ImGui::BeginCombo("##Textures", ""); // The second parameter is the label previewed before opening the combo.
									}
									if (bl) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
												EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).SetTextureKey(filepaths[n].string());
										}
										ImGui::EndCombo();
									}
								}
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								
								// Color Setting
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

						
						// ---------- GUI RENDERER ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>())
						{
							//if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							if (ImGui::CollapsingHeader("GUIRenderer", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
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


								// Vector of filepaths that have already been loaded - used to refer to later when needing to change the object's texture
								std::vector<std::filesystem::path> filepaths;
								int i{ 0 };
								int guiIndex{ 0 };
								for (auto it = ResourceManager::GetInstance().Textures.begin(); it != ResourceManager::GetInstance().Textures.end(); ++it, ++i)
								{
									filepaths.emplace_back(it->first);
									if (it->first == EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetTextureKey())
										guiIndex = i;
								}

								// Vector of the names of textures that have already been loaded
								std::vector<std::string> loadedTextureKeys;

								// get the keys of textures already loaded by the resource manager
								for (auto const& r_filepath : filepaths)
								{
									loadedTextureKeys.emplace_back(r_filepath.stem().string());
								}


								//create a combo box of texture ids
								ImGui::SetNextItemWidth(200.0f);
								if (!loadedTextureKeys.empty())
								{
									// Displays the current texture set on the object
									if (ImGui::BeginChild("GUICurrentTexture", ImVec2{ 116,116 }, true))
									{
										if (EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetTextureKey() != "")
										{
											ImGui::Image((void*)(intptr_t)ResourceManager::GetInstance().GetTexture(EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetTextureKey())->GetTextureID(), ImVec2{ 100,100 }, { 0,1 }, { 1,0 });
										}
									}
									ImGui::EndChild();

									// Shows a drop down of selectable textures
									ImGui::Text("Textures: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									bool bl{};
									if (EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetTextureKey() != "")
									{
										bl = ImGui::BeginCombo("##GUITextures", loadedTextureKeys[guiIndex].c_str()); // The second parameter is the label previewed before opening the combo.
									}
									else
									{
										bl = ImGui::BeginCombo("##GUITextures", ""); // The second parameter is the label previewed before opening the combo.
									}
									if (bl)
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
												EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).SetTextureKey(filepaths[n].string());
										}
										ImGui::EndCombo();
									}
								}


								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								//setting colors

								//get and set color variable of the renderer component
								ImVec4 color;
								color.x = EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetColor().r;
								color.y = EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetColor().g;
								color.z = EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetColor().b;
								color.w = EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetColor().a;

								ImGui::Text("Change Color: "); ImGui::SameLine();
								ImGui::ColorEdit4("##Change Color", (float*)&color, ImGuiColorEditFlags_AlphaPreview);

								EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).SetColor(color.x, color.y, color.z, color.w);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
							}
						}


						// ---------- C++ SCRIPT COMPONENT ---------- //

						// This checks if the current component type (name) is the same as the ID of the ScriptComponent - Hans
						if (name == EntityManager::GetInstance().GetComponentID<ScriptComponent>())
						{
							hasScripts = true;

							// This creates a Collapsible header Titled: ScriptComponent - Hans
							if (ImGui::CollapsingHeader("ScriptComponent", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								// Aligns an "o" button next to the header for an additional option to "reset" variables to default settings (Hans requested but yet to implement logic) - Hans
								ImGui::SameLine();
								std::string id = "options##", o = "o##";
								id += std::to_string(componentCount);
								o += std::to_string(componentCount);

								// Popup menu for reset option (logic not yet implemented) - Hans
								if (ImGui::BeginPopup(id.c_str()))
								{
									if (ImGui::Selectable("Reset")) {}
									ImGui::EndPopup();
								}

								// Button to open the above popup - Hans
								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());

								// This adds some vertical spacing - Hans
								ImGui::Dummy(ImVec2(0.0f, 5.0f));

								//setting keys - Jarran
								std::vector<const char*> key;

								// Iterates through all available scripts and stores their names in 'key' - Hans
								for (std::map<std::string, Script*>::iterator it = LogicSystem::m_scriptContainer.begin(); it != LogicSystem::m_scriptContainer.end(); ++it)
								{
									key.push_back(it->first.c_str());
								}
								static int scriptindex{};

								// Dropdown box for selecting a script to add - Hans
								ImGui::SetNextItemWidth(200.0f);
								if (!key.empty())
								{
									ImGui::Text("Scripts: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									//set selected texture id
									if (ImGui::Combo("##Scripts", &scriptindex, key.data(), static_cast<int>(key.size()))) {}			// This creates a combo box that lists all available scripts - Hans
								}

								// Button to add the selected script to the entity. When clicked, this button adds the selected script to the entity's ScriptComponent and calls the script's OnAttach method. - Hans
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
								if (ImGui::Button("Add Script"))
								{
									EntityManager::GetInstance().Get<ScriptComponent>(entityID).addScript(key[scriptindex]);
									LogicSystem::m_scriptContainer[key[scriptindex]]->OnAttach(entityID);
								}
								ImGui::PopStyleColor(1);

								// Separator and spacing
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								static int selectedScript{ -1 };
								static std::string selectedScriptName{};
								ImGui::Text("Scripts List");

								// Lists all the scripts currently added to the entity - Hans
								if (ImGui::BeginChild("ScriptList", ImVec2(-1, 200), true, ImGuiWindowFlags_NoResize)) {
									int n = 0;
									/*				for (int n = 0; n < EntityManager::GetInstance().Get<ScriptComponent>(entityID).m_scriptKeys.size(); n++)
													{*/
									for (auto& [str, state] : EntityManager::GetInstance().Get<ScriptComponent>(entityID).m_scriptKeys)
									{
										const bool is_selected = (selectedScript == n);

										if (ImGui::Selectable(str.c_str(), is_selected))
										{
											selectedScript = n; //seteting current index to check for selection
											selectedScriptName = str;
										}//imgui selectable is the function to make the clickable bar of text

										// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
										if (is_selected) // to show the highlight if selected
											ImGui::SetItemDefaultFocus();

										++n;
									}
									//}
								}
								ImGui::EndChild();
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));

								// Button to remove the selected script from the entity. When clicked, this button removes the selected script from the entity's ScriptComponent and calls the script's OnDetach method - Hans
								if (ImGui::Button("Remove Script"))
								{
									if (selectedScript >= 0)
									{
										EntityManager::GetInstance().Get<ScriptComponent>(entityID).removeScript(selectedScriptName);
										LogicSystem::m_scriptContainer[selectedScriptName]->OnDetach(entityID);
										selectedScript = -1;
									}
								}
								ImGui::PopStyleColor(1);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
							}
						}

                        
						
						// ---------- GUI ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<GUI>())
						{
							if (ImGui::CollapsingHeader("GUIComponent", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
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
								int index = static_cast<int>(EntityManager::GetInstance().Get<GUI>(entityID).m_UIType);
								const char* types[] = { "Button","TextBox" };
								ImGui::Text("UI Type: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.0f);
								//set combo box for the different collider types
								if (ImGui::Combo("##UI Types", &index, types, IM_ARRAYSIZE(types)))
								{
									EntityManager::GetInstance().Get<GUI>(entityID).m_UIType = static_cast<UIType>(index);
								}

								//set combo box for functions
								//setting keys
								std::vector<const char*> key;
								key.push_back("");
								//to get all the keys
								for (auto it = GUISystem::m_uiFunc.begin(); it != GUISystem::m_uiFunc.end(); ++it)
								{
									key.push_back(it->first.data());
								}
								int onclickfunc{};
								for (std::string str : key)
								{
									if (str == EntityManager::GetInstance().Get<GUI>(entityID).m_onClicked)
										break;
									onclickfunc++;
								}
								//create a combo box of scripts
								ImGui::SetNextItemWidth(200.0f);
								if (!key.empty())
								{
									ImGui::Text("On Clicked: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									//set selected texture id
									if (ImGui::Combo("##On Click Function", &onclickfunc, key.data(), static_cast<int>(key.size())))
									{
										EntityManager::GetInstance().Get<GUI>(entityID).m_onClicked = key[onclickfunc];
									}
								}

								int onhoverfunc{};
								for (std::string str : key)
								{
									if (str == EntityManager::GetInstance().Get<GUI>(entityID).m_onHovered)
										break;
									onhoverfunc++;
								}
								//create a combo box of scripts
								ImGui::SetNextItemWidth(200.0f);
								if (!key.empty())
								{
									ImGui::Text("On Hovered: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									//set selected texture id
									if (ImGui::Combo("##On Hover Function", &onhoverfunc, key.data(), static_cast<int>(key.size())))
									{
											EntityManager::GetInstance().Get<GUI>(entityID).m_onHovered = key[onhoverfunc];
									}
								}
							}
						}
                        

						
						// ---------- CAMERA ---------- //


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

						// Animation component
						if (name == EntityManager::GetInstance().GetComponentID<AnimationComponent>())
						{
							//if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
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
								for (std::map<std::string, std::shared_ptr<Animation>>::iterator it = ResourceManager::GetInstance().Animations.begin(); it != ResourceManager::GetInstance().Animations.end(); ++it)
								{
									key.push_back(it->first.c_str());
								}
								int index{};
								for (std::string str : key)
								{
									if (str == EntityManager::GetInstance().Get<AnimationComponent>(entityID).GetAnimationID())
										break;
									index++;
								}

								//create a combo box of texture ids
								ImGui::SetNextItemWidth(200.0f);
								if (!key.empty())
								{
									ImGui::Text("Animations: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									//set selected texture id
									if (ImGui::Combo("##Animation", &index, key.data(), static_cast<int>(key.size())))
									{
										EntityManager::GetInstance().Get<AnimationComponent>(entityID).SetCurrentAnimationIndex(key[index]);
									}
								}
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Separator();
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								////setting colors
								 
								////get and set color variable of the renderer component
								//ImVec4 color;
								//color.x = EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetColor().r;
								//color.y = EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetColor().g;
								//color.z = EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetColor().b;
								//color.w = EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetColor().a;

								//ImGui::Text("Change Color: "); ImGui::SameLine();
								//ImGui::ColorEdit4("##Change Color", (float*)&color, ImGuiColorEditFlags_AlphaPreview);

								//EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).SetColor(color.x, color.y, color.z, color.w);

								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
							}
						}
					}

					if (hasScripts)
					{
						for (auto& [key, val] : LogicSystem::m_scriptContainer)
						{
							if (key == "test")
							{
								testScript* p_Script = dynamic_cast<testScript*>(val);
								auto it = p_Script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_Script->GetScriptData().end())
									if (ImGui::CollapsingHeader("testdata", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										ImGui::Text("rot speed: "); ImGui::SameLine(); ImGui::InputFloat("##rspeed", &it->second.m_rotationSpeed, 1.0f, 100.f, "%.3f");
									}
							}

							if (key == "PlayerControllerScript")
							{
								PlayerControllerScript* p_Script = dynamic_cast<PlayerControllerScript*>(val);
								auto it = p_Script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_Script->GetScriptData().end())
									if (ImGui::CollapsingHeader("PlayerControllerScriptData", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										ImGui::Text("speed: "); ImGui::SameLine(); ImGui::DragFloat("##movespeed", &it->second.speed);
									}
							}

							if (key == "EnemyTestScript")
							{
								EnemyTestScript* p_Script = dynamic_cast<EnemyTestScript*>(val);
								auto it = p_Script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_Script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("EnemyTestScript", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										int id = static_cast<int> (it->second.playerID);
										ImGui::Text("Player ID: "); ImGui::SameLine(); ImGui::InputInt("##id", &id);
										it->second.playerID = id;
										ImGui::Text("speed: "); ImGui::SameLine(); ImGui::DragFloat("##enemyspeed", &it->second.speed);
										ImGui::Text("Idle Timer: "); ImGui::SameLine(); ImGui::DragFloat("##enemyidle", &it->second.idleTimer);
										ImGui::Text("Alert Timer: "); ImGui::SameLine(); ImGui::DragFloat("##enemyalert", &it->second.alertTimer);
										ImGui::Text("Timer Buffer: "); ImGui::SameLine(); ImGui::DragFloat("##enemytimerbuffer", &it->second.timerBuffer);
										ImGui::Text("Patrol Timer: "); ImGui::SameLine(); ImGui::DragFloat("##enemypatrol", &it->second.patrolTimer);
										ImGui::Text("Target Range: "); ImGui::SameLine(); ImGui::DragFloat("##targettingrange", &it->second.TargetRange);
									}
								}
							}
						}
					}



					ImGui::Dummy(ImVec2(0.0f, 10.0f));//add space
					ImGui::Separator();
					ImGui::Dummy(ImVec2(0.0f, 10.0f));//add space

					// Set the cursor position to the center of the window
					//the closest i can get to setting center the button x(
					//shld look fine
					ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 3.f, ImGui::GetCursorPosY()));

						if (ImGui::Button("Add Components", ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 0)))
						{
							ImGui::OpenPopup("Components");
						}

					//add different kind of components, however if it already has we cannot add
					if (ImGui::BeginPopup("Components"))
					{
						if (ImGui::Selectable("Add Collision"))
						{
							if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Collider>()))
								EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<Collider>() });
							else
								AddErrorLog("ALREADY HAS A COLLIDER");
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
						std::vector<std::string> test {"test1", "test2", "test3"}; // replace this vector with your not hardcoded way of getting your scripts
						if (ImGui::BeginMenu("Scripts"))
						{
							for (auto str : test) // change to whatever loop you want
							{
								if (ImGui::MenuItem(str.c_str())) 
								{
									//do whatever script specific stuff here
								}
							}
							ImGui::EndMenu();
						}

						if (ImGui::Selectable("Add Animation"))
						{
							if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
								EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<AnimationComponent>() });
							else
								AddErrorLog("ALREADY HAS ANIMATION");
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
		if (IsEditorActive())
		//testing for drag and drop
		if (!ImGui::Begin("resourcewindow", Active)) // draw resource list
		{
			ImGui::End(); //imgui close
		}
		else
		{
			static int draggedItemIndex = -1;
			static bool isDragging = false;
			static std::string iconDragged{};
			//ImGuiStyle& style = ImGui::GetStyle();
			if (ImGui::BeginChild("resource list", ImVec2(0, 0), true)) {
				
				// Displays Header with File Directories
				// skips ../ portion
				for (auto iter = std::next(m_parentPath.begin()); iter != m_parentPath.end(); ++iter)
				{
					ImGui::SameLine();
					ImGui::Text(("> " + (*iter).string()).c_str());
					if (ImGui::IsItemClicked(0)) {
						std::string newPath{};
						for (auto iter2 = m_parentPath.begin(); iter2 != iter; ++iter2)
						{
							newPath += (*iter2).string() + "/";
						}
						newPath += (*iter).string();
						m_parentPath = std::filesystem::path{ newPath };
						GetFileNamesInParentPath(m_parentPath, m_files);
						break;
					}
				}
				ImGui::Separator();
				

				ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)2), ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

				if (ImGui::IsWindowHovered())
				{
					glfwSetDropCallback(p_window, &HotLoadingNewFiles);
					if (m_fileDragged)
					{
						GetFileNamesInParentPath(m_parentPath, m_files);
						for (std::filesystem::path const& r_filepath : m_files)
						{
							if (r_filepath.extension() == ".png")
							{
								if (ResourceManager::GetInstance().Textures.find(r_filepath.string()) != ResourceManager::GetInstance().Textures.end())
								{
									ResourceManager::GetInstance().Textures[r_filepath.string()]->CreateTexture(r_filepath.string());
								}
							}
						}
						m_fileDragged = false;
					}
				}

				int numItemPerRow = (ImGui::GetWindowSize().x < 100.f) ? 1 : static_cast<int>(ImGui::GetWindowSize().x / 100.f);

				// list the files in the current showing directory as imgui text
				for (int n = 0; n < m_files.size(); n++) // loop through resource list here
				{	//resource list needs a list of icons for the texture for the image if possible
					//else just give a standard object icon here
					
					if (n % numItemPerRow) // to keep it in rows where 3 is max 3 colums
						ImGui::SameLine();
					
					if (ImGui::BeginChild(m_files[n].filename().string().c_str(), ImVec2(100, 100))) //child to hold image n text
					{
						std::string icon{};
						std::string const extension{ m_files[n].filename().extension().string() };
						if (extension == "")
							icon = "../Assets/Icons/Directory_Icon.png";
						else if (extension == ".mp3")
							icon = "../Assets/Icons/Audio_Icon.png";
						else if (extension == ".ttf")
							icon = "../Assets/Icons/Font_Icon.png";
						else if (extension == ".json")
							icon = "../Assets/Icons/Prefabs_Icon.png";
						else if (extension == ".png")
							icon = "../Assets/Icons/Texture_Icon.png";
						else
							icon = "../Assets/Icons/Other_Icon.png";

						ImGui::Image((void*)(intptr_t)ResourceManager::GetInstance().GetTexture(icon)->GetTextureID(), ImVec2(50, 50), { 0,1 }, { 1,0 });
						ImGui::Text(m_files[n].filename().string().c_str()); // text
					}
					ImGui::EndChild();

					// check if the mouse is hovering the asset
					if (ImGui::IsItemHovered())
					{
						// if item is a file with extension eg. .txt , .png
						if (m_files[n].extension() != "")
						{
							// Handle item clicks and drags
							if (ImGui::IsMouseClicked(0)) {
								draggedItemIndex = n; // Start dragging
								isDragging = true;

								std::string iconDraggedExtension = m_files[n].extension().string();
								if (iconDraggedExtension == "")
									iconDragged = "../Assets/Icons/Directory_Icon.png";
								else if (iconDraggedExtension == ".mp3")
									iconDragged = "../Assets/Icons/Audio_Icon.png";
								else if (iconDraggedExtension == ".ttf")
									iconDragged = "../Assets/Icons/Font_Icon.png";
								else if (iconDraggedExtension == ".json")
									iconDragged = "../Assets/Icons/Prefabs_Icon.png";
								else if (iconDraggedExtension == ".png")
									iconDragged = "../Assets/Icons/Texture_Icon.png";
								else
									iconDragged = "../Assets/Icons/Other_Icon.png";
							}
						}
						else
						{
							if (ImGui::IsMouseClicked(0)) {
								std::string replaceSeparators = m_files[n].string();
								std::replace(replaceSeparators.begin(), replaceSeparators.end(), '\\', '/');
								m_parentPath = std::filesystem::path{ replaceSeparators };
								GetFileNamesInParentPath(m_parentPath, m_files);
							}
						}

					}
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();

			//if player is still holding the mouse down
			if (isDragging)
			{
				if (draggedItemIndex >= 0)
				{
					// Create a floating preview of the dragged item
					ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x + 1.f, ImGui::GetMousePos().y - 1.f));
					ImGui::SetNextWindowSize(ImVec2(50, 50));
					std::string test = std::to_string(draggedItemIndex);
					ImGui::Begin(test.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
					ImGui::Image((void*)(intptr_t)ResourceManager::GetInstance().GetTexture(iconDragged)->GetTextureID(), ImVec2(34,34), { 0,1 }, { 1,0 });
					ImGui::End();
					
					// Check if the mouse button is released
					if (ImGui::IsMouseReleased(0))
					{
						if (m_entityToModify != -1)
						{
							// alters the texture assigned to renderer component in entity
							std::string const extension = m_files[draggedItemIndex].extension().string();
							if (extension == ".png")
							{
								ResourceManager::GetInstance().LoadTextureFromFile(m_files[draggedItemIndex].string(), m_files[draggedItemIndex].string());
								EntityManager::GetInstance().Get<Graphics::Renderer>(m_entityToModify).SetTextureKey(m_files[draggedItemIndex].string());
								EntityManager::GetInstance().Get<Graphics::Renderer>(m_entityToModify).SetColor(1.f, 1.f, 1.f, 1.f);
							}
							// add remaining editable assets audio etc
						}

						if (m_mouseInScene || m_mouseInObjectWindow)
						{
							if (m_files[draggedItemIndex].extension() == ".json")
							{
								serializationManager.LoadFromFile(m_files[draggedItemIndex].string());
								// change position of loaded prefab based on mouse cursor here
							}
						}

						isDragging = false;
						draggedItemIndex = -1;
					}
				}
			}
			else
			{
				m_entityToModify = -1;
			}


			ImGui::End(); //imgui close
		}


	}

	void Editor::ShowPerformanceWindow(bool* Active)
	{
		//if (IsEditorActive())
		if (!ImGui::Begin("performanceWindow", Active, ImGuiWindowFlags_AlwaysAutoResize)) // draw resource list
		{
			ImGui::End(); //imgui close
		}
		else
		{
			std::vector<float> const& fpsValues{ TimeManager::GetInstance().m_frameRateController.GetFpsValues() };
			ImGui::SeparatorText("FPS Viewer");
			std::string fpsString{ "FPS: " + std::to_string(static_cast<int>(TimeManager::GetInstance().m_frameRateController.GetFps())) };

			// Plot fps line graph
			ImGui::Text("Average FPS: %.2f", TimeManager::GetInstance().m_frameRateController.GetAverageFps());
			ImGui::Text("Max FPS: %.2f", TimeManager::GetInstance().m_frameRateController.GetMaxFps());
			ImGui::Text("Min FPS: %.2f", TimeManager::GetInstance().m_frameRateController.GetMinFps());
			ImGui::PlotLines("##FpsLine", fpsValues.data(), static_cast<int>(fpsValues.size()), 0, fpsString.c_str(), 0.0f, 1000.0f, ImVec2(400.f, 200.0f));

			// Plot System usage progress bar
			ImGui::SeparatorText("System Usage");
			ImGui::Text("Input: %.2f%%", TimeManager::GetInstance().GetSystemFrameUsage(SystemID::INPUT) * 100.f);
			ImGui::ProgressBar(TimeManager::GetInstance().GetSystemFrameUsage(SystemID::INPUT), ImVec2(400.f, 30.0f), NULL);

			ImGui::Text("Logic: %.2f%%", TimeManager::GetInstance().GetSystemFrameUsage(SystemID::LOGIC) * 100.f);
			ImGui::ProgressBar(TimeManager::GetInstance().GetSystemFrameUsage(SystemID::LOGIC), ImVec2(400.f, 30.0f), NULL);

			ImGui::Text("Physics: %.2f%%", TimeManager::GetInstance().GetSystemFrameUsage(SystemID::PHYSICS) * 100.f);
			ImGui::ProgressBar(TimeManager::GetInstance().GetSystemFrameUsage(SystemID::PHYSICS), ImVec2(400.f, 30.0f), NULL);

			ImGui::Text("Collision: %.2f%%", TimeManager::GetInstance().GetSystemFrameUsage(SystemID::COLLISION) * 100.f);
			ImGui::ProgressBar(TimeManager::GetInstance().GetSystemFrameUsage(SystemID::COLLISION), ImVec2(400.f, 30.0f), NULL);

			ImGui::Text("Animation: %.2f%%", TimeManager::GetInstance().GetSystemFrameUsage(SystemID::ANIMATION) * 100.f);
			ImGui::ProgressBar(TimeManager::GetInstance().GetSystemFrameUsage(SystemID::ANIMATION), ImVec2(400.f, 30.0f), NULL);

			ImGui::Text("Camera: %.2f%%", TimeManager::GetInstance().GetSystemFrameUsage(SystemID::CAMERA) * 100.f);
			ImGui::ProgressBar(TimeManager::GetInstance().GetSystemFrameUsage(SystemID::CAMERA), ImVec2(400.f, 30.0f), NULL);

			ImGui::Text("Graphics: %.2f%%", TimeManager::GetInstance().GetSystemFrameUsage(SystemID::GRAPHICS) * 100.f);
			ImGui::ProgressBar(TimeManager::GetInstance().GetSystemFrameUsage(SystemID::GRAPHICS), ImVec2(400.f, 30.0f), NULL);

			//char* names[] = { "Input", "Logic", "Physics", "Collision", "Camera", "Graphics" };
			//ImGui::SeparatorText("System Usage");
			//ImGui::PlotHistogram("##Test", values.data(), static_cast<int>(values.size()), 0, NULL, 0.0f, 1.0f, ImVec2(200, 80.0f));

			//if (ImGui::IsItemHovered())
			//{
			//	//current mouse position - the top left position of the rect to get your actual mouse
			//	float MousePositionX = ImGui::GetIO().MousePos.x - ImGui::GetItemRectMin().x;
			//	//so your mouseposition/ rect length * number of values to get your current index
			//	int hoveredIndex = static_cast<int>(MousePositionX / ImGui::GetItemRectSize().x * values.size());

			//	if (hoveredIndex > -1 && hoveredIndex < values.size())
			//	{
			//		ImGui::BeginTooltip();
			//		ImGui::Text("%s: %.2f%%", names[hoveredIndex], values[hoveredIndex] * 100);
			//		ImGui::EndTooltip();
			//	}
			//}
			ImGui::End(); //imgui close
		}
	}

	void Editor::SetDockingPort(bool* Active)
	{
		if (IsEditorActive()) {

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
						ImGui::DockBuilderDockWindow("Object List Window", dock_id_right);

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

				//docking port menu bar
				if (ImGui::BeginMainMenuBar())
				{
					//menu 1
					if (ImGui::BeginMenu("Scenes"))
					{
						if (ImGui::MenuItem("Save", "CTRL+S")) // the ctrl s is not programmed yet, need add to the key press event
						{
							engine_logger.AddLog(false, "Attempting to save all entities to file...", __FUNCTION__);
							// This will save all entities to a file
							serializationManager.SaveAllEntitiesToFile("../Assets/Prefabs/Saved_All_Entities.json");
							engine_logger.AddLog(false, "Entities saved successfully to file.", __FUNCTION__);
						}
						if (ImGui::MenuItem("Load"))
						{
							engine_logger.AddLog(false, "Opening file explorer to load entities...", __FUNCTION__);

							// Invoke the file explorer and allow user to choose a JSON file for loading entities.
							std::string filePath = serializationManager.OpenFileExplorer();
							if (!filePath.empty())
							{
								engine_logger.AddLog(false, "Attempting to load entities from chosen file...", __FUNCTION__);

								// This will load all entities from the file
								ClearObjectList();
								serializationManager.LoadAllEntitiesFromFile(filePath);
								engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
							}
							else
							{
								engine_logger.AddLog(false, "File path is empty. Aborted loading entities.", __FUNCTION__);
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
						if (ImGui::MenuItem("component list", "f7", m_showComponentWindow, !m_showComponentWindow))
						{
							m_showComponentWindow = !m_showComponentWindow;
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
						if (ImGui::MenuItem("Rubrics Test", "", m_showTestWindows, !m_showTestWindows))
						{
							m_showTestWindows = !m_showTestWindows;
						}
						ImGui::Separator();
						if (ImGui::MenuItem("Reset Default", "", false, true))
						{
							m_firstLaunch = true;
							m_showResourceWindow = false;
							m_showPerformanceWindow = false;
						}
						ImGui::EndMenu();
					}
				}
				ImGui::EndMainMenuBar(); // closing of menu begin function
				ImGui::End(); //finish drawing
			}
		}
	}

	void Editor::ShowSceneView(GLuint texture_id, bool* active)
	{
		if (IsEditorActive()) {
			ImGui::Begin("sceneview", active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

			//setting the current width and height of the window to be drawn on
			m_renderWindowWidth = ImGui::GetContentRegionAvail().x;
			m_renderWindowHeight = ImGui::GetContentRegionAvail().y;
			ImGuiStyle& style = ImGui::GetStyle();
			float size = ImGui::CalcTextSize("Play").x + style.FramePadding.x * 2.0f;
			float avail = ImGui::GetContentRegionAvail().x;

			float off = (float)((avail - size) * 0.5);
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off - (ImGui::CalcTextSize("Play").x + style.FramePadding.x) / 2);
			//ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2.f, ImGui::GetCursorPosY()));
			if (ImGui::Button("Play"))
			{
				m_isRunTime = true;
				m_showEditor = false;
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
	}

	void Editor::SetImGUIStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(HEX(200), HEX(200), HEX(200), 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(.15f, .15f, .20f, 1.00f);;
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(.1f, .1f, .13f, 1.00f);;
		colors[ImGuiCol_TitleBgActive] = ImVec4(.15f, .15f, .25f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.70f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.70f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.3f, 0.3f, 0.3f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.59f, 0.98f, 0.4f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		//[ImGuiCol_Text] = The color for the text that will be used for the whole menu.
		//	[ImGuiCol_TextDisabled] = Color for "not active / disabled text".
		//	[ImGuiCol_WindowBg] = Background color.
		//	[ImGuiCol_PopupBg] = The color used for the background in ImGui::Combo and ImGui::MenuBar.
		//	[ImGuiCol_Border] = The color that is used to outline your menu.
		//	[ImGuiCol_BorderShadow] = Color for the stroke shadow.
		//	[ImGuiCol_FrameBg] = Color for ImGui::InputText and for background ImGui::Checkbox
		//	[ImGuiCol_FrameBgHovered] = The color that is used in almost the same way as the one above, except that it changes color when guiding it to ImGui::Checkbox.
		//	[ImGuiCol_FrameBgActive] = Active color.
		//	[ImGuiCol_TitleBg] = The color for changing the main place at the very top of the menu(where the name of your "top-of-the-table" is shown.
		//		ImGuiCol_TitleBgCollapsed = ImguiCol_TitleBgActive
		//		= The color of the active title window, ie if you have a menu with several windows, this color will be used for the window in which you will be at the moment.
		//		[ImGuiCol_MenuBarBg] = The color for the bar menu. (Not all sawes saw this, but still)
		//		[ImGuiCol_ScrollbarBg] = The color for the background of the "strip", through which you can "flip" functions in the software vertically.
		//		[ImGuiCol_ScrollbarGrab] = Color for the scoll bar, ie for the "strip", which is used to move the menu vertically.
		//		[ImGuiCol_ScrollbarGrabHovered] = Color for the "minimized / unused" scroll bar.
		//		[ImGuiCol_ScrollbarGrabActive] = The color for the "active" activity in the window where the scroll bar is located.
		//		[ImGuiCol_ComboBg] = Color for the background for ImGui::Combo.
		//		[ImGuiCol_CheckMark] = Color for your ImGui::Checkbox.
		//		[ImGuiCol_SliderGrab] = Color for the slider ImGui::SliderInt and ImGui::SliderFloat.
		//		[ImGuiCol_SliderGrabActive] = Color of the slider,
		//		[ImGuiCol_Button] = the color for the button.
		//		[ImGuiCol_ButtonHovered] = Color when hovering over the button.
		//		[ImGuiCol_ButtonActive] = Button color used.
		//		[ImGuiCol_Header] = Color for ImGui::CollapsingHeader.
		//		[ImGuiCol_HeaderHovered] = Color, when hovering over ImGui::CollapsingHeader.
		//		[ImGuiCol_HeaderActive] = Used color ImGui::CollapsingHeader.
		//		[ImGuiCol_Column] = Color for the "separation strip" ImGui::Column and ImGui::NextColumn.
		//		[ImGuiCol_ColumnHovered] = Color, when hovering on the "strip strip" ImGui::Column and ImGui::NextColumn.
		//		[ImGuiCol_ColumnActive] = The color used for the "separation strip" ImGui::Column and ImGui::NextColumn.
		//		[ImGuiCol_ResizeGrip] = The color for the "triangle" in the lower right corner, which is used to increase or decrease the size of the menu.
		//		[ImGuiCol_ResizeGripHovered] = Color, when hovering to the "triangle" in the lower right corner, which is used to increase or decrease the size of the menu.
		//		[ImGuiCol_ResizeGripActive] = The color used for the "triangle" in the lower right corner, which is used to increase or decrease the size of the menu.
		//		[ImGuiCol_CloseButton] = The color for the button - closing menu.
		//		[ImGuiCol_CloseButtonHovered] = Color, when you hover over the button - close menu.
		//		[ImGuiCol_CloseButtonActive] = The color used for the button - closing menu.
		//		[ImGuiCol_TextSelectedBg] = The color of the selected text, in ImGui::MenuBar.
		//		[ImGuiCol_ModalWindowDarkening] = The color of the "Blackout Window" of your menu.
		//		I rarely see these designations, but still decided to put them here.
		//		[ImGuiCol_Tab] = The color for tabs in the menu.
		//		[ImGuiCol_TabActive] = The active color of tabs, ie when you click on the tab you will have this color.
		//		[ImGuiCol_TabHovered] = The color that will be displayed when hovering on the table.
		//		[ImGuiCol_TabSelected] = The color that is used when you are in one of the tabs.
		//		[ImGuiCol_TabText] = Text color that only applies to tabs.
		//		[ImGuiCol_TabTextActive] = Active text color for tabs.
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
			m_showComponentWindow = !m_showComponentWindow;

		if (KTE.keycode == GLFW_KEY_F6)
			m_showPerformanceWindow = !m_showPerformanceWindow;

		if (KTE.keycode == GLFW_KEY_ESCAPE)
		{
			m_showEditor = true;
			
			if (m_showEditor)
				m_isRunTime = false;
		}

		if (KTE.keycode == GLFW_KEY_F5)
			m_showResourceWindow = !m_showResourceWindow;

		if (KTE.keycode == GLFW_KEY_F10)
			ToggleDebugRender();
	}

	void Editor::HotLoadingNewFiles(GLFWwindow* p_window, int count, const char** paths)
	{
		// prints the number of directories / files dragged over
		m_fileDragged = true;
		std::stringstream sstream;
		sstream << "Drag and drop count - " << count;
		engine_logger.AddLog(false, sstream.str(), "");

		std::vector<std::filesystem::path> consolidatedPaths;
		for (int i{ 0 }; i < count; ++i)
		{
			consolidatedPaths.emplace_back(paths[i]);
		}
		for (std::filesystem::path const& r_path : consolidatedPaths)
		{
			if (!std::filesystem::equivalent(r_path.parent_path(), m_parentPath))
				std::filesystem::copy(r_path, std::filesystem::path{ m_parentPath.string() + "/" + r_path.filename().string()}, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
		}

	}
}