/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Editor.cpp
 \date     30-08-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu
 \par      code %:     90%
 \par      changes:    Defined all the logic for rendering the editor UI through ImGUI.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     5%
 \par      changes:    02-11-2023
                       Added object picking logic.

 \co-author            Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu
 \par      code %:     5%
 \par      changes:    02-11-2023
					   Added animation editor, collision layer matrix.

 \brief
	cpp file containing the definition of the editor class, which contains 
	logic for rendering the editor UI through ImGUI.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "Editor.h"
#include "Memory/MemoryManager.h"
#include "AudioManager/AudioManager.h"
#include "AudioManager/AudioComponent.h"
#include "Time/FrameRateTargetControl.h"
#include "Time/TimeManager.h"
#include "ResourceManager/ResourceManager.h"
#include <Windows.h>
#include <Commdlg.h>
#include "Data/SerializationManager.h"
#include "Physics/PhysicsManager.h"
#include "Physics/CollisionManager.h"
#include "Logging/Logger.h"
#include "Logic/LogicSystem.h"
#include "Graphics/RendererManager.h"
#include "Logic/testScript.h"
#include "Logic/PlayerControllerScript.h"
#include "Logic/EnemyTestScript.h"
#include "Logic/FollowScript.h"
#include "Logic/CameraManagerScript.h"
#include "Logic/GameStateController.h"
#include "GUISystem.h"
#include "Utilities/FileUtilities.h"
#include <random>
#include <rttr/type.h>
#include "Graphics/CameraManager.h"
#include "Graphics/Text.h"
#include "GameStateManager.h"
#include "Data/json.hpp"
#include "Input/InputSystem.h"
#include "Layers/CollisionLayer.h"
#include "Logic/CatScript.h"
#include "Logic/RatScript.h"
# define M_PI           3.14159265358979323846 // temp definition of pi, will need to discuss where shld we leave this later on

SerializationManager serializationManager;  // Create an instance
extern Logger engine_logger;

namespace PE {

	std::filesystem::path Editor::m_parentPath{ "../Assets" };
	bool Editor::m_fileDragged{ false };

	Editor::Editor() {
		std::ifstream configFile("../Assets/Settings/config.json");
		nlohmann::json configJson;
		configFile >> configJson;
		//initializing variables 

		if (configJson.contains("Editor"))
		{
			//m_firstLaunch needs to be serialized 
			if (configJson["Editor"].contains("firstLaunch"))
				m_firstLaunch = configJson["Editor"]["firstLaunch"].get<bool>();

			//serialize based on what was deserialized
			if (configJson["Editor"].contains("showConsole"))
				m_showConsole = configJson["Editor"]["showConsole"].get<bool>();

			if (configJson["Editor"].contains("showLogs"))
				m_showLogs = configJson["Editor"]["showLogs"].get<bool>();

			if (configJson["Editor"].contains("showObjectList"))
				m_showObjectList = configJson["Editor"]["showObjectList"].get<bool>();

			if (configJson["Editor"].contains("showSceneView"))
				m_showSceneView = configJson["Editor"]["showSceneView"].get<bool>();

			if (configJson["Editor"].contains("showTestWindows"))
				m_showTestWindows = configJson["Editor"]["showTestWindows"].get<bool>();

			if (configJson["Editor"].contains("showComponentWindow"))
				m_showComponentWindow = configJson["Editor"]["showComponentWindow"].get<bool>();

			if (configJson["Editor"].contains("showResourceWindow"))
				m_showResourceWindow = configJson["Editor"]["showResourceWindow"].get<bool>();

			if (configJson["Editor"].contains("showPerformanceWindow"))
				m_showPerformanceWindow = configJson["Editor"]["showPerformanceWindow"].get<bool>();

			if (configJson["Editor"].contains("showAnimationWindow"))
				m_showAnimationWindow = configJson["Editor"]["showAnimationWindow"].get<bool>();

			if (configJson["Editor"].contains("showPhysicsWindow"))
				m_showPhysicsWindow = configJson["Editor"]["showPhysicsWindow"].get<bool>();
				//show the entire gui 

			m_showEditor = true; // depends on the mode, whether we want to see the scene or the editor

			if (configJson["Editor"].contains("renderDebug"))
				m_renderDebug = configJson["Editor"]["renderDebug"].get<bool>(); // whether to render debug lines

			// also an e.g of how to make it safe
			m_isPrefabMode = false;
		}
		else
		{
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
			m_showAnimationWindow = false;
			m_showPhysicsWindow = false;
			//show the entire gui 
			m_showEditor = true; // depends on the mode, whether we want to see the scene or the editor
			m_renderDebug = true; // whether to render debug lines
			m_isPrefabMode = false;
		}

		configFile.close();

		m_showGameView = false;

		// Set the default visual theme of the editor
		m_currentStyle = GuiStyle::DARK;

		//Subscribe to key pressed event 
		ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, Editor::OnKeyTriggeredEvent, this)
		//for the object list
		m_objectIsSelected = false;
		m_currentSelectedObject = 1;
		m_mouseInObjectWindow = false;
		//mapping commands to function calls
		m_commands.insert(std::pair<std::string_view, void(PE::Editor::*)()>("test", &PE::Editor::test));
		m_commands.insert(std::pair<std::string_view, void(PE::Editor::*)()>("ping", &PE::Editor::ping));
		// loading for assets window
		GetFileNamesInParentPath(m_parentPath, m_files);
		m_mouseInScene = false;
		m_entityToModify = std::make_pair<std::string, int>("", -1);

	}

	Editor::~Editor()
	{
		const char* filepath = "../Assets/Settings/config.json";
		std::ifstream configFile(filepath);
		nlohmann::json configJson;
		configFile >> configJson;

		// save the stuff
		//m_firstLaunch needs to be serialized 
		configJson["Editor"]["firstLaunch"] = m_firstLaunch;
		//serialize based on what was deserialized
		configJson["Editor"]["showConsole"] = m_showConsole;
		configJson["Editor"]["showLogs"] = m_showLogs;
		configJson["Editor"]["showObjectList"] = m_showObjectList;
		configJson["Editor"]["showSceneView"] = m_showSceneView;
		configJson["Editor"]["showTestWindows"] = m_showTestWindows;
		configJson["Editor"]["showComponentWindow"] = m_showComponentWindow;
		configJson["Editor"]["showResourceWindow"] = m_showResourceWindow;
		configJson["Editor"]["showPerformanceWindow"] = m_showPerformanceWindow;
		configJson["Editor"]["showAnimationWindow"] = m_showAnimationWindow;
		configJson["Editor"]["showPhysicsWindow"] = m_showPhysicsWindow;
		//show the entire gui 
		configJson["Editor"]["showEditor"] = true; // depends on the mode, whether we want to see the scene or the editor
		configJson["Editor"]["renderDebug"] = m_renderDebug; // whether to render debug lines
		configJson["Editor"]["isPrefabMode"] = m_isPrefabMode;


		std::ofstream outFile(filepath);
		if (outFile)
		{
			outFile << configJson.dump(4);
			outFile.close();
		}
		else
		{
			std::cerr << "Could not open the file for writing: " << filepath << std::endl;
		}

		m_files.clear();
	}

	void Editor::GetWindowSize(float& width, float& height)
	{
		width = m_renderWindowWidth;
		height = m_renderWindowHeight;
	}

	float Editor::GetPlayWindowOffset()
	{
		return m_playWindowOffset;
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

		std::vector<EntityID> temp = EntityManager::GetInstance().GetEntitiesInPool(ALL);

		for (auto n :temp)
		{
			if (n != Graphics::CameraManager::GetUiCameraId())
			{
				LogicSystem::DeleteScriptData(n);
				EntityManager::GetInstance().RemoveEntity(n);
			}
		}
	}

	void Editor::Init(GLFWwindow* p_window_)
	{
		//check imgui's version 
		IMGUI_CHECKVERSION();
		//create imgui context 
		ImGui::CreateContext();
		switch (m_currentStyle)
		{
		case GuiStyle::DARK:
			SetImGUIStyle_Dark();
			break;
		case GuiStyle::PINK:
			SetImGUIStyle_Pink();
			break;
		case GuiStyle::BLUE:
			SetImGUIStyle_Blue();
			break;
		}


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

		ResourceManager::GetInstance().LoadIconFromFile("../Assets/Icons/Directory_Icon.png", "../Assets/Icons/Directory_Icon.png");
		ResourceManager::GetInstance().LoadIconFromFile("../Assets/Icons/Audio_Icon.png"	, "../Assets/Icons/Audio_Icon.png");
		ResourceManager::GetInstance().LoadIconFromFile("../Assets/Icons/Font_Icon.png", "../Assets/Icons/Font_Icon.png");
		ResourceManager::GetInstance().LoadIconFromFile("../Assets/Icons/Prefabs_Icon.png", "../Assets/Icons/Prefabs_Icon.png");
		ResourceManager::GetInstance().LoadIconFromFile("../Assets/Icons/Other_Icon.png", "../Assets/Icons/Other_Icon.png");
		ResourceManager::GetInstance().LoadIconFromFile("../Assets/Icons/Texture_Icon.png", "../Assets/Icons/Texture_Icon.png");
	}

	void Editor::Render(Graphics::FrameBuffer& r_frameBuffer)
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
			if (m_showSceneView) ShowSceneView(r_frameBuffer, &m_showSceneView);

			//draw the stuff for ellie to test
			if (m_showTestWindows) ShowDemoWindow(&m_showTestWindows);

			//resource window for drag n drop
			if (m_showResourceWindow) ShowResourceWindow(&m_showResourceWindow);

			//performance window showing time used per system
			if (m_showPerformanceWindow) ShowPerformanceWindow(&m_showPerformanceWindow);

			if (m_showAnimationWindow) ShowAnimationWindow(&m_showAnimationWindow);

			if (m_showPhysicsWindow) ShowPhysicsWindow(&m_showPhysicsWindow);

			if (m_showGameView) ShowGameView(r_frameBuffer , &m_showGameView);

			if (m_applyPrefab) ShowApplyWindow(&m_applyPrefab);

			if (m_isPrefabMode && ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				ImGui::OpenPopup("Confirm Exit");
			}

			if (ImGui::BeginPopupModal("Confirm Exit"))
			{
				ImGui::Text("Are you sure you want to exit Prefab Editor mode?");
				ImGui::Separator();
				if (ImGui::Button("Yes"))
					ImGui::OpenPopup("Save prefab");					
				if (ImGui::BeginPopup/*Modal*/("Save prefab"))
				{
					ImGui::Text("Do you want to save your changes?");
					ImGui::Separator();
					if (ImGui::Selectable("Yes"))
					{
						if (EntityManager::GetInstance().Has<EntityDescriptor>(1))
						{
							auto save = serializationManager.SerializeEntityPrefab(1);
							prefabTP = EntityManager::GetInstance().Get<EntityDescriptor>(1).prefabType;
							prefabCID = EntityManager::GetInstance().GetComponentIDs(1);
							m_applyPrefab = true;

							std::ofstream outFile(prefabFP);
							if (outFile)
							{
								outFile << save.dump(4);
								outFile.close();
							}
						}
						m_isPrefabMode = false;
						ClearObjectList();
						serializationManager.LoadAllEntitiesFromFile("../Assets/Prefabs/savestate.json");
						engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
						ImGui::ClosePopupToLevel(0, true);
					}
					//ImGui::SameLine();
					ImGui::Separator();
					if (ImGui::Selectable("No"))
					{
						m_isPrefabMode = false;
						ClearObjectList();
						serializationManager.LoadAllEntitiesFromFile("../Assets/Prefabs/savestate.json");
						engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
						ImGui::ClosePopupToLevel(0, true);
					}
					ImGui::EndPopup();
					m_undoStack.ClearStack();
				}
				ImGui::SameLine();
				if (ImGui::Button("No"))
				{
					ImGui::ClosePopupToLevel(0, true);
				}

				ImGui::EndPopup();
			}

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

	void Editor::ShowLogsWindow(bool* p_active)
	{
		//if active
		if (IsEditorActive())
		if (!ImGui::Begin("Debug Log Window", p_active))
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
			ImGui::SameLine();
			if (ImGui::Button("Add Debug Text"))
			{
				AddErrorLog("Debug text");
				AddInfoLog("Debug text");
				AddWarningLog("Debug text");
				AddEventLog("Debug text");
				AddLog("Debug text");
			}
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

	void Editor::ShowConsoleWindow(bool* p_active)
	{
		if (IsEditorActive())
		if (!ImGui::Begin("Console Window", p_active)) // start drawing
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

	void Editor::ShowObjectWindow(bool* p_active)
	{
		if (IsEditorActive())
		if (!ImGui::Begin("Object Hierarchy Window", p_active)) // draw object list
		{
			ImGui::End(); //imgui close
		}
		else
		{
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
					if (n.first == Graphics::CameraManager::GetUiCameraId())
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
						if (hoveredObject)
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
								if (hoveredObject)
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
				if (ImGui::Selectable("Save As Prefab"))
				{
					auto save = serializationManager.SerializeEntityPrefab(static_cast<int>(m_currentSelectedObject));
					std::string filepath = "../Assets/Prefabs/";
					filepath += EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).name; // can change to request name or smth
					filepath += "_Prefab.json";
					std::ofstream outFile(filepath);
					if (outFile)
					{
						outFile << save.dump(4);
						outFile.close();
					}
				}
				if (ImGui::Selectable("Delete Object"))
				{
						AddInfoLog("Object Deleted");

						for (const auto& id : SceneView())
						{
							if (EntityManager::GetInstance().Get<EntityDescriptor>(id).parent && EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.value() == m_currentSelectedObject)
								EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.reset();
						}
						if (m_currentSelectedObject != -1)
						EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).HandicapEntity();

						//create undo here
						if (m_currentSelectedObject != -1)
						m_undoStack.AddChange(new DeleteObjectUndo(m_currentSelectedObject));

						//EntityManager::GetInstance().RemoveEntity(m_currentSelectedObject);
						//LogicSystem::DeleteScriptData(m_currentSelectedObject);
						//if not first index
						//m_currentSelectedObject != 1 ? m_currentSelectedObject -= 1 : m_currentSelectedObject = 0;
						m_currentSelectedObject = -1; // just reset it
						//if object selected
						m_objectIsSelected = false;
						//m_currentSelectedObject > -1 ? m_objectIsSelected = true : m_objectIsSelected = false;

						if (EntityManager::GetInstance().GetEntitiesInPool(ALL).empty()) m_currentSelectedObject = -1;//if nothing selected

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
						EntityID s_id = serializationManager.LoadFromFile("../Assets/Prefabs/EditorDefaults/Empty_Prefab.json");
						m_undoStack.AddChange(new CreateObjectUndo(s_id));
				}
				if (ImGui::BeginMenu("Create UI Object"))
				{
					if (ImGui::MenuItem("Create UI Object")) // the ctrl s is not programmed yet, need add to the key press event
					{
						EntityID s_id = serializationManager.LoadFromFile("../Assets/Prefabs/EditorDefaults/UIObject_Prefab.json");
						m_undoStack.AddChange(new CreateObjectUndo(s_id));
					}
					if (ImGui::MenuItem("Create UI Button")) // the ctrl s is not programmed yet, need add to the key press event
					{
						EntityID s_id = serializationManager.LoadFromFile("../Assets/Prefabs/EditorDefaults/Button_Prefab.json");
						m_undoStack.AddChange(new CreateObjectUndo(s_id));
					}
					if (ImGui::MenuItem("Create Text Object")) // the ctrl s is not programmed yet, need add to the key press event
					{
						EntityID s_id = serializationManager.LoadFromFile("../Assets/Prefabs/EditorDefaults/Text_Prefab.json");
						m_undoStack.AddChange(new CreateObjectUndo(s_id));
					}
					ImGui::EndMenu();
				}
				if (ImGui::Selectable("Create Audio Object"))
				{
					EntityID s_id = serializationManager.LoadFromFile("../Assets/Prefabs/EditorDefaults/Audio_Prefab.json");
					m_undoStack.AddChange(new CreateObjectUndo(s_id));
				}
				if (ImGui::Selectable("Create Camera Object"))
				{
					EntityID s_id = serializationManager.LoadFromFile("../Assets/Prefabs/EditorDefaults/Camera_Prefab.json");
					m_undoStack.AddChange(new CreateObjectUndo(s_id));
				}
				ImGui::EndPopup();
			}

			ImGui::End();
		}
	}

	//temporary hardcoded stuff for testing for milestone 2
	void Editor::ShowDemoWindow(bool* p_active)
	{
		if (IsEditorActive())
		if (!ImGui::Begin("Rubric Test Window", p_active, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
		}
		else
		{			
			ImGui::SeparatorText("Scenes To Test");
			if (ImGui::Button("Reset Default Scene"))
			{
				LoadSceneFromGivenPath("../Assets/RubricTestScenes/DefaultScene.json");
			}
			ImGui::SameLine();
			if (ImGui::Button("Undo/Sceen Picking"))
			{
				LoadSceneFromGivenPath("../Assets/RubricTestScenes/UndoTest.json");
			}
			if (ImGui::Button("Prefab Editor Test Scene"))
			{
				LoadSceneFromGivenPath("../Assets/RubricTestScenes/Demo_Scene.json");
			}
			ImGui::SameLine();
			if (ImGui::Button("Layer Test Scene"))
			{
				LoadSceneFromGivenPath("../Assets/RubricTestScenes/LayerTest.json");
			}
			if (ImGui::Button("Script Test Scene 1"))
			{
				LoadSceneFromGivenPath("../Assets/RubricTestScenes/LogicScene1.json");
			}
			ImGui::SameLine();
			if (ImGui::Button("Script Test Scene 2"))
			{
				LoadSceneFromGivenPath("../Assets/RubricTestScenes/ChainPickUpScene.json");
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds 10 pixels of vertical space
			ImGui::End();
		}
	}

	void Editor::ShowComponentWindow(bool* p_active)
	{
		if (IsEditorActive())
		if (!ImGui::Begin("Property Editor Window", p_active, IsEditorActive() ? 0 : ImGuiWindowFlags_NoInputs) || (m_isPrefabMode && !EntityManager::GetInstance().Has<EntityDescriptor>(1)))
		{
			ImGui::End();
		}
		else
		{
			if (ImGui::BeginChild("GameComponentList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | (IsEditorActive() ? 0 : ImGuiWindowFlags_NoInputs)))
			{
				if (m_objectIsSelected || m_isPrefabMode)
				{
					EntityID entityID = (m_isPrefabMode)? 1 : m_currentSelectedObject;
					std::vector<ComponentID> components = EntityManager::GetInstance().GetComponentIDs(entityID);
					int componentCount = 0; //unique id for imgui objects
					bool hasScripts = false;
					for (const ComponentID& name : components)
					{
						++componentCount;//increment unique id

						


						// ---------- ENTITY DESCRIPTOR ---------- //
						ImGui::SetNextItemAllowOverlap(); // allow the stacking of buttons

						if (name == EntityManager::GetInstance().GetComponentID<EntityDescriptor>())
						{


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
									else if (vp.get_type().get_name() == "bool")
									{
										bool tmp = vp.get_value<bool>();
										std::string str = "##" + prop.get_name().to_string();
										ImGui::SameLine(); ImGui::Checkbox(str.c_str(), &tmp);
										prop.set_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID), tmp);
									}
								}
							}
						}


						// ---------- TRANSFORM ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<Transform>() && EntityManager::GetInstance().Has<Transform>(entityID))
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
									if (ImGui::Selectable("Remove"))
									{
										EntityManager::GetInstance().Remove<Transform>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());

								if (EntityManager::GetInstance().Has<Transform>(entityID))
								{
									for (auto& prop : currType.get_properties())
									{
										if (prop.get_name() == "Position" && EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent ||
											prop.get_name() == "Relative Position" && !EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent)
											continue;
										if (prop.get_name() == "Orientation" && EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent ||
											prop.get_name() == "Relative Orientation" && !EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent)
											continue;

										// hide properties if entity has a certain component
										if (prop.get_name() == "Orientation")
										{
											if (EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<GUI>()))
												continue;
											if (EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<TextComponent>()))
												continue;
										}

										if (prop.get_name() == "Width" || prop.get_name() == "Height")
										{
											if (EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Graphics::Camera>()))
												continue;
										}

										ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
										std::string nm(prop.get_name());
										nm += ": ";
										ImGui::Text(nm.c_str());
										rttr::variant vp = prop.get_value(EntityManager::GetInstance().Get<Transform>(entityID));

										static float prevVal{};
										// handle types
										if (vp.get_type().get_name() == "structPE::vec2")
										{
											PE::vec2 tmp = vp.get_value<PE::vec2>();
											ImGui::Text("x: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);
											ImGui::DragFloat(("##x" + prop.get_name().to_string()).c_str(), &tmp.x, 1.0f);
											if (ImGui::IsItemActivated())
											{
												prevVal = vp.get_value<PE::vec2>().x;
												std::cout << prevVal << std::endl;
											}
											if (ImGui::IsItemDeactivatedAfterEdit() && prevVal != tmp.x)
											{
												std::cout << "Edited" << std::endl;
												m_undoStack.AddChange(new ValueChange<float>(prevVal, tmp.x, &EntityManager::GetInstance().Get<Transform>(entityID).position.x));
											}
											ImGui::Text("y: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);
											ImGui::DragFloat(("##y" + prop.get_name().to_string()).c_str(), &tmp.y, 1.0f);
											if (ImGui::IsItemActivated())
											{
												prevVal = vp.get_value<PE::vec2>().y;
												std::cout << prevVal << std::endl;
											}
											if (ImGui::IsItemDeactivatedAfterEdit() && prevVal != tmp.y)
											{
												m_undoStack.AddChange(new ValueChange<float>(prevVal, tmp.y, &EntityManager::GetInstance().Get<Transform>(entityID).position.y));
											}
											prop.set_value(EntityManager::GetInstance().Get<Transform>(entityID), tmp);
										}
										else if (vp.get_type().get_name() == "float")
										{
											float tmp = vp.get_value<float>();
											std::string str = "##" + prop.get_name().to_string();
											ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);
											ImGui::DragFloat(str.c_str(), &tmp, 1.0f);
											if (ImGui::IsItemActivated())
											{
												prevVal = vp.get_value<float>();
												std::cout << prevVal << std::endl;
											}
											if (ImGui::IsItemDeactivatedAfterEdit() && prevVal != tmp)
											{
												if (prop.get_name() == "Width")
													m_undoStack.AddChange(new ValueChange<float>(prevVal, tmp, &EntityManager::GetInstance().Get<Transform>(entityID).width));
												if (prop.get_name() == "Height")
													m_undoStack.AddChange(new ValueChange<float>(prevVal, tmp, &EntityManager::GetInstance().Get<Transform>(entityID).height));
												if (prop.get_name() == "Orientation")
													m_undoStack.AddChange(new ValueChange<float>(prevVal, tmp, &EntityManager::GetInstance().Get<Transform>(entityID).orientation));
											}
											prop.set_value(EntityManager::GetInstance().Get<Transform>(entityID), tmp);
										}
									}
								}
							}
						}


						// ---------- RIGID BODY ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<RigidBody>() && EntityManager::GetInstance().Has<RigidBody>(entityID))
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
									if (ImGui::Selectable("Remove"))
									{
										EntityManager::GetInstance().Remove<RigidBody>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								if (EntityManager::GetInstance().Has<RigidBody>(entityID))
								{
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
						}


						// ---------- COLLIDER ---------- //
						

						if (name == EntityManager::GetInstance().GetComponentID<Collider>() && EntityManager::GetInstance().Has<Collider>(entityID))
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
									if (ImGui::Selectable("Remove"))
									{
										EntityManager::GetInstance().Remove<Collider>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								if (EntityManager::GetInstance().Has<Collider>(entityID))
								{
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
									ImGui::Checkbox("Is Trigger", &EntityManager::GetInstance().Get<Collider>(entityID).isTrigger);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
                                    
                                   
								    auto& collisionLayers{ CollisionLayerManager::GetInstance().GetCollisionLayers() };

								    std::vector<const char*> layerNames;

								    for (auto& collisionLayer : collisionLayers)
								    {
								    	layerNames.push_back(collisionLayer->GetCollisionLayerName().c_str());
								    }

								    int currentLayerIndex = static_cast<int>(EntityManager::GetInstance().Get<Collider>(entityID).collisionLayerIndex);
								    ImGui::Text("Collision Layer: "); ImGui::SameLine();
								    ImGui::SetNextItemWidth(200.0f);
								    //set combo box for the different collider types
								    if (ImGui::Combo("##Collision Layers", &currentLayerIndex, layerNames.data(), static_cast<unsigned>(layerNames.size())))
								    {
								    	EntityManager::GetInstance().Get<Collider>(entityID).collisionLayerIndex = currentLayerIndex;
								    }
							
								}
}
						}

						// ---------- RENDERER ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<Graphics::Renderer>() && EntityManager::GetInstance().Has<Graphics::Renderer>(entityID))
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
									if (ImGui::Selectable("Remove"))
									{
										EntityManager::GetInstance().Remove<Graphics::Renderer>(entityID);
									}
									ImGui::EndPopup();
								}
								if (EntityManager::GetInstance().Has<Graphics::Renderer>(entityID))
								{

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
										if (ImGui::BeginChild("currentTexture", ImVec2{ 116,116 }, true))
										{
											if (EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetTextureKey() != "")
											{
												ImGui::Image((void*)(intptr_t)ResourceManager::GetInstance().GetTexture(EntityManager::GetInstance().Get<Graphics::Renderer>(entityID).GetTextureKey())->GetTextureID(), ImVec2{ 100,100 }, { 0,1 }, { 1,0 });
											}
										}
										ImGui::EndChild();

										// checks if mouse if hovering the texture preview - to use for asset browser drag n drop
										if (ImGui::IsItemHovered())
										{
											m_entityToModify = std::make_pair<std::string, int>("Renderer", static_cast<int>(entityID));
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
						}

						
						// ---------- GUI RENDERER ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>() && EntityManager::GetInstance().Has<Graphics::GUIRenderer>(entityID))
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
									if (ImGui::Selectable("Remove"))
									{
										EntityManager::GetInstance().Remove<Graphics::GUIRenderer>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(entityID))
								{
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

										// checks if mouse if hovering the texture preview - to use for asset browser drag n drop
										if (ImGui::IsItemHovered())
										{
											m_entityToModify = std::make_pair<std::string, int>("GUIRenderer", static_cast<int>(entityID));
										}

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
						}


						// ---------- SCRIPT COMPONENT ---------- //
						

						if (name == EntityManager::GetInstance().GetComponentID<ScriptComponent>() && EntityManager::GetInstance().Has<ScriptComponent>(entityID))
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
									if (ImGui::Selectable("Remove"))
									{
										LogicSystem::DeleteScriptData(entityID);
										EntityManager::GetInstance().Remove<ScriptComponent>(entityID);
										hasScripts = false;
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								//setting keys
								if (EntityManager::GetInstance().Has<ScriptComponent>(entityID))
								{
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
										EntityManager::GetInstance().Get<ScriptComponent>(entityID).addScript(key[scriptindex], m_currentSelectedObject);
									}
									ImGui::PopStyleColor(1);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Separator();
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									static int selectedScript{ -1 };
									static std::string selectedScriptName{};
									ImGui::Text("Scripts List");
									//loop to show all the items ins the vector
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
									if (ImGui::Button("Remove Script"))
									{
										if (selectedScript >= 0)
										{
											EntityManager::GetInstance().Get<ScriptComponent>(entityID).removeScript(selectedScriptName, m_currentSelectedObject);
											selectedScript = -1;
										}
									}
									ImGui::PopStyleColor(1);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								}
							}
						}
                        
						
						// ---------- GUI ---------- //


						if (name == EntityManager::GetInstance().GetComponentID<GUI>() && EntityManager::GetInstance().Has<GUI>(entityID))
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
									if (ImGui::Selectable("Remove"))
									{
										EntityManager::GetInstance().Remove<GUI>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Checkbox("Disabled", &EntityManager::GetInstance().Get<GUI>(entityID).disabled);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								// Vector of filepaths that have already been loaded - used to refer to later when needing to change the object's texture
								std::vector<std::filesystem::path> filepaths;
								int i{ 0 };
								int defaultText{ 0 };
								int hoveredText{ 0 };
								int disabledText{ 0 };
								int pressedTexture{ 0 };
								for (auto it = ResourceManager::GetInstance().Textures.begin(); it != ResourceManager::GetInstance().Textures.end(); ++it, ++i)
								{
									filepaths.emplace_back(it->first);
									if (it->first == EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).GetTextureKey())
										defaultText = i;
									if (it->first == EntityManager::GetInstance().Get<GUI>(entityID).m_hoveredTexture)
										hoveredText = i;
									if (it->first == EntityManager::GetInstance().Get<GUI>(entityID).m_pressedTexture)
										pressedTexture = i;
									if (it->first == EntityManager::GetInstance().Get<GUI>(entityID).m_disabledTexture)
										disabledText = i;
								}

								// Vector of the names of textures that have already been loaded
								std::vector<std::string> loadedTextureKeys;

								// get the keys of textures already loaded by the resource manager
								for (auto const& r_filepath : filepaths)
								{
									loadedTextureKeys.emplace_back(r_filepath.stem().string());
								}
								if (!loadedTextureKeys.empty())
								{
									ImGui::Text("Default Texture:");
									if (ImGui::BeginCombo("##defaulttext", loadedTextureKeys[defaultText].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str())) {
												EntityManager::GetInstance().Get<Graphics::GUIRenderer>(entityID).SetTextureKey(filepaths[n].string());
												EntityManager::GetInstance().Get<GUI>(entityID).m_defaultTexture = filepaths[n].string();
											}
										}
										ImGui::EndCombo();
									}

									//get and set color variable of the renderer component
									vec4 defaultColor = EntityManager::GetInstance().Get<GUI>(entityID).m_defaultColor;
									ImVec4 defaultcolor(defaultColor.x, defaultColor.y, defaultColor.z, defaultColor.w);

									ImGui::Text("default Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Default Color", (float*)&defaultcolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUI>(entityID).m_defaultColor = vec4(defaultcolor.x, defaultcolor.y, defaultcolor.z, defaultcolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Hovered Texture:");
									if (ImGui::BeginCombo("##hoveredtext", loadedTextureKeys[hoveredText].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str())) 
											{
												EntityManager::GetInstance().Get<GUI>(entityID).m_hoveredTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 hoverColor = EntityManager::GetInstance().Get<GUI>(entityID).m_hoveredColor;
									ImVec4 hovercolor(hoverColor.x, hoverColor.y, hoverColor.z, hoverColor.w);

									ImGui::Text("hover Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Hover Color", (float*)&hovercolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUI>(entityID).m_hoveredColor = vec4(hovercolor.x, hovercolor.y, hovercolor.z, hovercolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Pressed Texture:");
									if (ImGui::BeginCombo("##pressedtext", loadedTextureKeys[pressedTexture].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
											{
												EntityManager::GetInstance().Get<GUI>(entityID).m_pressedTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 pressColor = EntityManager::GetInstance().Get<GUI>(entityID).m_pressedColor;
									ImVec4 presscolor(pressColor.x, pressColor.y, pressColor.z, pressColor.w);

									ImGui::Text("Press Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Press Color", (float*)&presscolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUI>(entityID).m_pressedColor = vec4(presscolor.x, presscolor.y, presscolor.z, presscolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Disabled Texture:");
									if (ImGui::BeginCombo("##disabledtext", loadedTextureKeys[disabledText].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
											{
												EntityManager::GetInstance().Get<GUI>(entityID).m_disabledTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 disableColor = EntityManager::GetInstance().Get<GUI>(entityID).m_disabledColor;
									ImVec4 disablecolor(disableColor.x, disableColor.y, disableColor.z, disableColor.w);

									ImGui::Text("Disable Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change disable Color", (float*)&disablecolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUI>(entityID).m_disabledColor = vec4(disablecolor.x, disablecolor.y, disablecolor.z, disablecolor.w);


								}

													

								ImGui::SeparatorText("Events");

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


						if (name == EntityManager::GetInstance().GetComponentID<Graphics::Camera>() && EntityManager::GetInstance().Has<Graphics::Camera>(entityID)) {
							if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								Graphics::Camera& cameraComponent{ EntityManager::GetInstance().Get<Graphics::Camera>(entityID) };
								float viewportWidth{ cameraComponent.GetViewportWidth() };
								float viewportHeight{ cameraComponent.GetViewportHeight() };
								bool isMainCamera{ cameraComponent.GetIsMainCamera() };

								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Checkbox("Is Main Camera: ", &isMainCamera); // bool to set this camera as the main cam
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								// commented out for now
								//ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								//ImGui::Text("Viewport Dimensions: ");
								//ImGui::Text("Width: "); ImGui::SameLine(); ImGui::InputFloat("##View Width", &viewportWidth, 1.0f, 100.f, "%.3f");
								//ImGui::Text("Height: "); ImGui::SameLine(); ImGui::InputFloat("##View Height", &viewportHeight, 1.0f, 100.f, "%.3f");
								//ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								float zoom{ EntityManager::GetInstance().Get<Graphics::Camera>(entityID).GetMagnification() };
								ImGui::Text("Zoom: "); ImGui::SameLine(); ImGui::InputFloat("##Zoom", &zoom, 1.0f, 100.f, "%.3f");
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								cameraComponent.SetViewDimensions(viewportWidth, viewportHeight);
								cameraComponent.SetMagnification(zoom);
								cameraComponent.SetMainCamera(isMainCamera);
							}
						}

						// ---------- ANIMATION COMPONENT ---------- //

						if (name == EntityManager::GetInstance().GetComponentID<AnimationComponent>() && EntityManager::GetInstance().Has<AnimationComponent>(entityID))
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
									if (ImGui::Selectable("Remove"))
									{
										EntityManager::GetInstance().Remove<AnimationComponent>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f)); //add space

								// setting animations
								std::vector<const char*> key;
								key.push_back("");
								if (EntityManager::GetInstance().Has<AnimationComponent>(entityID))
								{
									//to get all the animation keys for the entity
									for (auto const& tmp : EntityManager::GetInstance().Get<AnimationComponent>(entityID).GetAnimationList())
									{
										key.push_back(tmp.c_str());
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
											EntityManager::GetInstance().Get<AnimationComponent>(entityID).SetCurrentAnimationID(key[index]);
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

						//// ---------- Audio Component ---------- //
						if (name == EntityManager::GetInstance().GetComponentID<AudioComponent>())
						{
							if (ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								AudioComponent& audioComponent = EntityManager::GetInstance().Get<AudioComponent>(entityID);

								// Vector of filepaths for audio files
								std::vector<std::filesystem::path> audioFilePaths;
								int index = -1; // Initialize with -1 to indicate no selection
								int i = 0;

								for (auto it = ResourceManager::GetInstance().Sounds.begin(); it != ResourceManager::GetInstance().Sounds.end(); ++it, ++i)
								{
									audioFilePaths.emplace_back(it->first);
									// If current entity's audio component is using this audio file, remember its index
									if (it->first == EntityManager::GetInstance().Get<AudioComponent>(entityID).GetAudioKey())
										index = i;
								}

								// Vector of the names of audio files
								std::vector<std::string> loadedAudioKeys;

								// Get the keys of audio already loaded by the resource manager
								for (auto const& r_filepath : audioFilePaths)
								{
									loadedAudioKeys.emplace_back(r_filepath.filename().string());
								}

								// Placeholder combo box label
								std::string comboBoxLabel = "Drag-in or Select an audio...";
								if (index >= 0 && index < loadedAudioKeys.size())
								{
									comboBoxLabel = loadedAudioKeys[index];
								}

								// Always show the dropdown box
								ImGui::Text("Audio: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.0f);

								// Create combo box even if loadedAudioKeys is empty
								if (ImGui::BeginCombo("##Audio", comboBoxLabel.c_str()))
								{
									if (!loadedAudioKeys.empty())
									{
										for (int n = 0; n < loadedAudioKeys.size(); ++n)
										{
											bool isSelected = (comboBoxLabel == loadedAudioKeys[n]);
											if (ImGui::Selectable(loadedAudioKeys[n].c_str(), isSelected))
											{
												audioComponent.StopSound();
												audioComponent.SetAudioKey(audioFilePaths[n].string());
												comboBoxLabel = loadedAudioKeys[n];

												if (isSelected)
												{
													ImGui::SetItemDefaultFocus();
												}
											}
										}
									}
									ImGui::EndCombo();
								}

								// Check if mouse is hovering over the dropdown box for drag and drop
								if (ImGui::IsItemHovered())
								{
									m_entityToModify = std::make_pair<std::string, int>("Audio", static_cast<int>(entityID));
								}

								// Audio playback controls
								bool isLooping = audioComponent.IsLooping();
								ImGui::Checkbox("Loop", &isLooping);
								audioComponent.SetLoop(isLooping);

								if (ImGui::Button("Play"))
								{
									audioComponent.PlayAudioSound();
								}

								ImGui::SameLine();
								if (audioComponent.IsPaused())
								{
									if (ImGui::Button("Resume"))
									{
										audioComponent.ResumeSound();
									}
								}
								else
								{
									if (ImGui::Button("Pause"))
									{
										audioComponent.PauseSound();
									}
								}
								ImGui::SameLine();
								if (ImGui::Button("Stop"))
								{
									audioComponent.StopSound();
								}

								// Volume control for selected sound
								static float volume = 1.0f;
								if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f))
								{
									audioComponent.SetVolume(volume);
								}

								// Global volume control (affecting all sounds)
								static float globalVolume = 1.0f;
								if (ImGui::SliderFloat("Global Volume", &globalVolume, 0.0f, 1.0f))
								{
									AudioManager::GetInstance().SetGlobalVolume(globalVolume);
								}
							}
						}

						// ---------- Text Component ---------- //

						if (name == EntityManager::GetInstance().GetComponentID<TextComponent>() && EntityManager::GetInstance().Has<TextComponent>(entityID))
						{
							if (ImGui::CollapsingHeader("Text", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								//setting reset button to open a popup with selectable text
								ImGui::SameLine();
								std::string id = "options##", o = "o##";
								id += std::to_string(componentCount);
								o += std::to_string(componentCount);
								if (ImGui::BeginPopup(id.c_str()))
								{
									if (ImGui::Selectable("Reset")) {}
									if (ImGui::Selectable("Remove"))
									{
										EntityManager::GetInstance().Remove<TextComponent>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								if (EntityManager::GetInstance().Has<TextComponent>(entityID))
								{
									// Text Box
									std::string stringBuffer{ EntityManager::GetInstance().Get<TextComponent>(entityID).GetText() };
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Text: ");
									ImGui::InputTextMultiline("##Text", &stringBuffer, ImVec2(300.0f, 100.0f));

									EntityManager::GetInstance().Get<TextComponent>(entityID).SetText(stringBuffer);

									// Vector of filepaths that have already been loaded - used to refer to later when needing to change the object's fonts
									std::vector<std::filesystem::path> filepaths;
									int i{ 0 };
									int fontIndex{ 0 };
									for (auto it = ResourceManager::GetInstance().Fonts.begin(); it != ResourceManager::GetInstance().Fonts.end(); ++it, ++i)
									{
										filepaths.emplace_back(it->first);
										if (it->first == EntityManager::GetInstance().Get<TextComponent>(entityID).GetFontKey())
											fontIndex = i;
									}

									// Vector of the names of fonts that have already been loaded
									std::vector<std::string> fontTextureKeys;

									// get the keys of fonts already loaded by the resource manager
									for (auto const& r_filepath : filepaths)
									{
										fontTextureKeys.emplace_back(r_filepath.stem().string());
									}

									//// Setting fonts
									if (!fontTextureKeys.empty())
									{
										// create a combo box of Font ids
										ImGui::Text("Font: "); ImGui::SameLine();
										ImGui::SetNextItemWidth(200.0f);
										bool bl{};
										if (EntityManager::GetInstance().Get<TextComponent>(entityID).GetFontKey() != "")
										{
											bl = ImGui::BeginCombo("##FontTextures", fontTextureKeys[fontIndex].c_str()); // The second parameter is the label previewed before opening the combo.
										}
										else
										{
											bl = ImGui::BeginCombo("##FontTextures", ""); // The second parameter is the label previewed before opening the combo.
										}
										if (bl)
										{
											for (int n{ 0 }; n < fontTextureKeys.size(); ++n)
											{
												if (ImGui::Selectable(fontTextureKeys[n].c_str()))
													EntityManager::GetInstance().Get<TextComponent>(entityID).SetFont(filepaths[n].string());
											}
											ImGui::EndCombo();
										}
									}

									// Load font through file explorer
									ImGui::SameLine();
									if (ImGui::Button("Load"))
									{
										std::string filePath = serializationManager.OpenFileExplorerRequestPath();

										// Check if filePath is not empty
										if (!filePath.empty())
										{
											std::replace(filePath.begin(), filePath.end(), '\\', '/');
											filePath = ".." + filePath.substr(filePath.find("/Assets/"), filePath.find(".") - filePath.find("/Assets/")) + ".ttf";

											ResourceManager::GetInstance().LoadFontFromFile(filePath, filePath);
										}
										else
										{
											std::cerr << "No file path was selected for loading." << std::endl;
										}
									}
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Separator();
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

									float size{ EntityManager::GetInstance().Get<TextComponent>(entityID).GetSize() };
									ImGui::Text("Font Size: "); ImGui::SameLine(); ImGui::InputFloat("##FontSize", &size, 1.0f, 100.f, "%.3f");
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

									EntityManager::GetInstance().Get<TextComponent>(entityID).SetSize(size);

									// Color

									// get and set color variable of the text component
									ImVec4 color;
									color.x = EntityManager::GetInstance().Get<TextComponent>(entityID).GetColor().r;
									color.y = EntityManager::GetInstance().Get<TextComponent>(entityID).GetColor().g;
									color.z = EntityManager::GetInstance().Get<TextComponent>(entityID).GetColor().b;
									color.w = EntityManager::GetInstance().Get<TextComponent>(entityID).GetColor().a;

									ImGui::Text("Change Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Color Text", (float*)&color, ImGuiColorEditFlags_AlphaPreview);

								    EntityManager::GetInstance().Get<TextComponent>(entityID).SetColor(color.x, color.y, color.z, color.w);

									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								}
							}
						}
					}

					if (hasScripts)
					{
						m_currentSelectedObject = (m_isPrefabMode) ? 1 : m_currentSelectedObject;
						for (auto& [key, val] : LogicSystem::m_scriptContainer)
						{
							if (key == "GameStateController")
							{
								GameStateController* p_Script = dynamic_cast<GameStateController*>(val);
								auto it = p_Script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_Script->GetScriptData().end())
									if (ImGui::CollapsingHeader("GameStateController", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										ImGui::Text("Game State Active: "); ImGui::SameLine(); ImGui::Checkbox("##act",&it->second.GameStateManagerActive);
										int splashScreenId = static_cast<int> (it->second.SplashScreen);
										int executingStatementId = static_cast<int> (it->second.executingStatement);
										int mapOverlayId = static_cast<int> (it->second.mapOverlay);
										int pawOverlayId = static_cast<int> (it->second.pawOverlay);
										int foliageOverlayId = static_cast<int> (it->second.foliageOverlay);
										int energyHeaderId = static_cast<int> (it->second.energyHeader);
										int currentEnergyTextId = static_cast<int> (it->second.currentEnergyText);
										int slashTextId = static_cast<int> (it->second.slashText);
										int maxEnergyTextId = static_cast<int> (it->second.maxEnergyText);
										int energyBackgroundId = static_cast<int> (it->second.energyBackground);
										int turnNumberTextId = static_cast<int> (it->second.turnNumberText);
										int planAttackTextId = static_cast<int> (it->second.planAttackText);
										int planMovementTextId = static_cast<int> (it->second.planMovementText);
										int turnBackgroundId = static_cast<int> (it->second.turnBackground);
										int endTurnButtonId = static_cast<int> (it->second.endTurnButton);
										int endMovementTextId = static_cast<int> (it->second.endMovementText);
										int endTurnTextId = static_cast<int> (it->second.endTurnText);
										// Entity IDs
										ImGui::Text("SplashScreen ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##sscreen", &splashScreenId);
										if (splashScreenId != m_currentSelectedObject) { it->second.SplashScreen = splashScreenId; }
										ImGui::Text("Execution HUD: ");
										ImGui::Text("Executing Statement ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##executing", &executingStatementId);
										if (executingStatementId != m_currentSelectedObject) { it->second.executingStatement = executingStatementId; }
										ImGui::Text("Foliage Overlay ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##foliageoverlay", &foliageOverlayId);
										if (foliageOverlayId != m_currentSelectedObject) { it->second.foliageOverlay = foliageOverlayId; }
										ImGui::Text("Planning HUD: ");
										ImGui::Text("Map Overlay ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##mapoverlay", &mapOverlayId);
										if (mapOverlayId != m_currentSelectedObject) { it->second.mapOverlay = mapOverlayId; }
										ImGui::Text("Paw Overlay ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##pawoverlay", &pawOverlayId);
										if (pawOverlayId != m_currentSelectedObject) { it->second.pawOverlay = pawOverlayId; }
										ImGui::Text("Energy: ");
										ImGui::Text("Energy Header Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##energyHeader", &energyHeaderId);
										if (energyHeaderId != m_currentSelectedObject) { it->second.energyHeader = energyHeaderId; }
										ImGui::Text("Current Energy Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##currentenergy", &currentEnergyTextId);
										if (currentEnergyTextId != m_currentSelectedObject) { it->second.currentEnergyText = currentEnergyTextId; }
										ImGui::Text("Backslash Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##slashText", &slashTextId);
										if (slashTextId != m_currentSelectedObject) { it->second.slashText = slashTextId; }
										ImGui::Text("Maximum Energy Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##maxenergy", &maxEnergyTextId);
										if (maxEnergyTextId != m_currentSelectedObject) { it->second.maxEnergyText = maxEnergyTextId; }
										ImGui::Text("Energy Background ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##energybackground", &energyBackgroundId);
										if (energyBackgroundId != m_currentSelectedObject) { it->second.energyBackground = energyBackgroundId; }
										ImGui::Text("Turn count: ");
										ImGui::Text("Turn Number Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##turnnumber", &turnNumberTextId);
										if (turnNumberTextId != m_currentSelectedObject) { it->second.turnNumberText = turnNumberTextId; }
										ImGui::Text("Plan Attack Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##planattack", &planAttackTextId);
										if (planAttackTextId != m_currentSelectedObject) { it->second.planAttackText = planAttackTextId; }
										ImGui::Text("Plan Movement Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##planmovement", &planMovementTextId);
										if (planMovementTextId != m_currentSelectedObject) { it->second.planMovementText = planMovementTextId; }
										ImGui::Text("Phase Button: ");
										ImGui::Text("Turn Background ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##turnbg", &turnBackgroundId);
										if (turnBackgroundId != m_currentSelectedObject) { it->second.turnBackground = turnBackgroundId; }
										ImGui::Text("End Turn Button ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##endturnbutton", &endTurnButtonId);
										if (endTurnButtonId != m_currentSelectedObject) { it->second.endTurnButton = endTurnButtonId; }
										ImGui::Text("End Movement Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##endmovementtext", &endMovementTextId);
										if (endMovementTextId != m_currentSelectedObject) { it->second.endMovementText = endMovementTextId; }
										ImGui::Text("End Turn Text ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##endturntext", &endTurnTextId);
										if (endTurnTextId != m_currentSelectedObject) { it->second.endTurnText = endTurnTextId; }
									}
							}

							if (key == "testScript")
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

							if (key == "FollowScript")
							{
								FollowScript* p_Script = dynamic_cast<FollowScript*>(val);
								auto it = p_Script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_Script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("FollowScript", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										ImGui::SetNextItemWidth(100.0f);
										ImGui::InputInt("Distance Offset", &it->second.Size,0,0);
										ImGui::Checkbox("Look Towards", &it->second.LookTowardsMovement);
										int j = it->second.NumberOfFollower;
										ImGui::Text("Number of Follower + 1: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##ff",&j);
										if (j <= 5 && j >= 0)
										{
											it->second.NumberOfFollower = j;
										}
										else
										{
											it->second.NumberOfFollower = 5;
										}

										for (int i = 0; i < it->second.NumberOfFollower; i++)
										{
											if (i != 0)
											{
												int id = static_cast<int> (it->second.FollowingObject[i]);
												std::string test = std::string("##id") + std::to_string(i);
												ImGui::Text("Follower ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt(test.c_str(), &id);
												if(id != m_currentSelectedObject)
												it->second.FollowingObject[i] = id;
											}
										}

										ImGui::Checkbox("isAttaching", & it->second.IsAttaching);
										if (it->second.IsAttaching)
										{
											j = it->second.NumberOfAttachers;
											ImGui::Text("Number of Attacher + 1: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##aa", &j);
											if (j <= 5 && j >= 0)
											{
												it->second.NumberOfAttachers = j;
											}
											else
											{
												it->second.NumberOfAttachers = 5;
											}

											for (int i = 0; i < it->second.NumberOfAttachers; i++)
											{
												if (i != 0)
												{
													int id = static_cast<int> (it->second.ToAttach[i]);
													std::string test = std::string("##id2") + std::to_string(i);
													ImGui::Text("Attacher ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt(test.c_str(), &id);
													if (id != m_currentSelectedObject)
														it->second.ToAttach[i] = id;
												}
											}

											int id = static_cast<int> (it->second.SoundID);
											ImGui::Text("Sound ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##soundid", &id);
											if (id != m_currentSelectedObject)
												it->second.SoundID = id;
										}
									}
								}
							}
						
							if (key == "CameraManagerScript")
							{
								CameraManagerScript* p_Script = dynamic_cast<CameraManagerScript*>(val);
								auto it = p_Script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_Script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("FollowScript", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										int j = it->second.NumberOfCamera+1;
										ImGui::Text("Number of Camera: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##ff", &j);
										if (j <= 11 && j >= 1)
										{
											it->second.NumberOfCamera = j-1;
										}
										else
										{
											it->second.NumberOfCamera = 9;
										}

										for (int i = 0; i <= it->second.NumberOfCamera; i++)
										{
											int id = static_cast<int> (it->second.CameraIDs[i]);
											std::string test = std::string("##ids") + std::to_string(i);
											ImGui::Text("Follower ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt(test.c_str(), &id);
											if (id != m_currentSelectedObject)
												it->second.CameraIDs[i] = id;
										}
									}
								}

							}

							if (key == "CatScript")
							{
								CatScript* p_script = dynamic_cast<CatScript*>(val);
								auto it = p_script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader(key.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
											
										ImGui::Text("Is Main Cat: "); ImGui::SameLine(); ImGui::Checkbox("##maincat", &it->second.isMainCat);

										// cat stats
										ImGui::Text("Cat Stats");
										ImGui::Text("Cat Health: "); ImGui::SameLine(); ImGui::DragInt("##cathealth", &it->second.catHealth);
										ImGui::Text("Cat Max Energy "); ImGui::SameLine(); ImGui::DragInt("##catmaxenergy", &it->second.catMaxEnergy);
										ImGui::Text("Cat Speed "); ImGui::SameLine(); ImGui::DragFloat("##catspeed", &it->second.movementSpeed);

										// attack variables
										ImGui::Text("Attack Stats");
										ImGui::Text("Attack Damage: "); ImGui::SameLine(); ImGui::DragInt("##attackdamage", &it->second.attackDamage);
										ImGui::Text("Required Attack Points: "); ImGui::SameLine(); ImGui::DragInt("##attackpoints", &it->second.requiredAttackPoints);

										// Projectile variabls
										ImGui::Text("Projectile Stats");
										ImGui::Text("Bullet Fire Delay: "); ImGui::SameLine(); ImGui::DragFloat("##bulletdelay", &it->second.bulletDelay);
										ImGui::Text("Range: "); ImGui::SameLine(); ImGui::DragFloat("##projectilerange", &it->second.bulletRange);
										ImGui::Text("Lifetime: "); ImGui::SameLine(); ImGui::DragFloat("##projectilelifetime", &it->second.bulletLifeTime);
										ImGui::Text("Force: "); ImGui::SameLine(); ImGui::DragFloat("##projectileforce", &it->second.bulletForce);									
										

										ImGui::Separator();
										int num{};
										ImGui::Text("Add Animation state"); ImGui::SameLine();
										bool worked{ false };
										if (ImGui::Button("+"))
										{
											std::string str = "NewState";
											while (!worked)
											{
												if (it->second.animationStates.count(str))
												{
													str += 1;
												}
												else
												{
													it->second.animationStates.emplace(str, "");
													worked = true;
												}
											}
										}
										static std::pair<std::string, std::string> whoToRemove;
										static bool rmFlag{ false };
										for (auto& [k, v] : it->second.animationStates)
										{
											ImGui::Text("State: "); ImGui::SameLine(); 
											std::string curr = (whoToRemove.first == k ? whoToRemove.first : k);
											bool changed = ImGui::InputText(std::string("##" + k + std::to_string(++num)).c_str(), &curr);
											if (!changed)
											{
												if (whoToRemove.first == k)
												{
													if (!ImGui::IsItemActivated())
													{
														rmFlag = true;
													}
												}
											}
											else
											{
												if (k != curr)
												{
													whoToRemove.first = k;
													whoToRemove.second = curr;
													rmFlag = false;
												}
												
											}
											
										
											ImGui::Text("Animation: "); ImGui::SameLine();
											bool bl = ImGui::BeginCombo(std::string("##Animation" + k + std::to_string(num)).c_str(), v.c_str());
											if (bl)
											{
												if (EntityManager::GetInstance().Has<AnimationComponent>(entityID))
												{
													for (const auto& name : EntityManager::GetInstance().Get<AnimationComponent>(entityID).GetAnimationList())
													{
														if (ImGui::Selectable(name.c_str()))
															v = name;
													}
												}
												ImGui::EndCombo();
											}
											ImGui::Separator();
										}
										if (rmFlag)
										{
											it->second.animationStates.emplace(whoToRemove.second, it->second.animationStates.at(whoToRemove.first));
											it->second.animationStates.erase(whoToRemove.first);
											whoToRemove.first = "";
											whoToRemove.second = "";
											rmFlag = false;
										}
									}
								}
							}

							if (key == "RatScript")
							{
								RatScript* p_script = dynamic_cast<RatScript*>(val);
								auto it = p_script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader(key.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										// Misc script data
										ImGui::Text("Rat Settings");
										int input = static_cast<int>(it->second.mainCatID);
										ImGui::Text("Main Cat ID: "); ImGui::SameLine(); ImGui::InputInt("##targetCat", &input);
										it->second.mainCatID = static_cast<EntityID>(input);
										ImGui::Text("Health: "); ImGui::SameLine(); ImGui::InputInt("##rathealth", &it->second.health);
										ImGui::Text("Movement Speed: "); ImGui::SameLine(); ImGui::InputFloat("##ratmovespeed", &it->second.movementSpeed);

										ImGui::Text("Detection Radius: "); ImGui::SameLine(); ImGui::InputFloat("##ratdecrad", &it->second.detectionRadius);
										ImGui::Text("Attack Diameter: "); ImGui::SameLine(); ImGui::InputFloat("##ratattdia", &it->second.attackDiameter);
										ImGui::Text("Attack Duration: "); ImGui::SameLine(); ImGui::InputFloat("##ratattdur", &it->second.attackDuration);

										ImGui::Text("Touch Damage: "); ImGui::SameLine(); ImGui::InputInt("##rattdmg", &it->second.collisionDamage);
										ImGui::Text("Attack Damage: "); ImGui::SameLine(); ImGui::InputInt("##ratattdmg", &it->second.attackDamage);

										ImGui::Text("Attack Delay: "); ImGui::SameLine(); ImGui::InputFloat("##ratattdel", &it->second.attackDelay);


										// Animation state
										ImGui::Separator();
										int num{};
										ImGui::Text("Add Animation state"); ImGui::SameLine();
										bool worked{ false };
										if (ImGui::Button("+"))
										{
											std::string str = "NewState";
											while (!worked)
											{
												if (it->second.animationStates.count(str))
												{
													str += 1;
												}
												else
												{
													it->second.animationStates.emplace(str, "");
													worked = true;
												}
											}
										}
										static std::pair<std::string, std::string> whoToRemove;
										static bool rmFlag{ false };
										for (auto& [k, v] : it->second.animationStates)
										{
											ImGui::Text("State: "); ImGui::SameLine();
											std::string curr = (whoToRemove.first == k ? whoToRemove.first : k);
											bool changed = ImGui::InputText(std::string("##" + k + std::to_string(++num)).c_str(), &curr);
											if (!changed)
											{
												if (whoToRemove.first == k)
												{
													if (!ImGui::IsItemActivated())
													{
														rmFlag = true;
													}
												}
											}
											else
											{
												if (k != curr)
												{
													whoToRemove.first = k;
													whoToRemove.second = curr;
													rmFlag = false;
												}

											}


											ImGui::Text("Animation: "); ImGui::SameLine();
											bool bl = ImGui::BeginCombo(std::string("##Animation" + k + std::to_string(num)).c_str(), v.c_str());
											if (bl)
											{
												if (EntityManager::GetInstance().Has<AnimationComponent>(entityID))
												{
													for (const auto& name : EntityManager::GetInstance().Get<AnimationComponent>(entityID).GetAnimationList())
													{
														if (ImGui::Selectable(name.c_str()))
															v = name;
													}
												}
												ImGui::EndCombo();
											}
											ImGui::Separator();
										}
										if (rmFlag)
										{
											it->second.animationStates.emplace(whoToRemove.second, it->second.animationStates.at(whoToRemove.first));
											it->second.animationStates.erase(whoToRemove.first);
											whoToRemove.first = "";
											whoToRemove.second = "";
											rmFlag = false;
										}
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
						if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<TextComponent>())
								&& !EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
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
							if (ImGui::Selectable("Add Animation"))
							{
								if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
									EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<AnimationComponent>() });
								else
									AddErrorLog("ALREADY HAS ANIMATION");
							}
							if (ImGui::Selectable("Add Audio"))
							{
								if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<AudioComponent>()))
									EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<AudioComponent>() });
								else
									AddErrorLog("ALREADY HAS AUDIO");
							}
						}
						if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Graphics::Renderer>())) 
						{
							if (ImGui::Selectable("Add Text"))
							{
								if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<TextComponent>()))
									EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<TextComponent>() });
								else
									AddErrorLog("ALREADY HAS TEXT");
							}
						}

						ImGui::EndPopup();
					}

				}
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}

	void Editor::ShowResourceWindow(bool* p_active)
	{
		if (IsEditorActive())
		//testing for drag and drop
		if (!ImGui::Begin("Assets Browser", p_active)) // draw resource list
		{
			ImGui::End(); //imgui close
		}
		else
		{
			static int draggedItemIndex = -1;
			static bool isDragging = false;
			static std::string iconDragged{};
			static int rmbIndex = -1;
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
				ImGuiStyle& style = ImGui::GetStyle();
				float pos = ImGui::CalcTextSize("Refresh").x + style.FramePadding.x * 2.0f;
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - pos);
				// Refresh button
				if (ImGui::Button("Refresh"))
				{
					GetFileNamesInParentPath(m_parentPath, m_files);
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
								//if (ResourceManager::GetInstance().Textures.find(r_filepath.string()) != ResourceManager::GetInstance().Textures.end())
								//{
								//	ResourceManager::GetInstance().Textures[r_filepath.string()]->CreateTexture(r_filepath.string());
								//}
								ResourceManager::GetInstance().GetTexture(r_filepath.string());
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
						else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg")
							icon = "../Assets/Icons/Audio_Icon.png";
						else if (extension == ".ttf")
							icon = "../Assets/Icons/Font_Icon.png";
						else if (extension == ".json")
							icon = "../Assets/Icons/Prefabs_Icon.png";
						else if (extension == ".png")
							icon = "../Assets/Icons/Texture_Icon.png";
						else
							icon = "../Assets/Icons/Other_Icon.png";

						ImGui::Image((void*)(intptr_t)ResourceManager::GetInstance().GetIcon(icon)->GetTextureID(), ImVec2(50, 50), { 0,1 }, { 1,0 });
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
								else if (iconDraggedExtension == ".mp3" || iconDraggedExtension == ".wav" ||
									iconDraggedExtension == ".ogg" || iconDraggedExtension == ".flac" ||
									iconDraggedExtension == ".aiff" || iconDraggedExtension == ".mod" ||
									iconDraggedExtension == ".s3m" || iconDraggedExtension == ".xm" ||
									iconDraggedExtension == ".midi" || iconDraggedExtension == ".mid")
								{
									iconDragged = "../Assets/Icons/Audio_Icon.png";
								}
								else if (iconDraggedExtension == ".ttf")
									iconDragged = "../Assets/Icons/Font_Icon.png";
								else if (iconDraggedExtension == ".json")
									iconDragged = "../Assets/Icons/Prefabs_Icon.png";
								else if (iconDraggedExtension == ".png")
									iconDragged = "../Assets/Icons/Texture_Icon.png";
								else
									iconDragged = "../Assets/Icons/Other_Icon.png";
							}
							else if (ImGui::IsMouseClicked(1))
							{
								ImGui::OpenPopup("AssetDeletePopup");
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

					if (ImGui::IsItemClicked(1))
					{
						rmbIndex = n;

						if (m_files[n].extension().string() == ".json")
						{
							ImGui::OpenPopup("EditPrefab");
						}
						else
						{
							ImGui::OpenPopup("AssetDeletePopup");
						}
					}
					if (n == rmbIndex && ImGui::BeginPopup("AssetDeletePopup"))
					{
						if (ImGui::Selectable("Delete Asset"))
						{
							try
							{
								std::filesystem::remove(m_files[n]);
								GetFileNamesInParentPath(m_parentPath, m_files);
							}
							catch (std::filesystem::filesystem_error& e)
							{
								engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
								engine_logger.SetTime();
								engine_logger.AddLog(false, e.what(), __FUNCTION__);
							}
						}
						ImGui::EndPopup();
					}
					if (n == rmbIndex && ImGui::BeginPopup("EditPrefab"))
					{
						if (ImGui::Selectable("Modify Prefab"))
						{
							
							engine_logger.AddLog(false, "Enterting PreFabEditorMode...", __FUNCTION__);
							prefabFP = (m_isPrefabMode)? prefabFP : m_files[n].string();

							if (!m_isPrefabMode)
							{
								m_isPrefabMode = true;
								engine_logger.AddLog(false, "Attempting to save all entities to file...", __FUNCTION__);
								// This will save all entities to a file
								for (const auto& id : SceneView<EntityDescriptor>())
								{
									if (!id) // skip editor camera
										continue;
									EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(id);
									
									desc.sceneID = id;
										
									if (desc.parent)
									{
										EntityManager::GetInstance().Get<EntityDescriptor>(desc.parent.value()).children.emplace(id);
									}
								}
								serializationManager.SaveAllEntitiesToFile("../Assets/Prefabs/savestate.json");
								engine_logger.AddLog(false, "Entities saved successfully to file.", __FUNCTION__);
								
							}
							else if(EntityManager::GetInstance().Has<EntityDescriptor>(1))
							{

								auto save = serializationManager.SerializeEntityPrefab(1);
								std::ofstream outFile(prefabFP);
								if (outFile)
								{
									outFile << save.dump(4);
									outFile.close();
								}
								prefabFP = m_files[n].string();
							}
							ClearObjectList();
							engine_logger.AddLog(false, "Entities Cleared.", __FUNCTION__);
							serializationManager.LoadFromFile(prefabFP);
						}
						if (ImGui::Selectable("Delete Asset"))
						{
							std::filesystem::remove(m_files[n]);
							GetFileNamesInParentPath(m_parentPath, m_files);
						}
						ImGui::EndPopup();
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
					ImGui::Image((void*)(intptr_t)ResourceManager::GetInstance().GetIcon(iconDragged)->GetTextureID(), ImVec2(34,34), { 0,1 }, { 1,0 });
					ImGui::End();
					
					// Check if the mouse button is released
					if (ImGui::IsMouseReleased(0))
					{
						if (m_entityToModify.second != -1)
						{
							// alters the texture assigned to renderer component in entity
							std::string const extension = m_files[draggedItemIndex].extension().string();
							if (extension == ".png")
							{
								if (m_entityToModify.first == "Renderer")
								{
									ResourceManager::GetInstance().LoadTextureFromFile(m_files[draggedItemIndex].string(), m_files[draggedItemIndex].string());
									EntityManager::GetInstance().Get<Graphics::Renderer>(m_entityToModify.second).SetTextureKey(m_files[draggedItemIndex].string());
									EntityManager::GetInstance().Get<Graphics::Renderer>(m_entityToModify.second).SetColor(1.f, 1.f, 1.f, 1.f);
								}
								else if (m_entityToModify.first == "GUIRenderer")
								{
									ResourceManager::GetInstance().LoadTextureFromFile(m_files[draggedItemIndex].string(), m_files[draggedItemIndex].string());
									EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_entityToModify.second).SetTextureKey(m_files[draggedItemIndex].string());
									EntityManager::GetInstance().Get<Graphics::GUIRenderer>(m_entityToModify.second).SetColor(1.f, 1.f, 1.f, 1.f);
								}
							}
							if (extension == ".wav")
							{
								std::string newAudioKey = ResourceManager::GetInstance().LoadDraggedAudio(m_files[draggedItemIndex].string());
								std::cout << "[ShowResourceWindow] Dragged audio file: " << m_files[draggedItemIndex].string() << std::endl;
								std::cout << "[ShowResourceWindow] New audio key: " << newAudioKey << std::endl;
								if (!newAudioKey.empty())
								{
									EntityManager::GetInstance().Get<AudioComponent>(m_entityToModify.second).SetAudioKey(newAudioKey);
									std::cout << "currentSoundID updated to: " << EntityManager::GetInstance().Get<AudioComponent>(m_entityToModify.second).GetAudioKey() << std::endl;
								}
							}
							else
							{
								AudioComponent::ShowErrorMessage("Error: Invalid file type. Expected '.wav', but got '" + extension + "'.", "File Type Error");
							}
						}

						if (m_mouseInScene || m_mouseInObjectWindow)
						{
							if (m_files[draggedItemIndex].extension() == ".json")
							{
								EntityID s_id = serializationManager.LoadFromFile(m_files[draggedItemIndex].string());
								m_undoStack.AddChange(new CreateObjectUndo(s_id));
								// change position of loaded prefab based on mouse cursor here
							}
						}

						isDragging = false;
						draggedItemIndex = -1;
					}
				}
				m_entityToModify = std::make_pair<std::string>("", - 1);
			}
			

			ImGui::End(); //imgui close
		}


	}

	void Editor::ShowPerformanceWindow(bool* p_active)
	{
		//if (IsEditorActive())
		if (!ImGui::Begin("Performance Window", p_active, ImGuiWindowFlags_AlwaysAutoResize)) // draw resource list
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

	void Editor::ShowAnimationWindow(bool* p_active)
	{
		if (IsEditorActive())
		if (!ImGui::Begin("Animation Editor", p_active, ImGuiWindowFlags_AlwaysAutoResize)) // draw resource list
		{
			ImGui::End(); //imgui close
		}
		else
		{
			ImGui::SeparatorText("Rendering settings");
			std::map<const char*, EntityID> entities;
			std::vector<const char*> entityList;
			std::vector<const char*> animationList;

			static std::optional<EntityID> currentEntityID;
			static std::string currentAnimationID;

			int entityIndex{};
			int animationIndex{};

			// animation data
			static float previewCurrentFrameTime;
			static unsigned previewCurrentFrameIndex;
			std::shared_ptr<Graphics::Texture> texture;
			std::shared_ptr<Animation> currentAnimation;
			int frameCount{};
			int totalSprites{};
			int frameRate{};
			float animationDuration{};

			// frame data
			float frameTime{};
			ImVec2 minUV;
			ImVec2 maxUV;
			int framesHeld{};

			static bool playAnimation{ false };


			// get all the entities with animation component
			for (EntityID id : SceneView<AnimationComponent>())
			{
				entities[EntityManager::GetInstance().Get<EntityDescriptor>(id).name.c_str()] = id;
				entityList.push_back(EntityManager::GetInstance().Get<EntityDescriptor>(id).name.c_str());
			}

			std::vector<std::filesystem::path> animationFilePaths;
			std::vector<std::string> loadedAnimationKeys;
			// get all animations from resource manager
			for (std::map<std::string, std::shared_ptr<Animation>>::iterator it = ResourceManager::GetInstance().Animations.begin(); it != ResourceManager::GetInstance().Animations.end(); ++it)
			{
				animationFilePaths.emplace_back(it->first);
			}

			for(auto const& r_path : animationFilePaths)
			{
				loadedAnimationKeys.push_back(r_path.stem().string());
			}

			for (auto const& r_str : loadedAnimationKeys)
			{
				animationList.push_back(r_str.c_str());
			}

			// get index of current animation for combo box
			for (auto const& r_path : animationFilePaths)
			{
				if (r_path.string() == currentAnimationID)
					break;
				++animationIndex;
			}

			// get index of current entity for combo box
			for (std::string str : entityList)
			{
				if (currentEntityID.has_value())
				{
					if (str == EntityManager::GetInstance().Get<EntityDescriptor>(currentEntityID.value()).name)
					break;
				}
				++entityIndex;
			}

			ImGui::SetNextItemWidth(300.f);
			if (ImGui::Combo("Entity", &entityIndex, entityList.data(), static_cast<int>(entityList.size())))
			{
				currentEntityID = entities[entityList[entityIndex]];
			}

			ImGui::SetNextItemWidth(300.f);
			if (ImGui::Combo("Animation", &animationIndex, animationList.data(), static_cast<int>(animationList.size())))
			{
				currentAnimationID = animationFilePaths[animationIndex].string();
			}

			if (ImGui::Button("Create Animation"))
			{
				// Get the file path using the file explorer
				std::string filePath = serializationManager.OpenFileExplorerRequestPath();

				// Check if filePath is not empty
				if (!filePath.empty())
				{
					// format filepath to be relative to the Assets folder
					std::replace(filePath.begin(), filePath.end(), '\\', '/');
					filePath = ".." + filePath.substr(filePath.find("/Assets/"), filePath.find(".") - filePath.find("/Assets/")) + "_Anim.json";

					// Create a new animation
					currentAnimationID = AnimationManager::CreateAnimation(filePath);

					// Serialize the current animation data to JSON
					nlohmann::json serializedAnimation = ResourceManager::GetInstance().GetAnimation(currentAnimationID)->ToJson();

					serializationManager.SaveAnimationToFile(filePath, serializedAnimation);
					std::cout << "Animation created successfully at " << filePath << std::endl;
				}
				else
				{
					std::cerr << "No file path was selected for saving." << std::endl;
				}
			}

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::SeparatorText("Sprite Sheet");

			// if there's animation to preview
			if (currentAnimationID != "")
			{
				currentAnimation = ResourceManager::GetInstance().GetAnimation(currentAnimationID);

				// get max frames
				frameCount = currentAnimation->GetFrameCount() ? currentAnimation->GetFrameCount() - 1 : 0;
				frameTime = currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_duration;
				frameRate = currentAnimation->GetFrameRate();
				animationDuration = currentAnimation->GetAnimationDuration();

				// if animation has spritesheet, get the texture
				if (currentAnimation->GetSpriteSheetKey() != "")
				{
					texture = ResourceManager::GetInstance().GetTexture(currentAnimation->GetSpriteSheetKey());

					minUV = { currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_minUV.x,
							  currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_maxUV.y };
					maxUV = { currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_maxUV.x,
							  currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_minUV.y };
				}
			}
			else
			{
				// reset preview animation
				previewCurrentFrameTime = 0.f;
				previewCurrentFrameIndex = 0;
			}

			// spritesheet selection for animation
			// Vector of filepaths that have already been loaded - used to refer to later when needing to change the object's texture
			std::vector<std::filesystem::path> filepaths;
			int textureIndex{ 0 };
			for (auto it = ResourceManager::GetInstance().Textures.begin(); it != ResourceManager::GetInstance().Textures.end(); ++it)
			{
				filepaths.emplace_back(it->first);
			}

			// Vector of the names of textures that have already been loaded
			std::vector<std::string> loadedTextureKeys;
			std::vector<const char*> textureList;

			// get the keys of textures already loaded by the resource manager
			for (auto const& r_filepath : filepaths)
			{
				loadedTextureKeys.push_back(r_filepath.stem().string());
			}

			for (auto const& r_filepath : filepaths)
			{
				if (currentAnimation)
					if (r_filepath.string() == currentAnimation->GetSpriteSheetKey())
						break;
				++textureIndex;
			}

			for (auto const& r_str : loadedTextureKeys)
			{
				textureList.push_back(r_str.c_str());
			}

			// Displays the current spritesheet set on the animation
			if (ImGui::BeginChild("currentTexture", ImVec2{ 116,116 }, true))
			{
				// if theres a spritesheet texture
				if (texture)
					ImGui::Image((void*)(intptr_t)texture->GetTextureID(), ImVec2{ 100, 100 }, minUV, maxUV);
				ImGui::EndChild();
			}

			ImGui::SetNextItemWidth(300.f);
			if (ImGui::Combo("Spritesheet", &textureIndex, textureList.data(), static_cast<int>(textureList.size())))
			{
				if(currentAnimation)
					currentAnimation->SetSpriteSheetKey(filepaths[textureIndex].string());
			}

			if (ImGui::Button("Play"))
			{
				playAnimation = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				playAnimation = false;
			}

			// slider
			ImGui::SameLine();
			int frame{ static_cast<int>(previewCurrentFrameIndex)};
			ImGui::SetNextItemWidth(300);

			if (ImGui::SliderInt("##FrameSlider", &frame, 0, frameCount))
			{
				previewCurrentFrameIndex = static_cast<unsigned>(frame);
			}

			ImGui::SameLine();
			ImGui::Text("%d:%.f", static_cast<int>(animationDuration), (animationDuration - static_cast<int>(animationDuration)) * 100.f);

			if (currentAnimation)
			{
				// update preview animation
				if (playAnimation)
					currentAnimation->UpdateAnimationFrame(TimeManager::GetInstance().GetDeltaTime(), previewCurrentFrameTime, previewCurrentFrameIndex);

				// update frame display parameters
				frameTime = currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_duration;
				minUV = { currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_minUV.x,
						  currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_maxUV.y };
				maxUV = { currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_maxUV.x,
						  currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_minUV.y };
			}

			if (ImGui::Button("Add Animation"))
			{
				if (currentEntityID.has_value())
				{
					EntityManager::GetInstance().Get<AnimationComponent>(currentEntityID.value()).AddAnimationToComponent(currentAnimationID);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Remove Animation"))
			{
				if (currentEntityID.has_value())
				{
					EntityManager::GetInstance().Get<AnimationComponent>(currentEntityID.value()).RemoveAnimationFromComponent(currentAnimationID);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Save"))
			{
				if (currentAnimation)
				{
					std::string filePath = currentAnimation->GetAnimationID();

					// Serialize the current animation data to JSON
					nlohmann::json serializedAnimation = currentAnimation->ToJson();

					// Check if filePath is not empty
					if (!filePath.empty())
					{
						serializationManager.SaveAnimationToFile(filePath, serializedAnimation);
						std::cout << "Animation saved successfully to " << filePath << std::endl;
					}
					else
					{
						std::cerr << "No file path was selected for saving." << std::endl;
					}
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Load"))
			{
				std::string filePath = serializationManager.OpenFileExplorerRequestPath();

				// Check if filePath is not empty
				if (!filePath.empty())
				{
					nlohmann::json loadedAnimationData = serializationManager.LoadAnimationFromFile(filePath);

					if (!loadedAnimationData.is_null())
					{
						// create animation to load into
						currentAnimationID = AnimationManager::CreateAnimation(loadedAnimationData["animationID"].get<std::string>());

						currentAnimation = ResourceManager::GetInstance().GetAnimation(currentAnimationID);
						currentAnimation->Deserialize(loadedAnimationData);

						// Update texture for preview
						if (currentAnimation->GetSpriteSheetKey() != "")
						{
							texture = ResourceManager::GetInstance().GetTexture(currentAnimation->GetSpriteSheetKey());
						}

						// Set preview frame data to first frame of the loaded animation
						previewCurrentFrameIndex = 0; // Reset to first frame
						frameCount = currentAnimation->GetFrameCount();
						frameRate = currentAnimation->GetFrameRate();
						animationDuration = currentAnimation->GetAnimationDuration();
						frameTime = currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_duration;
						minUV = { currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_minUV.x,
								  currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_minUV.y };
						maxUV = { currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_maxUV.x,
								  currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_maxUV.y };

						//// debugging code- hans
						//std::cout << "Loaded Animation Data:\n" << loadedAnimationData.dump(4) << std::endl;
					}
					else
					{
						std::cerr << "Failed to load animation data from file." << std::endl;
					}
				}
				else
				{
					std::cerr << "No file path was selected for loading." << std::endl;
				}
			}

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::SeparatorText("Animation Properties");
			ImGui::Columns(2, "TwoSections", true);
			//ImGui::Text("Name");
			//ImGui::Dummy(ImVec2(0, 5));
			//ImGui::Text("looped");
			//ImGui::Dummy(ImVec2(0, 5));
			ImGui::Text("Total Sprites");
			ImGui::Dummy(ImVec2(0, 5));
			ImGui::Text("Frame Rate");
			ImGui::Dummy(ImVec2(0, 5));
			ImGui::Text("Frames Held");
			ImGui::Dummy(ImVec2(0, 5));

			ImGui::NextColumn();/*
			static std::string text{};
			ImGui::InputText("##name", &text);
			static bool looped{};
			ImGui::Checkbox("##looped", &looped);*/



			if (currentAnimation)
			{
				totalSprites = currentAnimation->GetFrameCount();
				animationDuration = currentAnimation->GetAnimationDuration();
				frameRate = currentAnimation->GetFrameRate();
			}

			// edit total sprites in an animation
			if (ImGui::InputInt("##totalSprites", &totalSprites))
			{
				totalSprites = totalSprites < 0 ? 0 : totalSprites;

				playAnimation = false;
				if(currentAnimation)
					currentAnimation->CreateAnimationFrames(totalSprites);

				// update frame duration
				frameTime = currentAnimation->GetCurrentAnimationFrame(previewCurrentFrameIndex).m_duration;

				// check if index is same size as total sprites
				if (previewCurrentFrameIndex == static_cast<unsigned>(totalSprites))
				{
					// set index to last sprite if not zero
					previewCurrentFrameIndex = totalSprites ? totalSprites - 1 : 0;
				}
			}

			if (ImGui::InputInt("##frameRate", &frameRate))
			{
				currentAnimation->SetCurrentAnimationFrameRate(frameRate);
			}

			// get frames held
			framesHeld = static_cast<int>(static_cast<float>(frameRate) * frameTime);

			ImGui::InputInt("##framesHeld", &framesHeld);

			// update animation frame data
			if (currentAnimation)
			{
				currentAnimation->SetCurrentAnimationFrameData(previewCurrentFrameIndex, static_cast<unsigned>(framesHeld));
			}

			// save animation here

			ImGui::End(); //imgui close
		}
	}

	void Editor::SetDockingPort(bool* p_active)
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
			if (!ImGui::Begin("DockSpace", p_active, window_flags))
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
						ImGui::DockBuilderDockWindow("Scene View", dockspace_id);

						//set the other sides
						ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.3f, nullptr, &dockspace_id);
						ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, &dockspace_id, &dockspace_id);
						ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, &dockspace_id, &dockspace_id);

						//setting the other dock locations
						ImGui::DockBuilderDockWindow("Object Hierarchy Window", dock_id_right);

						//set on the save location to dock ontop of eachother
						ImGui::DockBuilderDockWindow("Assets Browser", dock_id_down);
						ImGui::DockBuilderDockWindow("Console Window", dock_id_down);


						//set on the save location to dock ontop of eachother
						ImGui::DockBuilderDockWindow("Property Editor Window", dock_id_left);

						//split the bottom into 2
						ImGuiID dock_id_down2 = ImGui::DockBuilderSplitNode(dock_id_down, ImGuiDir_Right, 0.5f, nullptr, &dock_id_down);

						ImGui::DockBuilderDockWindow("Debug Log Window", dock_id_down2);

						//end dock
						ImGui::DockBuilderFinish(dockspace_id);

					}
				}




				//docking port menu bar
				if (IsEditorActive()) {
					if (ImGui::BeginMainMenuBar())
					{
						//menu 1
						if (ImGui::BeginMenu("File"))
						{
							if (m_isPrefabMode)
							{
								if (ImGui::MenuItem("Save", "CTRL+S")) // the ctrl s is not programmed yet, need add to the key press event
								{
									engine_logger.AddLog(false, "Attempting to save prefab entities to file...", __FUNCTION__);
									
									if (EntityManager::GetInstance().Has<EntityDescriptor>(1))
									{
										auto save = serializationManager.SerializeEntityPrefab(1);

										std::ofstream outFile(prefabFP);
										if (outFile)
										{
											outFile << save.dump(4);
											outFile.close();
										}

										engine_logger.AddLog(false, "Prefab saved successfully to file.", __FUNCTION__);
									}
								}
							}
							else
							{
								if (ImGui::MenuItem("Save", "CTRL+S")) // the ctrl s is not programmed yet, need add to the key press event
								{
									engine_logger.AddLog(false, "Attempting to save all entities to file...", __FUNCTION__);
									// This will save all entities to a file
									for (const auto& id : SceneView<EntityDescriptor>())
									{
										if (!id) // skip editor camera
											continue;
										EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(id);
										desc.sceneID = id;
										if (desc.parent)
										{
											EntityManager::GetInstance().Get<EntityDescriptor>(desc.parent.value()).children.emplace(id);
										}
									}
									serializationManager.SaveAllEntitiesToFile(serializationManager.OpenFileExplorerRequestPath());
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
                                        m_undoStack.ClearStack();
										ClearObjectList();
										serializationManager.LoadAllEntitiesFromFile(filePath);
										engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
									}
									else
									{
										engine_logger.AddLog(false, "File path is empty. Aborted loading entities.", __FUNCTION__);
									}
								}
							}
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
							if (ImGui::MenuItem("AnimationEditor", "", m_showAnimationWindow, !m_showAnimationWindow))
							{
								m_showAnimationWindow = !m_showAnimationWindow;
							}
							if (ImGui::MenuItem("PhysicsWindow", "", m_showPhysicsWindow, !m_showPhysicsWindow))
							{
								m_showPhysicsWindow = !m_showPhysicsWindow;
							}
							ImGui::Separator();
							if (ImGui::MenuItem("Close Editor (Game Wont Start Either)", "", m_showEditor, true))
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
								m_showAnimationWindow = false;
							}
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu("Theme"))
						{
							if (ImGui::MenuItem("Kurumi", ""))
							{
								m_currentStyle = GuiStyle::DARK;
								SetImGUIStyle_Dark();
							}
							if (ImGui::MenuItem("My Melody [WIP]", ""))
							{
								m_currentStyle = GuiStyle::PINK;
								SetImGUIStyle_Pink();
							}
							if (ImGui::MenuItem("Cinnamoroll", ""))
							{
								m_currentStyle = GuiStyle::BLUE;
								SetImGUIStyle_Blue();
							}
							ImGui::EndMenu();
						}
					}
					ImGui::EndMainMenuBar(); // closing of menu begin function
				}
				ImGui::End(); //finish drawing
			}
		}
	}

	void Editor::ShowPhysicsWindow(bool* p_active)
	{
		if (IsEditorActive())
		if (!ImGui::Begin("Physics Config Window", p_active, ImGuiWindowFlags_AlwaysAutoResize)) // draw resource list
		{
			ImGui::End(); //imgui close
		}
		else
		{
			ImGui::Text("Broad Phase Grid Size:");
			int grid[2] = { static_cast<int>(CollisionManager::gridSize.x),static_cast<int>(CollisionManager::gridSize.y) };
			ImGui::InputInt2("##grid",grid);
			CollisionManager::gridSize.x = static_cast<float>(abs( grid[0]));
			CollisionManager::gridSize.y = static_cast<float>(abs(grid[1]));
			ImGui::Dummy(ImVec2(0, 0.2f));

			ImGui::Text("Grid Active: "); ImGui::SameLine(); ImGui::Checkbox("##Checkers", &CollisionManager::gridActive);
			ImGui::Dummy(ImVec2(0, 0.2f));
			ImGui::Separator();


			if (ImGui::Button("Toggle Debug Lines"))
			{
				ToggleDebugRender();
			}

			ImGui::SeparatorText("Collision Layer Matrix");

			auto& collisionLayers { CollisionLayerManager::GetInstance().GetCollisionLayers() };
			
			std::vector<const char*> column_names{ "" };
			std::vector<const char*> row_names;

			for(auto& collisionLayer : collisionLayers)
			{
				column_names.push_back(collisionLayer->GetCollisionLayerName().c_str());
				row_names.push_back(collisionLayer->GetCollisionLayerName().c_str());
			}
		
			unsigned counter{};
			const int columns_count = static_cast<int>(column_names.size());
			static ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit| ImGuiTableFlags_Hideable;

				if (ImGui::BeginTable("collisionlayer", columns_count, table_flags))
				{
					ImGui::TableSetupColumn(column_names[0], ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoReorder);
					for (int n = columns_count - 1; n > 0 ; --n)
						ImGui::TableSetupColumn(column_names[n], ImGuiTableColumnFlags_WidthFixed);

					ImGui::TableHeadersRow();       // Draw remaining headers and allow access to context-menu and other functions.
					for (auto const& p_collisionLayer : collisionLayers)
					{
						ImGui::PushID(p_collisionLayer->GetCollisionLayerIndex());
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();
						ImGui::Text(row_names[p_collisionLayer->GetCollisionLayerIndex()], p_collisionLayer->GetCollisionLayerIndex());
						for (unsigned i{ static_cast<unsigned>(p_collisionLayer->GetCollisionLayerSignature().size()) }, column{ 1 }; i != 0; ++column)
							if (ImGui::TableSetColumnIndex(column))
							{
								bool bit = p_collisionLayer->IsCollidingWith(--i);
								ImGui::PushID(column);
								if(column < static_cast<unsigned>(columns_count) - counter)
									if (ImGui::Checkbox("", &bit))
									{
										// flip the bit
										p_collisionLayer->FlipCollisionLayerBit(i);

										// check if its not flipping its own bit
										if (p_collisionLayer->GetCollisionLayerIndex() != i)
										{
											// flip the bit of the other collision layer
											collisionLayers[i]->FlipCollisionLayerBit(p_collisionLayer->GetCollisionLayerIndex());
										}
									}
								ImGui::PopID();
							}
						ImGui::PopID();
						++counter;
					}
					ImGui::EndTable();
				}

				// print debug layer signature
				//for (auto const& p_collisionLayer : collisionLayers)
				//{
				//	ImGui::Text((p_collisionLayer->GetCollisionLayerName() + ": ").c_str());
				//	for (std::size_t i{ p_collisionLayer->GetCollisionLayerSignature().size()}; i != 0 ;)
				//	{
				//		ImGui::SameLine();
				//		ImGui::Text(" %d ", p_collisionLayer->IsCollidingWith(--i));
				//	}
				//}

			ImGui::End(); //imgui close
		}
	}

	void Editor::ShowSceneView(Graphics::FrameBuffer& r_frameBuffer, bool* p_active)
	{
		if (IsEditorActive()) {
			ImGui::Begin("Scene View", p_active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);


			m_renderWindowWidth = ImGui::GetContentRegionAvail().x;
			m_renderWindowHeight = ImGui::GetContentRegionAvail().y;

			ImGuiStyle& style = ImGui::GetStyle();
			if (!m_isPrefabMode)
			{
				float size = ImGui::CalcTextSize("Play").x + style.FramePadding.x * 2.0f;
				float avail = ImGui::GetContentRegionAvail().x;

				float off = (float)((avail - size) * 0.5);
				if (off > 0.0f)
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off - (ImGui::CalcTextSize("Play").x + style.FramePadding.x) / 2);

				if (ImGui::Button("Play"))
				{
					m_isRunTime = true;
					m_showEditor = false;
					m_showGameView = true;
					engine_logger.AddLog(false, "Attempting to save all entities to file...", __FUNCTION__);
					// This will save all entities to a file
					for (const auto& id : SceneView<EntityDescriptor>())
					{
						if (!id) // skip editor camera
							continue;
						EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(id);
						desc.sceneID = id;
						if (desc.parent)
						{
							EntityManager::GetInstance().Get<EntityDescriptor>(desc.parent.value()).children.emplace(id);
						}
					}
					serializationManager.SaveAllEntitiesToFile("../Assets/Prefabs/savestate.json");
                    m_undoStack.ClearStack();
					engine_logger.AddLog(false, "Entities saved successfully to file.", __FUNCTION__);
				}
				ImGui::SameLine();
				ImGui::BeginDisabled();
				if (ImGui::Button("Stop")) {
					m_showEditor = true;

					if (m_isRunTime)
					{
						ClearObjectList();
						serializationManager.LoadAllEntitiesFromFile("../Assets/Prefabs/savestate.json");
						engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
					}

					if (m_showEditor)
						m_isRunTime = false;

					m_showGameView = false;
				}
				ImGui::EndDisabled();
			}
			else // is prefab mode
			{
				float size = ImGui::CalcTextSize("Return").x + style.FramePadding.x * 2.0f;
				float avail = ImGui::GetContentRegionAvail().x;

				float off = (float)((avail - size) * 0.5);
				if (off > 0.0f)
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off - (ImGui::CalcTextSize("Return").x + style.FramePadding.x) / 2);

				if (ImGui::Button("Return"))
				{

					//auto save = serializationManager.SerializeEntityPrefab(1);

					//std::ofstream outFile(prefabFP);
					//if (outFile)
					//{
					//	outFile << save.dump(4);
					//	outFile.close();
					//}
					//m_isPrefabMode = false;
					//ClearObjectList();
					//serializationManager.LoadAllEntitiesFromFile("../Assets/Prefabs/savestate.json");
					//engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
					ImGui::OpenPopup("ReqSave?");
					m_applyPrefab = false;
				}

				ImGui::SameLine();
				if (ImGui::Button(" Save "))
				{
					if (EntityManager::GetInstance().Has<EntityDescriptor>(1))
					{
						auto save = serializationManager.SerializeEntityPrefab(1);

						std::ofstream outFile(prefabFP);
						if (outFile)
						{
							outFile << save.dump(4);
							outFile.close();
						}
					}
				}
				if (ImGui::BeginPopup/*Modal*/("ReqSave?"))
				{
					ImGui::Text("Do you want to save your changes?");
					ImGui::Separator();

					ImGui::Separator();

					if (ImGui::Selectable("Yes"))
					{
						if (EntityManager::GetInstance().Has<EntityDescriptor>(1))
						{
							auto save = serializationManager.SerializeEntityPrefab(1);
							prefabTP = EntityManager::GetInstance().Get<EntityDescriptor>(1).prefabType;
							prefabCID = EntityManager::GetInstance().GetComponentIDs(1);
							std::ofstream outFile(prefabFP);
							if (outFile)
							{
								outFile << save.dump(4);
								outFile.close();
							}
						}
						m_isPrefabMode = false;
						ClearObjectList();
						serializationManager.LoadAllEntitiesFromFile("../Assets/Prefabs/savestate.json");
						engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
						m_applyPrefab = true;
					}
					//ImGui::SameLine();
					ImGui::Separator();
					if (ImGui::Selectable("No"))
					{
						m_isPrefabMode = false;
						ClearObjectList();
						serializationManager.LoadAllEntitiesFromFile("../Assets/Prefabs/savestate.json");
						engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
					}
					ImGui::EndPopup();
					m_undoStack.ClearStack();
				}
			}
			//ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2.f, ImGui::GetCursorPosY()));
			if (ImGui::BeginChild("SceneViewChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar)) {
				if (r_frameBuffer.GetTextureId())
				{
					//the graphics rendered onto an image on the imgui window
					ImGui::Image(
						reinterpret_cast<void*>(
							static_cast<intptr_t>(r_frameBuffer.GetTextureId())),
						ImVec2(m_renderWindowWidth, m_renderWindowHeight),
						ImVec2(0, 1),
						ImVec2(1, 0)
					);
				}
				m_mouseInScene = ImGui::IsWindowHovered();
			}
			static ImVec2 clickedPosition;
			static ImVec2 screenPosition; // coordinates from the top left corner
			static ImVec2 currentPosition;
			static vec2 startPosition;
			if (m_mouseInScene)
			if (ImGui::IsMouseClicked(0))
			{
				clickedPosition.x = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x; // from bottom left corner
				clickedPosition.y = ImGui::GetCursorScreenPos().y - ImGui::GetMousePos().y; // from bottom left corner

				screenPosition.x = clickedPosition.x - m_renderWindowWidth * 0.5f; // in viewport space, origin in the center
				screenPosition.y = clickedPosition.y - m_renderWindowHeight * 0.5f; // in viewport space, origin in the center

				// Check that position is within the viewport
				if (clickedPosition.x < 0 || clickedPosition.x >= m_renderWindowWidth
					|| clickedPosition.y < 0 || clickedPosition.y >= m_renderWindowHeight)
				{
					
				}
				else
				{
					m_currentSelectedObject = -1;
					// Loop through all objects
					for (long i{ static_cast<long>(Graphics::RendererManager::renderedEntities.size() - 1) }; i >= 0; --i)
					{
						EntityID id{ Graphics::RendererManager::renderedEntities[i] };

						// Get the transform component of the entity
						Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };

						glm::vec4 transformedCursor{ screenPosition.x, screenPosition.y, 0.f, 1.f };

						// Transform the position of the mouse cursor from screen space to model space
						glm::vec4 worldSpacePosition{
								Graphics::CameraManager::GetEditorCamera().GetViewToWorldMatrix() // screen to world position
								* transformedCursor // screen position
						};

						// If trying to pick text or UI, don't transform to world space
						if (!EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<TextComponent>())
							&& !EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
						{
							transformedCursor = worldSpacePosition;
						}
						else
						{
							transformedCursor /= Graphics::CameraManager::GetEditorCamera().GetMagnification();
						}

						transformedCursor = Graphics::RendererManager::GenerateInverseTransformMatrix(r_transform.width, r_transform.height, r_transform.orientation, r_transform.position.x, r_transform.position.y) // world to model
							* transformedCursor;

						glm::vec4 backToWorldSpacePosition{
								Graphics::RendererManager::GenerateTransformMatrix(r_transform.width, r_transform.height, r_transform.orientation, r_transform.position.x, r_transform.position.y) // world to model
								* transformedCursor
						};


						// Check if the cursor position is within the bounds of the object
						if (!(transformedCursor.x < -0.5f || transformedCursor.x > 0.5f
							|| transformedCursor.y < -0.5f || transformedCursor.y > 0.5f))
						{
							// It is within the bounds, break out of the loop
							m_currentSelectedObject = static_cast<int>(id);
							break;
						}
					}

					if (m_currentSelectedObject != -1)
					{
						if (EntityManager::GetInstance().Has<Transform>(m_currentSelectedObject))
						startPosition = EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).position;
					}
				}

				std::cout << "\n";
			}

			static float rotation;
			static bool rotating = false;
			static bool scaling = false;
			static bool moved = false;
			static float height;
			static float width;
			if (m_currentSelectedObject != -1)
			{
				if (InputSystem::IsKeyTriggered(GLFW_KEY_R) && rotating == false)
				{
					if (EntityManager::GetInstance().Has<Transform>(m_currentSelectedObject))
					{
						rotation = EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).orientation;
						clickedPosition.y = ImGui::GetCursorScreenPos().y - ImGui::GetMousePos().y;
						rotating = true;
					}
				}

				if (InputSystem::IsKeyTriggered(GLFW_KEY_S) && scaling == false)
				{
					if (EntityManager::GetInstance().Has<Transform>(m_currentSelectedObject))
					{
						height = EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height;
						width = EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width;
						clickedPosition.y = ImGui::GetCursorScreenPos().y - ImGui::GetMousePos().y;
						clickedPosition.x = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x;

						scaling = true;
					}
				}
			}
			float currentRotation;
			float currentHeight;
			float currentWidth;
			if (ImGui::IsMouseDragging(0) && (m_currentSelectedObject >= 0) && m_currentSelectedObject != -1)
			{
				currentPosition.x = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x;
				currentPosition.y = ImGui::GetCursorScreenPos().y - ImGui::GetMousePos().y;
				// Check that position is within the viewport
				if (!(clickedPosition.x < 0 || clickedPosition.x >= m_renderWindowWidth
					|| clickedPosition.y < 0 || clickedPosition.y >= m_renderWindowHeight))
				{
					ImVec2 offset;
					offset.x = currentPosition.x - clickedPosition.x;
					offset.y = currentPosition.y - clickedPosition.y;

					float magnification = Graphics::CameraManager::GetEditorCamera().GetMagnification();

					if (!EntityManager::GetInstance().Has(m_currentSelectedObject, EntityManager::GetInstance().GetComponentID<TextComponent>())
						&& !EntityManager::GetInstance().Has(m_currentSelectedObject, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
					{
						offset.x *= magnification;
						offset.y *= magnification;
					}


					if (InputSystem::IsKeyHeld(GLFW_KEY_R) && scaling == false)
					{
						currentRotation = rotation - offset.y;
						if (EntityManager::GetInstance().Has<Transform>(m_currentSelectedObject))
						{
							EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).orientation = currentRotation;
							rotating = true;
						}
					}

					if (InputSystem::IsKeyHeld(GLFW_KEY_S) && rotating == false)
					{
						if (InputSystem::IsKeyHeld(GLFW_KEY_X))
						{
							currentWidth = width + offset.x;
								EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width = currentWidth;
								EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height = height;
						}
						else if (InputSystem::IsKeyHeld(GLFW_KEY_Y))
						{
							currentHeight = height + offset.y;
								EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height = currentHeight;
								EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width = width;
						}
						else
						{
							currentHeight = height + offset.y;
							currentWidth = width + offset.x;
								EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height = currentHeight;
								EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width = currentWidth;
						}
						scaling = true;
					}

					if (m_mouseInScene)
						if (rotating != true && scaling != true)
						{ 
							moved = true;
							if (InputSystem::IsKeyHeld(GLFW_KEY_X))
							{
								EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).position = vec2(startPosition.x + offset.x, startPosition.y);
							}
							else if (InputSystem::IsKeyHeld(GLFW_KEY_Y))
							{
								EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).position = vec2(startPosition.x , startPosition.y + offset.y);
							}
							else
							{
									EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).position = vec2(startPosition.x + offset.x, startPosition.y + offset.y);
							}
						}


				}

			}
			else
			{
				clickedPosition.y = ImGui::GetCursorScreenPos().y - ImGui::GetMousePos().y;
				clickedPosition.x = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x;
			}

			if (!ImGui::IsMouseDown(0) && m_currentSelectedObject != -1)
			{

				if (moved && EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).position != vec2(clickedPosition.x, clickedPosition.y))
				{
					m_undoStack.AddChange(new ValueChange<vec2>(startPosition, EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).position, &EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).position));
				}

				if(rotating && rotation != EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).orientation)
					m_undoStack.AddChange(new ValueChange<float>(rotation, EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).orientation, &EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).orientation));

				if (scaling)
				{
/*					if(width != EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width)
						m_undoStack.AddChange(new ValueChange<float>(width, EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width, &EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width));
					if (height != EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height)
						m_undoStack.AddChange(new ValueChange<float>(height, EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height, &EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height));
				*/	
					if (vec2(width, height) != vec2(EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width, EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height))
						m_undoStack.AddChange(new ValueChange2<float>(height, EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height, &EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).height
						,width, EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width, &EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject).width));
				}
				rotating = false;
				scaling = false;
				moved = false;
			}
			//end the window


			ImGui::EndChild();


			ImGui::End();
		}
	}

	void Editor::ShowGameView(Graphics::FrameBuffer& r_frameBuffer, bool* p_active)
	{
		if (!IsEditorActive())
		{
			//get the current viewport so that i can get the full size of the window
			ImGuiViewport* viewport = ImGui::GetMainViewport();

			//making sure the dockspace will cover the entire window
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
		}

		ImGui::Begin("Game View", p_active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration);


		m_renderWindowWidth = ImGui::GetContentRegionAvail().x;
		m_renderWindowHeight = ImGui::GetContentRegionAvail().y;

		ImGuiStyle& style = ImGui::GetStyle();
		float size = ImGui::CalcTextSize("Play").x + style.FramePadding.x * 2.5f + ImGui::CalcTextSize("Pause").x;
		float avail = ImGui::GetContentRegionAvail().x;

		float off = (float)((avail - size) * 0.5);
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off - (ImGui::CalcTextSize("Play").x + style.FramePadding.x) / 2 - (ImGui::CalcTextSize("Pause").x + style.FramePadding.x)/4);

		static bool toDisable = true;

		ImGui::BeginDisabled(toDisable);
		if (ImGui::Button("Play"))
		{
			m_isRunTime = true;	
			toDisable = true;
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::BeginDisabled(!toDisable);
		if (ImGui::Button("Pause"))
		{
			m_isRunTime = false;
			toDisable = false;
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button("Stop")) 
		{
			GameStateManager::GetInstance().ResetDefaultState();
			m_showEditor = true;
			toDisable = true;
			if (m_isRunTime)
			{
				ClearObjectList();
				serializationManager.LoadAllEntitiesFromFile("../Assets/Prefabs/savestate.json");
				engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
			}

			if (m_showEditor)
				m_isRunTime = false;

			m_showGameView = false;
		}

		//ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2.f, ImGui::GetCursorPosY()));
		if (ImGui::BeginChild("GamesViewChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar)) {
			if (r_frameBuffer.GetTextureId())
			{
				//the graphics rendered onto an image on the imgui window
				ImGui::Image(
					reinterpret_cast<void*>(
						static_cast<intptr_t>(r_frameBuffer.GetTextureId())),
					ImVec2(m_renderWindowWidth, m_renderWindowHeight),
					ImVec2(0, 1),
					ImVec2(1, 0)
				);
			}
			m_mouseInScene = ImGui::IsWindowHovered();
		}
		

		ImGui::EndChild();


		ImGui::End();

	}

	void Editor::ShowApplyWindow(bool* p_active)
	{
		if (prefabTP == "")
		{
			*p_active = false;
		}
		if (!ImGui::Begin("Modify Entities", p_active, ImGuiWindowFlags_AlwaysAutoResize)) // draw resource list
		{
			*p_active = false;
			ImGui::End(); //imgui close
		}
		else
		{
			ImGui::Text("Select what entities to apply the prefab to:");
			ImGui::Separator();
			static std::set<EntityID> modify;
			static std::map<EntityID, std::vector<bool>> modComps;
			int cnt{};
			for (const auto& id : SceneView())
			{
				if (prefabTP != "" && prefabTP == EntityManager::GetInstance().Get<EntityDescriptor>(id).prefabType)
				{
					bool tmp{ static_cast<bool>(modify.count(id)) };
					ImGui::Checkbox((std::to_string(id) + ". " + EntityManager::GetInstance().Get<EntityDescriptor>(id).name).c_str(), &tmp);
					if (tmp && !modify.count(id))
					{
						modify.emplace(id);
						if (!modComps.count(id))
						{
							modComps.emplace(id, std::vector<bool>(prefabCID.size()));

							for (auto b : modComps.at(id))
							{
								b = true;
							}
						}
					}
					if (!tmp && modify.count(id))
					{
						modify.erase(id);
						modComps.erase(id);
						
					}

					ImGui::Separator();
					if (tmp)
					{
						ImGui::Indent();
							
						for (size_t i{}; i < modComps[id].size(); ++i)
						{
							if (prefabCID.at(i) == EntityManager::GetInstance().GetComponentID<EntityDescriptor>())
								continue;

							bool tmp2 = modComps[id].at(i);
							std::string name = EntityManager::GetInstance().m_componentNames.at(prefabCID.at(i)).c_str();
							name += "##";
							name += std::to_string(i);
							name += std::to_string(id);
							ImGui::Checkbox(name.c_str(), &tmp2);
							modComps[id].at(i) = tmp2;
						}

						ImGui::Unindent();
						ImGui::Separator();
					}
					++cnt;
				}
			}
			if (!cnt)
				*p_active = false;
			ImGui::Separator();

			if (ImGui::Button("Apply"))
			{
				// exectue the changes!!
				EntityID pfid = serializationManager.LoadFromFile(prefabFP);
				for (auto id : modify)
				{
					for (size_t i{}; i < prefabCID.size(); ++i)
					{
						if (modComps.at(id).at(i) && prefabCID.at(i) != EntityManager::GetInstance().GetComponentID<EntityDescriptor>())
						{
							if (prefabCID.at(i) == EntityManager::GetInstance().GetComponentID<Transform>())
							{
								Transform xform = EntityManager::GetInstance().Get<Transform>(pfid);
								xform.position = EntityManager::GetInstance().Get<Transform>(id).position;
								EntityFactory::GetInstance().LoadComponent(id, prefabCID.at(i), &xform);
							}
							else
							{
								EntityFactory::GetInstance().LoadComponent(id, prefabCID.at(i), EntityManager::GetInstance().GetComponentPoolPointer(prefabCID.at(i))->Get(pfid));
							}
						}
					}
				}
				EntityManager::GetInstance().RemoveEntity(pfid);
				modify.clear();
				modComps.clear();
				*p_active = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				*p_active = false;
			}
			// do some apply, set boolean to false

			ImGui::End();
		}
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

	void Editor::HotLoadingNewFiles(GLFWwindow* p_window, int count, const char** p_paths)
	{
		// prints the number of directories / files dragged over
		m_fileDragged = true;
		std::stringstream sstream;
		sstream << "Drag and drop count - " << count;
		engine_logger.AddLog(false, sstream.str(), "");

		std::vector<std::filesystem::path> consolidatedPaths;
		for (int i{ 0 }; i < count; ++i)
		{
			consolidatedPaths.emplace_back(p_paths[i]);
		}
		for (std::filesystem::path const& r_path : consolidatedPaths)
		{
			if (!std::filesystem::equivalent(r_path.parent_path(), m_parentPath))
				std::filesystem::copy(r_path, std::filesystem::path{ m_parentPath.string() + "/" + r_path.filename().string()}, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
		}
	}

	void Editor::SetImGUIStyle_Dark()
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

	void Editor::LoadSceneFromGivenPath(std::string_view path)
	{
		m_undoStack.ClearStack();
		ClearObjectList();
		serializationManager.LoadAllEntitiesFromFile(path);
	}

	void Editor::SetImGUIStyle_Pink()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(HEX(255), HEX(255), HEX(255), 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(.75f, .61f, .66f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(1.00f, .75f, .85f, 1.00f);;
		colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, .35f, .39f, 1.00f);;
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.26f, 0.61f, 0.78f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
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

	}



	void Editor::SetImGUIStyle_Blue()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(HEX(0), HEX(0), HEX(0), 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.70f, 0.82f, .95f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(.95f, 0.95f, .95f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.32f, 0.58f, 0.99f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1, 1, 1.00f);;
		colors[ImGuiCol_TitleBgActive] = ImVec4(.7f, .7f, .7f, 1.00f);;
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(1.f, 1.f, 1.f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(.75f, 0.75f, 0.75f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(.95f, .95f, .95f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(.85f, .85f, .85f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(.8f, .8f, .8f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(.95f, 0.95f, 0.95f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(1.f, 1.f, 1.f, 0.40f); // buttons
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(1.f, 1.f, 1.f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
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
	}

	std::string GenerateTimestampID() {
		auto now = std::chrono::system_clock::now();
		auto now_c = std::chrono::system_clock::to_time_t(now);
		return std::to_string(now_c);
	}
}
