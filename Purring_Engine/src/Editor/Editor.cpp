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
#include "Logic/GameStateController_v2_0.h"
#include "Logic/UI/HealthBarScript_v2_0.h"
#include "Logic/DeploymentScript.h"
#include "Logic/MainMenuController.h"
#include "Logic/Cat/CatScript_v2_0.h"
#include "Logic/IntroCutsceneController.h"
#include "GUISystem.h"
#include "GUI/Canvas.h"
#include "Utilities/FileUtilities.h"
#include <random>
#include <cmath>
#include <rttr/type.h>
#include "Graphics/CameraManager.h"
#include "Graphics/Text.h"
#include "GameStateManager.h"
#include "Data/json.hpp"
#include "Input/InputSystem.h"
#include "Layers/CollisionLayer.h"
#include "Logic/CatScript.h"
#include "Logic/RatScript.h"
#include "UndoStack.h"
#include "System.h"
#include "Math/MathCustom.h"
#include "SceneManager/SceneManager.h"
#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/Rat/RatIdle_v2_0.h"
#include "Layers/LayerManager.h"
#include "Logic/IntroCutsceneController.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>


//Hierarchy
#include "Hierarchy/HierarchyManager.h"

// interaction layer manager
#include "Layers/LayerManager.h"

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

			if (configJson["Editor"].contains("showLayerWindow"))
				m_showLayerWindow = configJson["Editor"]["showLayerWindow"].get<bool>(); 

			if (configJson["Editor"].contains("layerSettings"))
			{
				LayerState layer = configJson["Editor"]["layerSettings"].get<unsigned long long>();
				LayerManager::GetInstance().SetLayerState(layer);
			}


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
			m_showComponentWindow = true;
			m_showResourceWindow = true;
			m_showPerformanceWindow = false;
			m_showAnimationWindow = false;
			m_showPhysicsWindow = false;
			//show the entire gui 
			m_showEditor = true; // depends on the mode, whether we want to see the scene or the editor
			m_renderDebug = true; // whether to render debug lines
			m_isPrefabMode = false;
			m_showLayerWindow = false;
		}

		configFile.close();

		m_showGameView = false;

		// Set the default visual theme of the editor
		m_currentStyle = GuiStyle::DARK;

		//Subscribe to key pressed event 
		ADD_KEY_EVENT_LISTENER(PE::KeyEvents::KeyTriggered, Editor::OnKeyTriggeredEvent, this)
		//for the object list
		m_objectIsSelected = false;
		m_currentSelectedObject = -1;
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
		configJson["Editor"]["showComponentWindow"] = m_showComponentWindow;
		configJson["Editor"]["showResourceWindow"] = m_showResourceWindow;
		configJson["Editor"]["showPerformanceWindow"] = m_showPerformanceWindow;
		configJson["Editor"]["showAnimationWindow"] = m_showAnimationWindow;
		configJson["Editor"]["showPhysicsWindow"] = m_showPhysicsWindow;
		configJson["Editor"]["showLayerWindow"] = m_showLayerWindow;

		//show the entire gui 
		configJson["Editor"]["showEditor"] = true; // depends on the mode, whether we want to see the scene or the editor
		configJson["Editor"]["renderDebug"] = m_renderDebug; // whether to render debug lines
		configJson["Editor"]["isPrefabMode"] = m_isPrefabMode;
		configJson["Editor"]["layerSettings"] = LayerManager::GetInstance().GetLayerState().to_ullong();


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

	bool Editor::IsSceneViewFocused()
	{
		return m_sceneViewFocused;
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

	bool Editor::CompareFloat16Arrays(float* a, float* b)
	{
		for (int i = 0; i < 16; ++i) {
			if (a[i] != b[i]) {
				return false; // Values differ by more than epsilon
			}
		}
		return true; // All values are within epsilon of each other
	}

	void Editor::ping()
	{
		AddConsole("pong");
	}

	void Editor::test()
	{
		AddConsole("test");
	}

	void Editor::ClearObjectList()
	{
		// make sure not hovering any objects as we are deleting
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
		Hierarchy::GetInstance().Update();
		LayerManager::GetInstance().ResetLayerCache();
	}

	void Editor::Init()
	{
		//check imgui's version 
		IMGUI_CHECKVERSION();
		//create imgui context 
		ImGui::CreateContext();

		// Load Editor Fonts
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("../Assets/Fonts/San Francisco/SFUIText-Regular.ttf", 16.0f);
		io.Fonts->Build();

		switch (m_currentStyle)
		{
		case GuiStyle::DARK:
			SetImGUIStyle_Dark();
			break;
		case GuiStyle::LIGHT:
			SetImGUIStyle_Light();
			break;
		case GuiStyle::KURUMI:
			SetImGUIStyle_Kurumi();
			break;
		case GuiStyle::PINK:
			SetImGUIStyle_Pink();
			break;
		case GuiStyle::BLUE:
			SetImGUIStyle_Blue();
			break;
		}

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		//getting the full display size of glfw so that the ui know where to be in
		int width, height;
		glfwGetWindowSize(WindowManager::GetInstance().GetWindow(), &width, &height);
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
		ImGui_ImplGlfw_InitForOpenGL(WindowManager::GetInstance().GetWindow(), true);

		ImGui_ImplOpenGL3_Init("#version 450");
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

			//resource window for drag n drop
			if (m_showResourceWindow) ShowResourceWindow(&m_showResourceWindow);

			//performance window showing time used per system
			if (m_showPerformanceWindow) ShowPerformanceWindow(&m_showPerformanceWindow);

			if (m_showAnimationWindow) ShowAnimationWindow(&m_showAnimationWindow);

			if (m_showPhysicsWindow) ShowPhysicsWindow(&m_showPhysicsWindow);

			if (m_showGameView) ShowGameView(r_frameBuffer , &m_showGameView);

			if (m_applyPrefab) ShowApplyWindow(&m_applyPrefab);

			if (m_showLayerWindow) ShowLayerWindow(&m_showLayerWindow);

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
							nlohmann::json save = serializationManager.SerializeEntityPrefab(1);
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
						
						StopAndLoadScene();

						engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
						ImGui::ClosePopupToLevel(0, true);
					}
					//ImGui::SameLine();
					ImGui::Separator();
					if (ImGui::Selectable("No"))
					{
						m_isPrefabMode = false;

						// deselect object
						StopAndLoadScene();

						engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
						ImGui::ClosePopupToLevel(0, true);
					}
					ImGui::EndPopup();
					UndoStack::GetInstance().ClearStack();
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
		const LogColors& colors = GetLogColorsForCurrentStyle();

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
						color = colors.errorColor; has_color = true;
					}
					else if (i->find("[INFO]") != std::string::npos)
					{
						if (infofilter)
							continue;
						color = colors.infoColor; has_color = true;
					}
					else if (i->find("[WARNING]") != std::string::npos)
					{
						if (warningfilter)
							continue;
						color = colors.warningColor; has_color = true;
					}
					else if (i->find("[EVENT]") != std::string::npos)
					{
						if (eventfilter)
							continue;
						color = colors.eventColor; has_color = true;
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

	void Editor::ObjectWindowHelper(const EntityID& r_id, bool& r_selected, bool& r_hoveringObject, bool& r_drag, std::optional<EntityID>& r_hoveredObject, std::optional<EntityID>& r_dragID, std::string& r_dragName, std::set<std::string>& usedNames)
	{
		if (EntityManager::GetInstance().Get<EntityDescriptor>(r_id).children.size())// has children
		{
			ImGui::Indent();
			std::map<EntityID, EntityID> childOrder;
			for (const auto& childID : EntityManager::GetInstance().Get<EntityDescriptor>(r_id).children)
			{
				while (childOrder.count(EntityManager::GetInstance().Get<EntityDescriptor>(childID).sceneID))
					++EntityManager::GetInstance().Get<EntityDescriptor>(childID).sceneID;
				childOrder[EntityManager::GetInstance().Get<EntityDescriptor>(childID).sceneID] = childID;
			}
			for (auto [k,v] : childOrder)
			{
				if (!EntityManager::GetInstance().Get<EntityDescriptor>(v).isAlive)
					continue;
				std::string name2;


				name2 += EntityManager::GetInstance().Get<EntityDescriptor>(v).name;

				if (usedNames.count(name2))
					name2 += "- " + std::to_string(v);

				usedNames.emplace(name2);

				r_selected = (m_currentSelectedObject == static_cast<int>(v));

				if (!LayerManager::GetInstance().GetLayerState(EntityManager::GetInstance().Get<EntityDescriptor>(v).interactionLayer) || !EntityManager::GetInstance().Get<EntityDescriptor>(v).isActive)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
				}

				if ((name2.length()) ? ImGui::Selectable(name2.c_str(), r_selected) : ImGui::Selectable(("##" + name2).c_str(), r_selected)) //imgui selectable is the function to make the clickable bar of text
					m_currentSelectedObject = static_cast<int>(v);

				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered()) 
				{
					r_hoveringObject = true;
					r_hoveredObject = v;
					if (ImGui::IsMouseDragging(0) && r_drag == false)
					{
						m_currentSelectedObject = static_cast<int>(v); //seteting current index to check for selection
						r_dragName = name2;
						r_drag = true;
						r_dragID = v;
					}
				}

				if (ImGui::IsItemClicked(1))
				{
					//m_currentSelectedObject = static_cast<int>(r_hoveredObject.value());
					if (r_hoveredObject)
						m_currentSelectedObject = static_cast<int>(r_hoveredObject.value());
					ImGui::OpenPopup("popup");
				}
				ObjectWindowHelper(v, r_selected, r_hoveringObject, r_drag, r_hoveredObject, r_dragID, r_dragName, usedNames);

			}

			ImGui::Unindent();
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
			std::set<std::string> usedNames;
			if (ImGui::BeginChild("GameObjectList", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) 
			{
				m_mouseInObjectWindow = ImGui::IsWindowHovered();
				for (const EntityID& id : Hierarchy::GetInstance().GetHierarchyOrder())
				{
					// skip camera
					if (id == Graphics::CameraManager::GetUiCameraId())
						continue;

					if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).isAlive)
						continue;

					std::string name;

					name += EntityManager::GetInstance().Get<EntityDescriptor>(id).name;

					if (usedNames.count(name))
						name += " - " + std::to_string(id);
					
					usedNames.emplace(name);

					bool is_selected = (m_currentSelectedObject == static_cast<int>(id));

					if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.has_value())
					{
						if (!LayerManager::GetInstance().GetLayerState(EntityManager::GetInstance().Get<EntityDescriptor>(id).interactionLayer) || !EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
						}

						if ((name.length())? ImGui::Selectable(name.c_str(), is_selected) : ImGui::Selectable(("##" + name).c_str(), is_selected)) //imgui selectable is the function to make the clickable bar of text
							m_currentSelectedObject = static_cast<int>(id);

						ImGui::PopStyleColor();

						if (ImGui::IsItemHovered()) {
							isHoveringObject = true;
							hoveredObject = id;
							if (ImGui::IsMouseDragging(0) && drag == false)
							{
								//seteting current index to check for selection
								dragName = name;
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
						ObjectWindowHelper(id, is_selected, isHoveringObject, drag, hoveredObject, dragID, dragName, usedNames);
						
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
							if (hoveredObject)
							{
								if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
								{
									std::swap(EntityManager::GetInstance().Get<EntityDescriptor>(dragID.value()).sceneID, EntityManager::GetInstance().Get<EntityDescriptor>(hoveredObject.value()).sceneID);
								}
								else
								{
									Hierarchy::GetInstance().AttachChild(hoveredObject.value(), dragID.value());
								}
							}
							else
							{
								Hierarchy::GetInstance().DetachChild(dragID.value());
								EntityID order = 1;
								EntityID largest = 1;
								for (const auto& id : Hierarchy::GetInstance().GetHierarchyOrder())
								{
									if (id == dragID.value())
										continue;
									largest = (EntityManager::GetInstance().Get<EntityDescriptor>(id).sceneID < largest) ? largest : EntityManager::GetInstance().Get<EntityDescriptor>(id).sceneID;
									EntityManager::GetInstance().Get<EntityDescriptor>(id).sceneID = order++;
								}
								EntityManager::GetInstance().Get<EntityDescriptor>(dragID.value()).sceneID = order;
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
						
						if (m_currentSelectedObject != -1)
						{
							if (EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).children.size())
							{
								for (auto cid : EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).children)
								{
									EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).savedChildren.emplace_back(cid);
								}

								for (const auto& cid : EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).savedChildren)
								{
									if (EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).parent.has_value())
										Hierarchy::GetInstance().AttachChild(EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).parent.value(), cid);
									else
										Hierarchy::GetInstance().DetachChild(cid);
								}
							}
							if (EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).parent)
							{
								EntityManager::GetInstance().Get<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).parent.value()).children.erase(m_currentSelectedObject);
							}

							UndoStack::GetInstance().AddChange(new DeleteObjectUndo(m_currentSelectedObject));
							EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).HandicapEntity();
						}

						m_currentSelectedObject = -1; // just reset it
						//if object selected
						m_objectIsSelected = false;
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
			
			EntityID NextCanvasID{};
			if (ImGui::IsItemClicked(1) && !isHoveringObject)
			{
				ImGui::OpenPopup("Object");
			}
			if (ImGui::BeginPopup("Object"))
			{
				if (ImGui::Selectable("Create Empty Object"))
				{
						EntityID s_id = serializationManager.LoadFromFile("EditorDefaults/Empty_Prefab.json");
						UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));

						m_currentSelectedObject = static_cast<int>(s_id);
				}
				if (ImGui::BeginMenu("Create UI Object"))
				{
					if (ImGui::MenuItem("Create Canvas Object")) // the ctrl s is not programmed yet, need add to the key press event
					{
						EntityID s_id = serializationManager.LoadFromFile("EditorDefaults/Canvas_Prefab.json");
						UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));
						m_currentSelectedObject = static_cast<int>(s_id);
					}
					if (ImGui::MenuItem("Create UI Object")) // the ctrl s is not programmed yet, need add to the key press event
					{
		
						NextCanvasID = CheckCanvas();
						EntityID s_id = serializationManager.LoadFromFile("EditorDefaults/UIObject_Prefab.json");
						Hierarchy::GetInstance().AttachChild(NextCanvasID, s_id);
						UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));
						m_currentSelectedObject = static_cast<int>(s_id);
					}
					if (ImGui::MenuItem("Create UI Button")) // the ctrl s is not programmed yet, need add to the key press event
					{
						NextCanvasID = CheckCanvas();
						EntityID s_id = serializationManager.LoadFromFile("EditorDefaults/Button_Prefab.json");
						Hierarchy::GetInstance().AttachChild(NextCanvasID, s_id);
						UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));
						m_currentSelectedObject = static_cast<int>(s_id);
					}
					if (ImGui::MenuItem("Create UI Slider")) // the ctrl s is not programmed yet, need add to the key press event
					{
						NextCanvasID = CheckCanvas();
						EntityID s_id = serializationManager.LoadFromFile("EditorDefaults/SliderBody_Prefab.json");
						Hierarchy::GetInstance().AttachChild(NextCanvasID, s_id);
						UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));
						m_currentSelectedObject = static_cast<int>(s_id);
					}
					if (ImGui::MenuItem("Create Text Object")) // the ctrl s is not programmed yet, need add to the key press event
					{
						NextCanvasID = CheckCanvas();
						EntityID s_id = serializationManager.LoadFromFile("EditorDefaults/Text_Prefab.json");
						Hierarchy::GetInstance().AttachChild(NextCanvasID, s_id);
						UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));
						m_currentSelectedObject = static_cast<int>(s_id);
					}
					ImGui::EndMenu();
				}
				if (ImGui::Selectable("Create Audio Object"))
				{
					EntityID s_id = serializationManager.LoadFromFile("EditorDefaults/Audio_Prefab.json");
					UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));
					m_currentSelectedObject = static_cast<int>(s_id);
				}
				if (ImGui::Selectable("Create Camera Object"))
				{
					EntityID s_id = serializationManager.LoadFromFile("EditorDefaults/Camera_Prefab.json");
					UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));
					m_currentSelectedObject = static_cast<int>(s_id);
				}
				ImGui::EndPopup();
			}

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
					EntityID entityID = /*(m_isPrefabMode) ? 1 :*/ m_currentSelectedObject;
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
										ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);  (prop.get_name().to_string() == "Name")?ImGui::InputText(str.c_str(), &tmp) : ImGui::Text(tmp.c_str());
										prop.set_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID), tmp);

									}
									else if (vp.get_type().get_name() == "classstd::optional<unsigned__int64>")
									{
										std::optional<EntityID> tmp = vp.get_value<std::optional<EntityID>>();

										std::string str = "##" + prop.get_name().to_string();
										
										std::string tmpStr{(tmp.has_value())?std::to_string(tmp.value()) : "None"};
										ImGui::SameLine(); ImGui::SetNextItemWidth(100.f);  ImGui::Text(tmpStr.c_str());
											
									}
									else if (vp.get_type().get_name() == "bool")
									{
										bool tmp = vp.get_value<bool>();
										std::string str = "##" + prop.get_name().to_string();
										ImGui::SameLine(); ImGui::Checkbox(str.c_str(), &tmp);
										//prop.set_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID), tmp);
										if (prop.get_name().to_string() == "Active" && tmp != vp.get_value<bool>())
										{
											(tmp) ? EntityManager::GetInstance().Get<EntityDescriptor>(entityID).EnableEntity() : EntityManager::GetInstance().Get<EntityDescriptor>(entityID).DisableEntity();
										}
									}
									else if (vp.get_type().get_name() == "int")
									{
										if (prop.get_name().to_string() == "Render Layer")
										{
											int tmp = vp.get_value<int>();
											std::string str = "##" + prop.get_name().to_string();
											if (EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent)
												ImGui::BeginDisabled();

											ImGui::SameLine(); ImGui::SliderInt(str.c_str(), &tmp, 0, 10);

											if (EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent)
												ImGui::EndDisabled();
											prop.set_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID), tmp);
											EntityManager::GetInstance().Get<EntityDescriptor>(entityID).SetLayer(tmp);
										}	
										else if (prop.get_name().to_string() == "Interaction Layer")
										{
											int tmp = vp.get_value<int>();
											std::string str = "##" + prop.get_name().to_string();
											/*if (EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent)
												ImGui::BeginDisabled();*/

											ImGui::SameLine(); ImGui::SliderInt(str.c_str(), &tmp, 0, 10);

											/*if (EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent)
												ImGui::EndDisabled();*/
											prop.set_value(EntityManager::GetInstance().Get<EntityDescriptor>(entityID), tmp);
											EntityManager::GetInstance().Get<EntityDescriptor>(entityID).interactionLayer = tmp;
											LayerManager::GetInstance().UpdateEntity(entityID);
										}
									}
									else if (vp.get_type().get_name() == "unsigned__int64")
									{
										if (prop.is_readonly())
										{
											ImGui::SameLine(); ImGui::Text(std::to_string(vp.get_value<EntityID>()).c_str());
										}
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
											if (EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<GUIButton>()))
												continue;											
											if (EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<GUISlider>()))
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
												if(!EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent.has_value())
												UndoStack::GetInstance().AddChange(new ValueChange<float>(prevVal, tmp.x, &EntityManager::GetInstance().Get<Transform>(entityID).position.x));
												else
												UndoStack::GetInstance().AddChange(new ValueChange<float>(prevVal, tmp.x, &EntityManager::GetInstance().Get<Transform>(entityID).relPosition.x));
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
												if (!EntityManager::GetInstance().Get<EntityDescriptor>(entityID).parent.has_value())
												UndoStack::GetInstance().AddChange(new ValueChange<float>(prevVal, tmp.y, &EntityManager::GetInstance().Get<Transform>(entityID).position.y));
												else
													UndoStack::GetInstance().AddChange(new ValueChange<float>(prevVal, tmp.y, &EntityManager::GetInstance().Get<Transform>(entityID).relPosition.y));
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
											if (ImGui::IsItemDeactivatedAfterEdit() && !CompareFloats(prevVal, tmp))
											{
												if (prop.get_name() == "Width")
													UndoStack::GetInstance().AddChange(new ValueChange<float>(prevVal, tmp, &EntityManager::GetInstance().Get<Transform>(entityID).width));
												if (prop.get_name() == "Height")
													UndoStack::GetInstance().AddChange(new ValueChange<float>(prevVal, tmp, &EntityManager::GetInstance().Get<Transform>(entityID).height));
												if (prop.get_name() == "Orientation")
														UndoStack::GetInstance().AddChange(new ValueChange<float>(prevVal, tmp, &EntityManager::GetInstance().Get<Transform>(entityID).orientation));
												if (prop.get_name() == "Relative Orientation")
													UndoStack::GetInstance().AddChange(new ValueChange<float>(prevVal, tmp, &EntityManager::GetInstance().Get<Transform>(entityID).relOrientation));
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

									if (!loadedTextureKeys.empty())
									{


										// Shows a drop down of selectable textures
										ImGui::Text("Textures: "); ImGui::SameLine();
										ImGui::SetNextItemWidth(300.0f);
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

									//create a combo box of texture ids
									ImGui::SetNextItemWidth(200.0f);
									if (!loadedTextureKeys.empty())
									{
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


						if (name == EntityManager::GetInstance().GetComponentID<GUIButton>() && EntityManager::GetInstance().Has<GUIButton>(entityID))
						{
							if (ImGui::CollapsingHeader("GUIButton", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
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
										EntityManager::GetInstance().Remove<GUIButton>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Checkbox("Disabled", &EntityManager::GetInstance().Get<GUIButton>(entityID).disabled);
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
									if (it->first == EntityManager::GetInstance().Get<GUIButton>(entityID).m_hoveredTexture)
										hoveredText = i;
									if (it->first == EntityManager::GetInstance().Get<GUIButton>(entityID).m_pressedTexture)
										pressedTexture = i;
									if (it->first == EntityManager::GetInstance().Get<GUIButton>(entityID).m_disabledTexture)
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
												EntityManager::GetInstance().Get<GUIButton>(entityID).m_defaultTexture = filepaths[n].string();
											}
										}
										ImGui::EndCombo();
									}

									//get and set color variable of the renderer component
									vec4 defaultColor = EntityManager::GetInstance().Get<GUIButton>(entityID).m_defaultColor;
									ImVec4 defaultcolor(defaultColor.x, defaultColor.y, defaultColor.z, defaultColor.w);

									ImGui::Text("default Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Default Color", (float*)&defaultcolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUIButton>(entityID).m_defaultColor = vec4(defaultcolor.x, defaultcolor.y, defaultcolor.z, defaultcolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Hovered Texture:");
									if (ImGui::BeginCombo("##hoveredtext", loadedTextureKeys[hoveredText].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str())) 
											{
												EntityManager::GetInstance().Get<GUIButton>(entityID).m_hoveredTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 hoverColor = EntityManager::GetInstance().Get<GUIButton>(entityID).m_hoveredColor;
									ImVec4 hovercolor(hoverColor.x, hoverColor.y, hoverColor.z, hoverColor.w);

									ImGui::Text("hover Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Hover Color", (float*)&hovercolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUIButton>(entityID).m_hoveredColor = vec4(hovercolor.x, hovercolor.y, hovercolor.z, hovercolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Pressed Texture:");
									if (ImGui::BeginCombo("##pressedtext", loadedTextureKeys[pressedTexture].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
											{
												EntityManager::GetInstance().Get<GUIButton>(entityID).m_pressedTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 pressColor = EntityManager::GetInstance().Get<GUIButton>(entityID).m_pressedColor;
									ImVec4 presscolor(pressColor.x, pressColor.y, pressColor.z, pressColor.w);

									ImGui::Text("Press Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Press Color", (float*)&presscolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUIButton>(entityID).m_pressedColor = vec4(presscolor.x, presscolor.y, presscolor.z, presscolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Disabled Texture:");
									if (ImGui::BeginCombo("##disabledtext", loadedTextureKeys[disabledText].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
											{
												EntityManager::GetInstance().Get<GUIButton>(entityID).m_disabledTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 disableColor = EntityManager::GetInstance().Get<GUIButton>(entityID).m_disabledColor;
									ImVec4 disablecolor(disableColor.x, disableColor.y, disableColor.z, disableColor.w);

									ImGui::Text("Disable Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change disable Color", (float*)&disablecolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUIButton>(entityID).m_disabledColor = vec4(disablecolor.x, disablecolor.y, disablecolor.z, disablecolor.w);


								}

													

								ImGui::SeparatorText("Events");

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
									if (str == EntityManager::GetInstance().Get<GUIButton>(entityID).m_onClicked)
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
										EntityManager::GetInstance().Get<GUIButton>(entityID).m_onClicked = key[onclickfunc];
									}
								}

								int onhoverfunc{};
								for (std::string str : key)
								{
									if (str == EntityManager::GetInstance().Get<GUIButton>(entityID).m_onHovered)
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
											EntityManager::GetInstance().Get<GUIButton>(entityID).m_onHovered = key[onhoverfunc];
									}
								}

								int onhoverenterfunc{};
								for (std::string str : key)
								{
									if (str == EntityManager::GetInstance().Get<GUIButton>(entityID).m_onHoverEnter)
										break;
									onhoverenterfunc++;
								}
								//create a combo box of scripts
								ImGui::SetNextItemWidth(200.0f);
								if (!key.empty())
								{
									ImGui::Text("On Hover Enter: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									//set selected texture id
									if (ImGui::Combo("##On Hover Enter Function", &onhoverenterfunc, key.data(), static_cast<int>(key.size())))
									{
										EntityManager::GetInstance().Get<GUIButton>(entityID).m_onHoverEnter = key[onhoverenterfunc];
									}
								}

								int onhoverexitfunc{};
								for (std::string str : key)
								{
									if (str == EntityManager::GetInstance().Get<GUIButton>(entityID).m_onHoverExit)
										break;
									onhoverexitfunc++;
								}
								//create a combo box of scripts
								ImGui::SetNextItemWidth(200.0f);
								if (!key.empty())
								{
									ImGui::Text("On Hover Exit: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);
									//set selected texture id
									if (ImGui::Combo("##On Hover Exit Function", &onhoverexitfunc, key.data(), static_cast<int>(key.size())))
									{
										EntityManager::GetInstance().Get<GUIButton>(entityID).m_onHoverExit = key[onhoverexitfunc];
									}
								}
							}
						}
                        
						if (name == EntityManager::GetInstance().GetComponentID<GUISlider>() && EntityManager::GetInstance().Has<GUISlider>(entityID))
						{
							if (ImGui::CollapsingHeader("GUISlider", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
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
										EntityManager::GetInstance().Remove<GUISlider>(entityID);
									}
									ImGui::EndPopup();
								}

								if (ImGui::Button(o.c_str()))
									ImGui::OpenPopup(id.c_str());
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
									if (it->first == EntityManager::GetInstance().Get<GUISlider>(entityID).m_hoveredTexture)
										hoveredText = i;
									if (it->first == EntityManager::GetInstance().Get<GUISlider>(entityID).m_pressedTexture)
										pressedTexture = i;
									if (it->first == EntityManager::GetInstance().Get<GUISlider>(entityID).m_disabledTexture)
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
												EntityManager::GetInstance().Get<GUISlider>(entityID).m_defaultTexture = filepaths[n].string();
											}
										}
										ImGui::EndCombo();
									}

									//get and set color variable of the renderer component
									vec4 defaultColor = EntityManager::GetInstance().Get<GUISlider>(entityID).m_defaultColor;
									ImVec4 defaultcolor(defaultColor.x, defaultColor.y, defaultColor.z, defaultColor.w);

									ImGui::Text("default Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Default Color", (float*)&defaultcolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUISlider>(entityID).m_defaultColor = vec4(defaultcolor.x, defaultcolor.y, defaultcolor.z, defaultcolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Hovered Texture:");
									if (ImGui::BeginCombo("##hoveredtext", loadedTextureKeys[hoveredText].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
											{
												EntityManager::GetInstance().Get<GUISlider>(entityID).m_hoveredTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 hoverColor = EntityManager::GetInstance().Get<GUISlider>(entityID).m_hoveredColor;
									ImVec4 hovercolor(hoverColor.x, hoverColor.y, hoverColor.z, hoverColor.w);

									ImGui::Text("hover Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Hover Color", (float*)&hovercolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUISlider>(entityID).m_hoveredColor = vec4(hovercolor.x, hovercolor.y, hovercolor.z, hovercolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Pressed Texture:");
									if (ImGui::BeginCombo("##pressedtext", loadedTextureKeys[pressedTexture].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
											{
												EntityManager::GetInstance().Get<GUISlider>(entityID).m_pressedTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 pressColor = EntityManager::GetInstance().Get<GUISlider>(entityID).m_pressedColor;
									ImVec4 presscolor(pressColor.x, pressColor.y, pressColor.z, pressColor.w);

									ImGui::Text("Press Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change Press Color", (float*)&presscolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUISlider>(entityID).m_pressedColor = vec4(presscolor.x, presscolor.y, presscolor.z, presscolor.w);
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									ImGui::Text("Disabled Texture:");
									if (ImGui::BeginCombo("##disabledtext", loadedTextureKeys[disabledText].c_str())) // The second parameter is the label previewed before opening the combo.
									{
										for (int n{ 0 }; n < loadedTextureKeys.size(); ++n)
										{
											if (ImGui::Selectable(loadedTextureKeys[n].c_str()))
											{
												EntityManager::GetInstance().Get<GUISlider>(entityID).m_disabledTexture = filepaths[n].string();
												//std::cout << filepaths[n].string() << std::endl;
											}
										}
										ImGui::EndCombo();
									}

									vec4 disableColor = EntityManager::GetInstance().Get<GUISlider>(entityID).m_disabledColor;
									ImVec4 disablecolor(disableColor.x, disableColor.y, disableColor.z, disableColor.w);

									ImGui::Text("Disable Color: "); ImGui::SameLine();
									ImGui::ColorEdit4("##Change disable Color", (float*)&disablecolor, ImGuiColorEditFlags_AlphaPreview);

									EntityManager::GetInstance().Get<GUISlider>(entityID).m_disabledColor = vec4(disablecolor.x, disablecolor.y, disablecolor.z, disablecolor.w);
								}

								ImGui::SeparatorText("Value");
								ImGui::Checkbox("Is Health Bar", &EntityManager::GetInstance().Get<GUISlider>(entityID).m_isHealthBar);
								ImGui::Text("Knob Width: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.0f);
								ImGui::DragFloat("##knobw", &EntityManager::GetInstance().Get<GUISlider>(entityID).m_currentWidth);
								ImGui::Text("Min Value: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.0f);
								ImGui::DragFloat("##Min Value", &EntityManager::GetInstance().Get<GUISlider>(entityID).m_minValue);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Text("Max Value: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.0f);
								ImGui::DragFloat("##Max Value", &EntityManager::GetInstance().Get<GUISlider>(entityID).m_maxValue);
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
								ImGui::Text("Current Value: "); ImGui::SameLine();
								ImGui::SetNextItemWidth(200.0f);
								ImGui::DragFloat("##Current Value", &EntityManager::GetInstance().Get<GUISlider>(entityID).m_currentValue);
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
									float lineSpacing{ EntityManager::GetInstance().Get<TextComponent>(entityID).GetLineSpacing() };

									EnumTextAlignment selectedButtonHAlignment{ EntityManager::GetInstance().Get<TextComponent>(entityID).GetHAlignment() };
									EnumTextAlignment selectedButtonVAlignment{ EntityManager::GetInstance().Get<TextComponent>(entityID).GetVAlignment() };

									ImGui::Text("Font Size: "); ImGui::SameLine(); ImGui::InputFloat("##FontSize", &size, 1.0f, 100.f, "%.3f");
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

									ImGui::Text("Line Spacing: "); ImGui::SameLine(); ImGui::InputFloat("##LineSpacing", &lineSpacing, 1.0f, 100.f, "%.3f");
									ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

									EntityManager::GetInstance().Get<TextComponent>(entityID).SetSize(size);
									EntityManager::GetInstance().Get<TextComponent>(entityID).SetLineSpacing(lineSpacing);

									// Horizontal Alignment
									ImGui::Text("Horizontal Alignment ");
									
									ImGui::SameLine();									
									ImGui::BeginDisabled(selectedButtonHAlignment == EnumTextAlignment::LEFT);
									if (ImGui::Button("Left"))
									{
										selectedButtonHAlignment = EnumTextAlignment::LEFT;
									}
									ImGui::EndDisabled();

									ImGui::SameLine();
									ImGui::BeginDisabled(selectedButtonHAlignment == EnumTextAlignment::CENTER);
									if (ImGui::Button("Center##hAlignment"))
									{
										selectedButtonHAlignment = EnumTextAlignment::CENTER;
									}
									ImGui::EndDisabled();

									ImGui::SameLine();
									ImGui::BeginDisabled(selectedButtonHAlignment == EnumTextAlignment::RIGHT);
									if (ImGui::Button("Right"))
									{										
										selectedButtonHAlignment = EnumTextAlignment::RIGHT;
									}
									ImGui::EndDisabled();

									EntityManager::GetInstance().Get<TextComponent>(entityID).SetHAlignment(selectedButtonHAlignment);

									// Vertical Alignment
									ImGui::Text("Vertical Alignment ");

									ImGui::SameLine();
									ImGui::BeginDisabled(selectedButtonVAlignment == EnumTextAlignment::TOP);
									if (ImGui::Button("Top"))
									{
										selectedButtonVAlignment = EnumTextAlignment::TOP;
									}
									ImGui::EndDisabled();

									ImGui::SameLine();
									ImGui::BeginDisabled(selectedButtonVAlignment == EnumTextAlignment::CENTER);
									if (ImGui::Button("Center##vAlignment"))
									{
										selectedButtonVAlignment = EnumTextAlignment::CENTER;
									}
									ImGui::EndDisabled();

									ImGui::SameLine();
									ImGui::BeginDisabled(selectedButtonVAlignment == EnumTextAlignment::BOTTOM);
									if (ImGui::Button("Bottom"))
									{
										selectedButtonVAlignment = EnumTextAlignment::BOTTOM;
									}
									ImGui::EndDisabled();

									EntityManager::GetInstance().Get<TextComponent>(entityID).SetVAlignment(selectedButtonVAlignment);

									// Overflow
									int hOverflowIndex = static_cast<int>(EntityManager::GetInstance().Get<TextComponent>(entityID).GetHOverflow());
									int vOverflowIndex = static_cast<int>(EntityManager::GetInstance().Get<TextComponent>(entityID).GetVOverflow()) - 1; // shared enum
									const char* hOverflowTypes[] = { "Wrap", "Overflow" };
									const char* vOverflowTypes[] = { "Overflow", "Truncate" };

									// horizontal overflow
									ImGui::Text("Horizontal Overflow: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);

									// set combo box for the different collider types
									if (ImGui::Combo("##hOverflow", &hOverflowIndex, hOverflowTypes, IM_ARRAYSIZE(hOverflowTypes)))
									{
										EntityManager::GetInstance().Get<TextComponent>(entityID).SetHOverflow(static_cast<EnumTextOverflow>(hOverflowIndex));
									}

									// vertical overflow
									ImGui::Text("Vertical Overflow: "); ImGui::SameLine();
									ImGui::SetNextItemWidth(200.0f);

									// set combo box for the different collider types
									if (ImGui::Combo("##vOverflow", &vOverflowIndex, vOverflowTypes, IM_ARRAYSIZE(vOverflowTypes)))
									{
										EntityManager::GetInstance().Get<TextComponent>(entityID).SetVOverflow(static_cast<EnumTextOverflow>(vOverflowIndex + 1)); // shared enum
									}

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


						// ---------- Canvas Component ---------- //

						if (name == EntityManager::GetInstance().GetComponentID<Canvas>() && EntityManager::GetInstance().Has<Canvas>(entityID))
						{
							if (ImGui::CollapsingHeader("Canvas", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
							{
								//setting reset button to open a popup with selectable text
								ImGui::SameLine();
								std::string id = "options##", o = "o##";
								id += std::to_string(componentCount);
								o += std::to_string(componentCount);

								if (ImGui::Button(o.c_str()))
										ImGui::OpenPopup(id.c_str());
								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

								Canvas const& canvasComponent{ EntityManager::GetInstance().Get<Canvas>(entityID) };
								std::stringstream iss{};
								iss << std::to_string(static_cast<int>(canvasComponent.GetWidth())) << " x " << std::to_string(static_cast<int>(canvasComponent.GetHeight()));
								ImGui::Text("Target Resolution: "); ImGui::SameLine(); ImGui::Text(iss.str().c_str());

								ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space


								if (ImGui::BeginPopup(id.c_str()))
								{
										if (ImGui::Selectable("Reset")) {}
										if (ImGui::Selectable("Remove"))
										{
												EntityManager::GetInstance().Remove<Canvas>(entityID);
										}
										ImGui::EndPopup();
								}
							}
						}
					}

					if (hasScripts)
					{
						//m_currentSelectedObject = (m_isPrefabMode) ? 1 : m_currentSelectedObject;
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

							if (key == "GameStateController_v2_0")
							{
								GameStateController_v2_0* p_script = dynamic_cast<GameStateController_v2_0*>(val);
								auto it = p_script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("GameStateController_v2_0", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										ImGui::Text("Game State Active: "); ImGui::SameLine(); ImGui::Checkbox("##act", &it->second.GameStateManagerActive);
										int PauseBackGroundCanvasID = static_cast<int> (it->second.PauseBackGroundCanvas);
										int PauseMenuCanvasID = static_cast<int> (it->second.PauseMenuCanvas);
										int AreYouSureCanvasID = static_cast<int> (it->second.AreYouSureCanvas);
										int AreYouSureRestartCanvasID = static_cast<int> (it->second.AreYouSureRestartCanvas);
										int WinCanvasID = static_cast<int> (it->second.WinCanvas);
										int LoseCanvasID = static_cast<int> (it->second.LoseCanvas);
										int HowToPlayCanvasID = static_cast<int> (it->second.HowToPlayCanvas);
										int HUDCanvasID = static_cast<int> (it->second.HUDCanvas);
										int ExecuteCanvasID = static_cast<int> (it->second.ExecuteCanvas);
										int TurnCounterCanvasID = static_cast<int> (it->second.TurnCounterCanvas);
										int HTPID1 = static_cast<int> (it->second.HowToPlayPageOne);
										int HTPID2 = static_cast<int> (it->second.HowToPlayPageTwo);
										int CatPortID = static_cast<int> (it->second.CatPortrait);
										int RatPortID = static_cast<int> (it->second.RatPortrait);
										int PortID = static_cast<int> (it->second.Portrait);
										int BackgroundID = static_cast<int> (it->second.Background);
										int JournalID = static_cast<int> (it->second.Journal);
										int JournalButtonID = static_cast<int> (it->second.JournalButton);
										int TransitionPanelID = static_cast<int> (it->second.TransitionPanel);
										int PhaseBannerID = static_cast<int> (it->second.PhaseBanner);
									
										ImGui::Text("BackgroundCanvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##bgc", &PauseBackGroundCanvasID);
										if (PauseBackGroundCanvasID != m_currentSelectedObject) { it->second.PauseBackGroundCanvas = PauseBackGroundCanvasID; }

										ImGui::Text("PauseMenuCanvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##pmc", &PauseMenuCanvasID);
										if (PauseMenuCanvasID != m_currentSelectedObject) { it->second.PauseMenuCanvas = PauseMenuCanvasID; }

										ImGui::Text("AreYouSureCanvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##ays", &AreYouSureCanvasID);
										if (AreYouSureCanvasID != m_currentSelectedObject) { it->second.AreYouSureCanvas = AreYouSureCanvasID; }

										ImGui::Text("AreYouSureRestartCanvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##aysr", &AreYouSureRestartCanvasID);
										if (AreYouSureRestartCanvasID != m_currentSelectedObject) { it->second.AreYouSureRestartCanvas = AreYouSureRestartCanvasID; }

										ImGui::Text("WinCanvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##ws", &WinCanvasID);
										if (WinCanvasID != m_currentSelectedObject) { it->second.WinCanvas = WinCanvasID; }

										ImGui::Text("LoseCanvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##ls", &LoseCanvasID);
										if (LoseCanvasID != m_currentSelectedObject) { it->second.LoseCanvas = LoseCanvasID; }

										ImGui::Text("HowToPlayCanvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##htps", &HowToPlayCanvasID);
										if (HowToPlayCanvasID != m_currentSelectedObject) { it->second.HowToPlayCanvas = HowToPlayCanvasID; }

										ImGui::Text("How to Play P1 ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##htp1", &HTPID1);
										if (HTPID1 != m_currentSelectedObject) { it->second.HowToPlayPageOne = HTPID1; }

										ImGui::Text("How to Play P2 ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##htp2", &HTPID2);
										if (HTPID2 != m_currentSelectedObject) { it->second.HowToPlayPageTwo = HTPID2; }

										ImGui::Text("HUD Canvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##hudc", &HUDCanvasID);
										if (HUDCanvasID != m_currentSelectedObject) { it->second.HUDCanvas = HUDCanvasID; }

										ImGui::Text("Foliage ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##folc", &ExecuteCanvasID);
										if (ExecuteCanvasID != m_currentSelectedObject) { it->second.ExecuteCanvas = ExecuteCanvasID; }

										ImGui::Text("Turn Counter Canvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##tcc", &TurnCounterCanvasID);
										if (TurnCounterCanvasID != m_currentSelectedObject) { it->second.TurnCounterCanvas = TurnCounterCanvasID; }

										ImGui::Text("Cat Portrait Canvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##cpt", &CatPortID);
										if (CatPortID != m_currentSelectedObject) { it->second.CatPortrait = CatPortID; }

										ImGui::Text("Rat Portrait Canvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##rpt", &RatPortID);
										if (RatPortID != m_currentSelectedObject) { it->second.RatPortrait = RatPortID; }

										ImGui::Text("Portrait ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##npt", &PortID);
										if (PortID != m_currentSelectedObject) { it->second.Portrait = PortID; }

										ImGui::Text("Level Background ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##bg", &BackgroundID);
										if (BackgroundID != m_currentSelectedObject) { it->second.Background = BackgroundID; }

										ImGui::Text("Transition Panel ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##tpc", &TransitionPanelID);
										if (TransitionPanelID != m_currentSelectedObject) { it->second.TransitionPanel = TransitionPanelID; }

										ImGui::Text("Journal ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##jcid", &JournalID);
										if (JournalID != m_currentSelectedObject) { it->second.Journal = JournalID; }

										ImGui::Text("Journal Button ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##jbid", &JournalButtonID);
										if (JournalButtonID != m_currentSelectedObject) { it->second.JournalButton = JournalButtonID; }

										ImGui::Text("Phase Banner ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##PBID", &PhaseBannerID);
										if (PhaseBannerID != m_currentSelectedObject) { it->second.PhaseBanner = PhaseBannerID; }

										for (int i = 0; i < 5; i++)
										{
											if (i != 0)
											{
												int id = static_cast<int> (it->second.clicklisttest[i]);
												std::string test = std::string("##id2") + std::to_string(i);
												ImGui::Text("Click Test ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt(test.c_str(), &id);
												if (id != m_currentSelectedObject)
													it->second.clicklisttest[i] = id;
											}
										}
									}
								}
							}

							if (key == "HealthBarScript_v2_0")
							{
								HealthBarScript_v2_0* p_Script = dynamic_cast<HealthBarScript_v2_0*>(val);
								auto it = p_Script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_Script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("HealthBarScript", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
											int id = static_cast<int> (it->second.followObjectID);
											ImGui::Text("Follow Object ID: "); ImGui::SameLine(); ImGui::InputInt("##healthbarfollowid", &id);
											it->second.followObjectID = id;

											//get and set color variable of the healthbar
											ImVec4 color;
											color.x = it->second.fillColorFull.x;
											color.y = it->second.fillColorFull.y;
											color.z = it->second.fillColorFull.z;
											color.w = it->second.fillColorFull.w;

											ImGui::Text("Full Health Color: "); ImGui::SameLine();
											ImGui::ColorEdit4("##fullhealthbarcolor", (float*)&color, ImGuiColorEditFlags_AlphaPreview);

											it->second.fillColorFull.x = color.x;
											it->second.fillColorFull.y = color.y;
											it->second.fillColorFull.z = color.z;
											it->second.fillColorFull.w = color.w;

											ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space


											//get and set color variable of the healthbar
											color.x = it->second.fillColorHalf.x;
											color.y = it->second.fillColorHalf.y;
											color.z = it->second.fillColorHalf.z;
											color.w = it->second.fillColorHalf.w;

											ImGui::Text("Half Health Color: "); ImGui::SameLine();
											ImGui::ColorEdit4("##halfhealthbarcolor", (float*)&color, ImGuiColorEditFlags_AlphaPreview);

											it->second.fillColorHalf.x = color.x;
											it->second.fillColorHalf.y = color.y;
											it->second.fillColorHalf.z = color.z;
											it->second.fillColorHalf.w = color.w;

											ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space


											//get and set color variable of the healthbar
											color.x = it->second.fillColorAlmostEmpty.x;
											color.y = it->second.fillColorAlmostEmpty.y;
											color.z = it->second.fillColorAlmostEmpty.z;
											color.w = it->second.fillColorAlmostEmpty.w;

											ImGui::Text("Low Health Color: "); ImGui::SameLine();
											ImGui::ColorEdit4("##lowhealthbarcolor", (float*)&color, ImGuiColorEditFlags_AlphaPreview);

											it->second.fillColorAlmostEmpty.x = color.x;
											it->second.fillColorAlmostEmpty.y = color.y;
											it->second.fillColorAlmostEmpty.z = color.z;
											it->second.fillColorAlmostEmpty.w = color.w;

											ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
									}
								}
							}

							if (key == "DeploymentScript")
							{
								DeploymentScript* p_script = dynamic_cast<DeploymentScript*>(val);
								auto it = p_script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("DeploymentScript", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										int FollowingTextureObjectID = static_cast<int> (it->second.FollowingTextureObject);
										int NoGoAreaID = static_cast<int> (it->second.NoGoArea);
										int DeploymentAreaID = static_cast<int> (it->second.DeploymentArea);

										ImGui::Text("Following Texture Object ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##FTO", &FollowingTextureObjectID);
										 it->second.FollowingTextureObject = FollowingTextureObjectID;

										ImGui::Text("No Go Areas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##NGA", &NoGoAreaID);
										if (NoGoAreaID != m_currentSelectedObject) { it->second.NoGoArea = NoGoAreaID; }


										ImGui::Text("Deployment Zone ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##DPZ", &DeploymentAreaID);
										{ it->second.DeploymentArea = DeploymentAreaID; }
									}
								}
							}

							if (key == "IntroCutsceneController")
							{
								IntroCutsceneController* p_script = dynamic_cast<IntroCutsceneController*>(val);
								auto it = p_script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("IntroCutsceneController", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										int CutsceneObjectID = static_cast<int> (it->second.CutsceneObject);
										int FinalSceneID = static_cast<int> (it->second.FinalScene);
										int TextID = static_cast<int> (it->second.Text);
										int TransitionScreenID = static_cast<int> (it->second.TransitionScreen);

										ImGui::Text("CutScene Object ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##csoi", &CutsceneObjectID);
										it->second.CutsceneObject = CutsceneObjectID;

										ImGui::Text("Final Frame Object ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##fsidd", &FinalSceneID);
										{ it->second.FinalScene = FinalSceneID; }

										ImGui::Text("Text Object ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##tttid", &TextID);
										{ it->second.Text = TextID; }

										ImGui::Text("Transition Screen ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##tssid", &TransitionScreenID);
										{ it->second.TransitionScreen = TransitionScreenID; }
									}
								}
							}

							if (key == "MainMenuController")
							{
								MainMenuController* p_script = dynamic_cast<MainMenuController*>(val);
								auto it = p_script->GetScriptData().find(m_currentSelectedObject);
								if (it != p_script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("MainMenuController", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										int AreYouSureID = static_cast<int> (it->second.AreYouSureCanvas);
										int MainMenuCanvasID = static_cast<int> (it->second.MainMenuCanvas);
										int SplashScreenID = static_cast<int> (it->second.SplashScreen);

										ImGui::Text("Main Menu Canvas ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##MMCID", &MainMenuCanvasID);
										if (MainMenuCanvasID != m_currentSelectedObject) { it->second.MainMenuCanvas = MainMenuCanvasID; }

										ImGui::Text("Splash Screen ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##SSMM", &SplashScreenID);
										if (SplashScreenID != m_currentSelectedObject) { it->second.SplashScreen = SplashScreenID; }

										ImGui::Text("Are You Sure Canvas Object ID: "); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f); ImGui::InputInt("##AYSMMID", &AreYouSureID);
										if (AreYouSureID != m_currentSelectedObject) it->second.AreYouSureCanvas = AreYouSureID;
									}
								}
							}

							if (key == "RatScript_v2_0")
							{
								RatScript_v2_0* p_Script = dynamic_cast<RatScript_v2_0*>(val);
								auto it = p_Script->GetScriptData().find(m_currentSelectedObject);

								if (it != p_Script->GetScriptData().end())
								{
									if (ImGui::CollapsingHeader("Rat Settings", ImGuiTreeNodeFlags_DefaultOpen))
									{
										ImGui::Checkbox("Should Patrol", &it->second.shouldPatrol);
									}

									if (ImGui::CollapsingHeader("Rat Patrol Points", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										if (it->second.patrolPoints.empty())
										{
											it->second.patrolPoints.push_back(PE::vec2(0.0f, 0.0f));		// Default Point 1
											it->second.patrolPoints.push_back(PE::vec2(100.0f, 100.0f));	// Default Point 2
										}

										for (size_t i = 0; i < it->second.patrolPoints.size(); ++i)
										{
											ImGui::PushID(static_cast<int>(i)); // Use i as the ID
											ImGui::Text("Point %zu:", i + 1); // Display point number
											ImGui::SameLine();
											float pos[2] = { it->second.patrolPoints[i].x, it->second.patrolPoints[i].y };
											ImGui::InputFloat2("##PatrolPoint", pos); // Input field for editing points
											it->second.patrolPoints[i] = PE::vec2(pos[0], pos[1]); // Update patrol point with new values
											ImGui::PopID();
										}

										ImGui::Dummy(ImVec2(0.0f, 5.0f));

										ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.7f, 0.0f, 1.0f)); // Green color
										if (ImGui::Button("Add Patrol Point"))
										{
											it->second.patrolPoints.push_back(PE::vec2(0.0f, 0.0f));
										}
										ImGui::PopStyleColor(1); // Pop button color style

										ImGui::Dummy(ImVec2(0.0f, 5.0f));

										ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 1.0f)); // Red color
										if (ImGui::Button("Delete Last Patrol Point") && it->second.patrolPoints.size() > 2)
										{
											it->second.patrolPoints.pop_back();
										}
										ImGui::PopStyleColor(1); // Pop button color style
									}
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
							if (key == "CatScript_v2_0")
							{
								CatScript_v2_0* p_script = dynamic_cast<CatScript_v2_0*>(val);
								auto it = p_script->GetScriptData().find(m_currentSelectedObject);

								if (it != p_script->GetScriptData().end())
								{
									//std::cout << "CatData_______________________________" << std::endl;
									if (ImGui::CollapsingHeader("CatScript_v2_0", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
									{
										std::vector<std::string> types{"Main Cat", "Grey Cat", "Orange Cat", "Fluffy Cat"};
										std::vector < std::variant <GreyCatAttackVariables, OrangeCatAttackVariables>> variants{ GreyCatAttackVariables(), OrangeCatAttackVariables() };

										rttr::type type = rttr::type::get_by_name("CatScript_v2_0");
										rttr::instance inst = rttr::instance(it->second);
										for (auto props : type.get_properties())
										{
											if (props.get_type().get_name() == "float")
											{
												float value = props.get_value(inst).get_value<float>();
												ImGui::Text(props.get_name().to_string().c_str()); 
												ImGui::SameLine();
												ImGui::SetNextItemWidth(100.0f);
												ImGui::InputFloat(("##" + props.get_name().to_string()).c_str(), &value);
												props.set_value(inst, value);
											}
											else if (props.get_type().get_name() == "int")
											{
												int value = props.get_value(inst).get_value<int>();
												ImGui::Text(props.get_name().to_string().c_str());
												ImGui::SameLine();
												ImGui::SetNextItemWidth(100.0f);
												ImGui::InputInt(("##" + props.get_name().to_string()).c_str(), &value);
												props.set_value(inst, value);
											}
											else if (props.get_type().get_name() == "bool" && props.get_name() == "isCaged")
											{
												bool value = props.get_value(inst).get_value<bool>();
												ImGui::Text(props.get_name().to_string().c_str());
												ImGui::SameLine();
												ImGui::Checkbox(("##" + props.get_name().to_string()).c_str(), &value);
												props.set_value(inst, value);
											}
											else if (props.get_type().get_name() == "unsigned__int64")
											{
												EntityID value = props.get_value(inst).get_value<EntityID>();
												ImGui::Text(props.get_name().to_string().c_str());
												ImGui::SameLine();
												ImGui::SetNextItemWidth(100.0f);
												int tmp = static_cast<int>(value);
												ImGui::InputInt(("##" + props.get_name().to_string()).c_str(), &tmp);
												value = static_cast<EntityID>(tmp);
												props.set_value(inst, value);
											}
											else if (props.get_type().get_name() == "enumPE::EnumCatType")
											{
												int value = static_cast<int>(it->second.catType);
												ImGui::Text(props.get_name().to_string().c_str());
												ImGui::SameLine();
												ImGui::SetNextItemWidth(100.0f);
												if (ImGui::BeginCombo(("##" + types[value]).c_str(), types[value].c_str()))
												{
													int cnt{ 0 };
													bool selected{ false };
													for (const auto& str : types)
													{
														if (ImGui::Selectable(str.c_str(), &selected))
														{
																													
															it->second.catType = EnumCatType(cnt);
															// temp logic since only 2 variants are programmed in
															int tmp = (cnt <= 1) ? 0 : 1;
															if (tmp != it->second.attackVariables.index())
															{
																it->second.attackVariables = variants.at(tmp);
																break;
															}
														}
														++cnt;
													}
													ImGui::EndCombo();
												}
												props.set_value(inst, value);
											}
											else if (props.get_type().get_name() == "unsignedint")
											{
												int value = props.get_value(inst).get_value<int>();
												ImGui::Text(props.get_name().to_string().c_str());
												ImGui::SameLine();
												ImGui::SetNextItemWidth(100.0f);
												ImGui::InputInt(("##" + props.get_name().to_string()).c_str(), &value);
												props.set_value(inst, value);
											}
											else if (props.get_type().get_name() == "classstd::map<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>,classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>,structstd::less<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>>,classstd::allocator<structstd::pair<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>const,classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>>> >")
											{
												
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
											else if (props.get_type().get_name() == "classstd::variant<structPE::GreyCatAttackVariables,structPE::OrangeCatAttackVariables>")
											{
												// display option to swap the types
												ImGui::Separator();
												if (!it->second.attackVariables.index())
												{ 
													GreyCatAttackVariables& var = std::get<GreyCatAttackVariables>(it->second.attackVariables);
													ImGui::Text("Damage:"); ImGui::SameLine(); ImGui::InputInt("##GCAdamage", &(var.damage));
													ImGui::Text("Bullet Delay:"); ImGui::SameLine(); ImGui::InputFloat("##GCAdelay", &(var.bulletDelay));
													ImGui::Text("Bullet Range:"); ImGui::SameLine(); ImGui::InputFloat("##GCArange", &(var.bulletRange));
													ImGui::Text("Bullet lifetime:"); ImGui::SameLine(); ImGui::InputFloat("##GCAlife", &(var.bulletLifeTime));
													ImGui::Text("Bullet force:"); ImGui::SameLine(); ImGui::InputFloat("##GCAforce", &(var.bulletForce));
													ImGui::Text("Bullet Anim Index:"); ImGui::SameLine(); ImGui::InputInt("##GCAindex", reinterpret_cast<int*>(&(var.bulletFireAnimationIndex)));

												}
												else
												{
													OrangeCatAttackVariables& var = std::get<OrangeCatAttackVariables>(it->second.attackVariables);
													ImGui::Text("Damage:"); ImGui::SameLine(); ImGui::InputInt("##OCAdamage", &(var.damage));
													ImGui::Text("Stomp Radius:"); ImGui::SameLine(); ImGui::InputFloat("##GCAdelay", &(var.stompRadius));
													ImGui::Text("Stomp Lifetime:"); ImGui::SameLine(); ImGui::InputFloat("##GCAdelay", &(var.stompLifeTime));
													ImGui::Text("Stomp Force:"); ImGui::SameLine(); ImGui::InputFloat("##GCAdelay", &(var.stomopForce));

												}
											}
										
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
								&& !EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>())
							&& !EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<AudioComponent>()))
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
							if (ImGui::Selectable("Add Canvas"))
							{
								if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Canvas>()))
									EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<Canvas>() });
								else
									AddErrorLog("ALREADY HAS CANVAS");
							}
						}
						if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<Graphics::Renderer>())
							&& !EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<AudioComponent>()))
						{
							if (ImGui::Selectable("Add Text"))
							{
								if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<TextComponent>()))
									EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<TextComponent>() });
								else
									AddErrorLog("ALREADY HAS TEXT");
							}
							if (ImGui::Selectable("Add ScriptComponent"))
							{
									if (!EntityManager::GetInstance().Has(entityID, EntityManager::GetInstance().GetComponentID<ScriptComponent>()))
											EntityFactory::GetInstance().Assign(entityID, { EntityManager::GetInstance().GetComponentID<ScriptComponent>() });
									else
											AddErrorLog("ALREADY HAS A SCRIPTCOMPONENT");
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
		// Dynamic Scaling Variables
		const ImVec2 childWindowSize = ImVec2(150, 130);
		const int baseMaxCharCount = 15;
		const float childWindowPadding = 5.0f;
		static float scaleFactor = 1.0f;
		const float minScaleFactor = 0.9f;		// Minimum scale factor
		const float maxScaleFactor = 1.2f;		// Maximum scale factor

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

				// Check for Ctrl key and mouse wheel usage
				ImGuiIO& io = ImGui::GetIO();
				if (io.KeyCtrl && io.MouseWheel != 0)
				{
					scaleFactor += io.MouseWheel * 0.05f;
					scaleFactor = std::max(minScaleFactor, std::min(scaleFactor, maxScaleFactor));
				}

				// Adjust the maximum character count and the size variables based on the scale factor
				int scaledMaxCharCount = static_cast<int>(baseMaxCharCount * scaleFactor);
				ImVec2 scaledChildWindowSize = ImVec2(140 * scaleFactor, 130 * scaleFactor);
				float scaledMaxImageSize = 70.0f * scaleFactor;
				float scaledIconSize = 50.0f * scaleFactor;

				// Calculate the total width including dynamic padding
				float dynamicPadding = childWindowPadding * scaleFactor;
				float totalChildWidth = scaledChildWindowSize.x + dynamicPadding;

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

					// Calculate the space needed for the refresh button
					ImGuiStyle& style = ImGui::GetStyle();
					float refreshButtonWidth = ImGui::CalcTextSize("Refresh").x + style.FramePadding.x * 2.0f;
					float searchBarWidth = 600.f;

					// Calculate the position to right-align the search bar and refresh button
					float availableWidth = ImGui::GetContentRegionAvail().x - searchBarWidth - refreshButtonWidth;

					// Align the search bar to the right
					ImGui::SameLine(availableWidth);
					ImGui::SetNextItemWidth(searchBarWidth);
					static char searchQuery[256] = "";
					ImGui::InputTextWithHint("##Search", "Type to Search...", searchQuery, IM_ARRAYSIZE(searchQuery));

					// Align the Refresh button next to the search bar
					ImGui::SameLine();
					if (ImGui::Button("Refresh"))
					{
						GetFileNamesInParentPath(m_parentPath, m_files);
					}

					ImGui::Separator();

					ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)2), ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

				if (ImGui::IsWindowHovered())
				{
					glfwSetDropCallback(WindowManager::GetInstance().GetWindow(), &HotLoadingNewFiles);
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

					// Calculate number of items per row with dynamic padding
					int numItemPerRow = static_cast<int>(ImGui::GetContentRegionAvail().x / totalChildWidth);
					if (numItemPerRow < 1) numItemPerRow = 1;


					// list the files in the current showing directory as imgui text
					for (int n = 0; n < m_files.size(); n++) // loop through resource list here
					{	//resource list needs a list of icons for the texture for the image if possible
						//else just give a standard object icon here

						std::string filename = m_files[n].filename().string();

						// Filter files based on search query
						if (searchQuery[0] == '\0' || CaseInsensitiveFind(filename, searchQuery))
						{

							if (n % numItemPerRow != 0)
								ImGui::SameLine();
							else
								ImGui::NewLine();

							if (ImGui::BeginChild(m_files[n].filename().string().c_str(), scaledChildWindowSize, false)) //child to hold image n text
							{
								std::string icon{};
								std::string const extension{ m_files[n].filename().extension().string() };
								GLuint textureID = 0;
								ImVec2 image_size = ImVec2(scaledIconSize, scaledIconSize);
								// fixed height for the image area
								const float imageAreaHeight = 80.0f;

								if (extension == ".png")
								{
									textureID = ResourceManager::GetInstance().GetIcon(m_files[n].string())->GetTextureID();
									ImVec2 textureSize = ResourceManager::GetInstance().GetTextureSize(m_files[n].string());

									// Calculate the aspect ratio
									float aspectRatio = textureSize.x / textureSize.y;
									if (aspectRatio > 2.0f)
									{
										image_size.x = scaledMaxImageSize;
										image_size.y = scaledMaxImageSize / aspectRatio;
									}
									else
									{
										image_size.x = scaledMaxImageSize * aspectRatio;
										image_size.y = scaledMaxImageSize;
									}

									ImVec4 darkBg = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
									ImVec4 darkerBorder = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

									// Calculate vertical position to center the image within the image area
									float spaceAboveImage = (imageAreaHeight - image_size.y) * 0.5f;

									// Centering the background and border
									ImVec2 borderPadding(2, 2);
									ImVec2 totalBorderSize = ImVec2(image_size.x + 2 * borderPadding.x, image_size.y + 2 * borderPadding.y);
									float borderPosX = (childWindowSize.x - totalBorderSize.x) * 0.5f;

									// Use the same spaceAboveImage for aligning the top of the border
									ImVec2 borderPos = ImGui::GetCursorScreenPos();
									borderPos.x += borderPosX;
									borderPos.y += spaceAboveImage;

									// Draw background and border
									ImDrawList* draw_list = ImGui::GetWindowDrawList();
									ImVec2 p_min = ImVec2(borderPos.x, borderPos.y);
									ImVec2 p_max = ImVec2(p_min.x + totalBorderSize.x, p_min.y + totalBorderSize.y);

									draw_list->AddRectFilled(p_min, p_max, ImGui::ColorConvertFloat4ToU32(darkBg));
									draw_list->AddRect(p_min, p_max, ImGui::ColorConvertFloat4ToU32(darkerBorder), 0.0f, ImDrawFlags_RoundCornersAll, 2.0f);
								}
								else
								{
									if (extension == "")
										icon = "../Assets/Icons/Directory_Icon.png";
									else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg")
										icon = "../Assets/Icons/Audio_Icon.png";
									else if (extension == ".ttf")
										icon = "../Assets/Icons/Font_Icon.png";
									else if (extension == ".json")
										icon = "../Assets/Icons/Prefabs_Icon.png";
									else
										icon = "../Assets/Icons/Other_Icon.png";

									textureID = ResourceManager::GetInstance().GetIcon(icon)->GetTextureID();
								}

								// Calculate vertical position to center the image within the image area
								float spaceAboveImage = (imageAreaHeight - image_size.y) * 0.5f;
								ImGui::Dummy(ImVec2(0.0f, spaceAboveImage));

								float iconPosX = (childWindowSize.x - image_size.x) * 0.5f;
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconPosX);

								// Display the Icon or actual texture
								ImGui::Image((void*)(intptr_t)textureID, image_size, ImVec2(0, 1), ImVec2(1, 0));

								float spaceBelowImage = imageAreaHeight - spaceAboveImage - image_size.y;
								ImGui::Dummy(ImVec2(0.0f, spaceBelowImage));

								// Truncation of Text based on scaled character count
								filename = m_files[n].filename().string();
								std::string displayText = filename.length() > scaledMaxCharCount
									? filename.substr(0, scaledMaxCharCount - 3) + "..."
									: filename;

								// Centering and Displaying the Text
								float textWidth = ImGui::CalcTextSize(displayText.c_str()).x;
								float centerTextPosX = (childWindowSize.x - textWidth) * 0.5f;
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + centerTextPosX);
								ImGui::Text("%s", displayText.c_str());
							}
							ImGui::EndChild();

							// check if the mouse is hovering the asset
							if (ImGui::IsItemHovered())
							{
								// Hover over asset browser icons tooltip
								ImGui::BeginTooltip();
								ImGui::TextUnformatted(m_files[n].filename().string().c_str());
								ImGui::EndTooltip();

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
									prefabFP = (m_isPrefabMode) ? prefabFP : m_files[n].string();

									if (!m_isPrefabMode)
									{
										m_isPrefabMode = true;
										engine_logger.AddLog(false, "Attempting to save all entities to file...", __FUNCTION__);
										SaveAndPlayScene();
										engine_logger.AddLog(false, "Entities saved successfully to file.", __FUNCTION__);

									}
									else if (EntityManager::GetInstance().Has<EntityDescriptor>(1))
									{
										nlohmann::json save = serializationManager.SerializeEntityPrefab(1);
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
									
									serializationManager.LoadFromFile(prefabFP, true);
									
									
								}
								if (ImGui::Selectable("Delete Asset"))
								{
									std::filesystem::remove(m_files[n]);
									GetFileNamesInParentPath(m_parentPath, m_files);
								}
								ImGui::EndPopup();
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
						ImGui::Image((void*)(intptr_t)ResourceManager::GetInstance().GetIcon(iconDragged)->GetTextureID(), ImVec2(34, 34), { 0,1 }, { 1,0 });
						ImGui::End();

						// Check if the mouse button is released
						if (ImGui::IsMouseReleased(0))
						{
							if (m_entityToModify.second != -1)
							{
								// alters the texture assigned to renderer component in entity
								std::string const extension = m_files[draggedItemIndex].extension().string();
								if (extension == ".png" && !EntityManager::GetInstance().Has<AudioComponent>(m_currentSelectedObject))
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
								else if (EntityManager::GetInstance().Has<AudioComponent>(m_currentSelectedObject))
								{
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
							}

							if (m_animationToModify.second != "")
							{
								if (m_animationToModify.first == "Animation")
								{
									ResourceManager::GetInstance().LoadTextureFromFile(m_files[draggedItemIndex].string(), m_files[draggedItemIndex].string());
									ResourceManager::GetInstance().GetAnimation(m_animationToModify.second)->SetSpriteSheetKey(m_files[draggedItemIndex].string());
								}
							}

							if (m_mouseInScene || m_mouseInObjectWindow)
							{
								if (m_files[draggedItemIndex].extension() == ".json")
								{
									EntityID s_id = serializationManager.LoadFromFile(m_files[draggedItemIndex].string(), true);
									UndoStack::GetInstance().AddChange(new CreateObjectUndo(s_id));
									// change position of loaded prefab based on mouse cursor here
								}
							}

							isDragging = false;
							draggedItemIndex = -1;
						}
					}
					m_entityToModify = std::make_pair<std::string>("", -1);
					m_animationToModify = std::make_pair<std::string>("", "");
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


			ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space

			ImGui::Text("Total Draw Calls: "); ImGui::SameLine(); ImGui::Text(std::to_string(Graphics::RendererManager::totalDrawCalls).c_str());
			ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
			ImGui::Text("Object Draw Calls: "); ImGui::SameLine(); ImGui::Text(std::to_string(Graphics::RendererManager::objectDrawCalls).c_str());
			ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
			ImGui::Text("Text Draw Calls: "); ImGui::SameLine(); ImGui::Text(std::to_string(Graphics::RendererManager::textDrawCalls).c_str());
			ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space
			ImGui::Text("Debug Shape Draw Calls: "); ImGui::SameLine(); ImGui::Text(std::to_string(Graphics::RendererManager::debugDrawCalls).c_str());
			ImGui::Dummy(ImVec2(0.0f, 5.0f));//add space


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
			std::vector<const char*> animationList;

			static std::string currentAnimationID;

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
			/*for (const auto& layer : LayerView<AnimationComponent>())
			{
				for (EntityID id : InternalView(layer))
				{
					entities[EntityManager::GetInstance().Get<EntityDescriptor>(id).name.c_str()] = id;
					entityList.push_back(EntityManager::GetInstance().Get<EntityDescriptor>(id).name.c_str());
				}
			}*/

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

			ImGui::SetNextItemWidth(300.f);

			// show current selected entity if there it has a animation component


			if (m_currentSelectedObject != -1)
			{
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

						// Add animation component to entity
						if (!EntityManager::GetInstance().Has(m_currentSelectedObject, EntityManager::GetInstance().GetComponentID<AnimationComponent>()))
							EntityFactory::GetInstance().Assign(m_currentSelectedObject, { EntityManager::GetInstance().GetComponentID<AnimationComponent>() });
					}
					else
					{
						std::cerr << "No file path was selected for saving." << std::endl;
					}
				}

				// if current selected object has animation component, show rest of animation editor
				if (EntityManager::GetInstance().Has<AnimationComponent>(m_currentSelectedObject))
				{
					if (currentAnimationID != "")
					{
						currentAnimation = ResourceManager::GetInstance().GetAnimation(currentAnimationID);
					}

					// Save animation
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

					// Load animation
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

					if (currentAnimation)
					{
						// Add animation to entity
						if (ImGui::Button("Add Animation to object"))
						{
							EntityManager::GetInstance().Get<AnimationComponent>(m_currentSelectedObject).AddAnimationToComponent(currentAnimationID);
						}

						ImGui::SameLine();
						if (ImGui::Button("Remove Animation from object"))
						{
							EntityManager::GetInstance().Get<AnimationComponent>(m_currentSelectedObject).RemoveAnimationFromComponent(currentAnimationID);
						}
					}

					ImGui::Dummy(ImVec2(0, 5));
					ImGui::SeparatorText("Sprite Sheet");

					// if there's animation to preview
					if (currentAnimation)
					{
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
					}
					ImGui::EndChild();
					

					// checks if mouse if hovering the texture preview - to use for asset browser drag n drop
					if (ImGui::IsItemHovered())
					{
						m_animationToModify = std::make_pair<std::string, std::string const&>("Animation", currentAnimationID);
					}

					ImGui::SetNextItemWidth(300.f);
					if (ImGui::Combo("Spritesheet", &textureIndex, textureList.data(), static_cast<int>(textureList.size())))
					{
						if (currentAnimation)
							currentAnimation->SetSpriteSheetKey(filepaths[textureIndex].string());
					}

					// deprecated
					//// Load texture through file explorer
					//if (ImGui::Button("Load Spritesheet"))
					//{
					//	std::string filePath = serializationManager.OpenFileExplorerRequestPath();

					//	// Check if filePath is not empty
					//	if (!filePath.empty())
					//	{
					//		std::replace(filePath.begin(), filePath.end(), '\\', '/');
					//		filePath = ".." + filePath.substr(filePath.find("/Assets/"), filePath.find(".") - filePath.find("/Assets/")) + ".png";

					//		ResourceManager::GetInstance().LoadTextureFromFile(filePath, filePath);
					//	}
					//	else
					//	{
					//		std::cerr << "No file path was selected for loading." << std::endl;
					//	}
					//}


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
					int frame{ static_cast<int>(previewCurrentFrameIndex) };
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
						if (currentAnimation)
							currentAnimation->CreateAnimationFrames(totalSprites);

						// update frame duration
						if (currentAnimation)
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
						if(currentAnimation)
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

				}
				else
				{
					// if no animation component, show message
					ImGui::Text("To begin animating Entity, create an animation");
				}
			}
			else
			{
				ImGui::Text("No entity selected");
				
			}

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
						SaveAndPlayScene();
						engine_logger.AddLog(false, "Entities saved successfully to file.", __FUNCTION__);
					}
					ImGui::SameLine();
					ImGui::BeginDisabled();
					if (ImGui::Button("Stop")) {
						m_showEditor = true;

						if (m_isRunTime)
						{
							StopAndLoadScene();

							engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
						}

						if (m_showEditor)
							m_isRunTime = false;

						m_showGameView = false;
						GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)-> SetCurrentLevel(0);
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
						ImGui::OpenPopup("ReqSave?");
						m_applyPrefab = false;
					}

					ImGui::SameLine();
					if (ImGui::Button(" Save "))
					{
						if (EntityManager::GetInstance().Has<EntityDescriptor>(1))
						{
							nlohmann::json save = serializationManager.SerializeEntityPrefab(1);
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
								nlohmann::json save = serializationManager.SerializeEntityPrefab(1);
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
							StopAndLoadScene();

							engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
							m_applyPrefab = true;
						}
						//ImGui::SameLine();
						ImGui::Separator();
						if (ImGui::Selectable("No"))
						{
							m_isPrefabMode = false;
							StopAndLoadScene();

							engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);
						}
						ImGui::EndPopup();
						UndoStack::GetInstance().ClearStack();
					}
				}
				ImGui::Dummy(ImVec2(0, .2f));

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
					static bool ToSavePopup{};
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
										nlohmann::json save = serializationManager.SerializeEntityPrefab(1);

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
								if (ImGui::MenuItem("Create")) // the ctrl s is not programmed yet, need add to the key press event
								{
									engine_logger.AddLog(false, "Create empty scene...", __FUNCTION__);
									// This will create a default scene
									ToSavePopup = true;
								}
								if (ImGui::MenuItem("Save", "CTRL+S")) // the ctrl s is not programmed yet, need add to the key press event
								{
									engine_logger.AddLog(false, "Attempting to save all entities to file...", __FUNCTION__);
									// This will save all entities to a file

									serializationManager.SaveAllEntitiesToFile(serializationManager.OpenFileExplorerRequestPath(), true);
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
										// clear undo stack
										UndoStack::GetInstance().ClearStack();

										// deselect object
										m_currentSelectedObject = -1;

										// load scene from filepath
										SceneManager::GetInstance().LoadSceneToLoad(filePath.substr(filePath.find_last_of('\\') + 1));
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
							if (ImGui::MenuItem("Dark", ""))
							{
								m_currentStyle = GuiStyle::DARK;
								SetImGUIStyle_Dark();
							}
							if (ImGui::MenuItem("Light", ""))
							{
								m_currentStyle = GuiStyle::LIGHT;
								SetImGUIStyle_Light();
							}
							if (ImGui::MenuItem("Kurumi", ""))
							{
								m_currentStyle = GuiStyle::KURUMI;
								SetImGUIStyle_Kurumi();
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
						if (ImGui::BeginMenu("Settings"))
						{
							if (ImGui::MenuItem("Layer Settings", ""))
							{
								m_showLayerWindow = true;
							}

							ImGui::EndMenu();
						}
					}
					ImGui::EndMainMenuBar(); // closing of menu begin function

					if (ToSavePopup)
					{
						float size = ImGui::CalcTextSize("Do You Want To Save Your Current Scene?").x + style.FramePadding.x * 2.0f;
						m_currentSelectedObject = -1;
						ImGui::SetNextWindowSize(ImVec2(size, 70));
						ImGui::OpenPopup("To Save");
					}

					if (ImGui::BeginPopupModal("To Save",nullptr,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
					{
						ImGui::Text("Do You Want To Save Your Current Scene?");

						ImGui::Dummy(ImVec2(0,7));

						float size = ImGui::CalcTextSize("Yes").x + style.FramePadding.x * 2.0f;
						float avail = ImGui::GetContentRegionAvail().x;

						float off = (float)((avail - size) * 0.5);
						if (off > 0.0f)
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off - (ImGui::CalcTextSize("Play").x + style.FramePadding.x) / 2);

						if (ImGui::Button("Yes"))
						{
							UndoStack::GetInstance().ClearStack();

							// if active scene is default scene, open file explorer to save new scene
							if (SceneManager::GetInstance().GetActiveScene() == "DefaultScene.json")
							{
								serializationManager.SaveAllEntitiesToFile(serializationManager.OpenFileExplorerRequestPath(), true);
							}
							else
							{
								// save active scene
								serializationManager.SaveAllEntitiesToFile(SceneManager::GetInstance().GetActiveScene());
							}

							SceneManager::GetInstance().CreateDefaultScene();
							engine_logger.AddLog(false, "Default scene created", __FUNCTION__);
							ToSavePopup = false;

							ImGui::ClosePopupToLevel(0, true);
						}

						ImGui::SameLine();

						if(ImGui::Button("No"))
						{
							UndoStack::GetInstance().ClearStack();

							SceneManager::GetInstance().CreateDefaultScene();
							engine_logger.AddLog(false, "Default scene created", __FUNCTION__);

							ToSavePopup = false;

							ImGui::ClosePopupToLevel(0, true);
						}
						ImGui::EndPopup();
					}
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
		if (IsEditorActive()) 
		{
			static bool hasParent;
			static bool moving;
			ImGui::Begin("Scene View", p_active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

			//ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2.f, ImGui::GetCursorPosY()));
			if (ImGui::BeginChild("SceneViewChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar)) 
			{
				m_renderWindowWidth = ImGui::GetContentRegionAvail().x;
				m_renderWindowHeight = ImGui::GetContentRegionAvail().y;

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
				m_sceneViewFocused = ImGui::IsWindowFocused();

				ImGuiStyle& style = ImGui::GetStyle();
				ImVec2 padding{ style.WindowPadding };

				if (m_currentSelectedObject != -1)
				{

					//by default is false though
					ImGuizmo::SetOrthographic(true);

					//call before manipulate
					ImGuizmo::SetDrawlist();

					//basically setting the viewport if the position is off need to change or add/remove offset
					ImGuizmo::SetRect(ImGui::GetWindowPos().x + padding.x, ImGui::GetWindowPos().y + padding.y, m_renderWindowWidth, m_renderWindowHeight);

					auto EditorCamera = GETCAMERAMANAGER()->GetEditorCamera();
					glm::mat4 cameraProjection = EditorCamera.GetViewToNdcMatrix();

					// if the selected obj is UI, make the camera view matrix an identity matrix
					glm::mat4 cameraView = EditorCamera.GetWorldToViewMatrix();

					auto& ct = EntityManager::GetInstance().Get<Transform>(m_currentSelectedObject);				

					EntityID parentId{};
					if (EntityManager::GetInstance().Has<EntityDescriptor>(m_currentSelectedObject))
					{
						auto optionalParent{ EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).parent };
						hasParent = optionalParent.has_value();
						if (hasParent) { parentId = optionalParent.value(); }
					}

					float transform[16]{};
					//float transform2[16]{};
					static float OldTransform[16]{};
					static float OldLocalX,OldLocalY;
					static Transform currentTransform{};

					if (!hasParent)
					{
						float Scale[3]{ ct.width,ct.height,0 },
							Rotation[3]{ 0,0, glm::degrees(ct.orientation) },
							Translation[3]{ ct.position.x,ct.position.y };

						ImGuizmo::RecomposeMatrixFromComponents(Translation, Rotation, Scale, transform);
					}
					else
					{
						float Scale[3]{ ct.width,ct.height,0 },
							Rotation[3]{ 0,0, glm::degrees(ct.relOrientation) },
							Translation[3]{ ct.position.x,ct.position.y };

						OldLocalX = ct.relPosition.x;
						OldLocalY = ct.relPosition.y;

						ImGuizmo::RecomposeMatrixFromComponents(Translation, Rotation, Scale, transform);

						//float Scale2[3]{ ct.width,ct.height,0 },
						//	Rotation2[3]{ 0,0, glm::degrees(ct.orientation) },
						//	Translation2[3]{ ct.position.x,ct.position.y };

						//ImGuizmo::RecomposeMatrixFromComponents(Translation2, Rotation2, Scale2, transform2);
					}
					//for rendering the gizmo
					if (!hasParent)
					{
						ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), m_currentGizmoOperation, ImGuizmo::WORLD, transform);
					}
					else
					{
						ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), m_currentGizmoOperation, ImGuizmo::LOCAL, transform);
						
						//this works, but we will end up drawing 2 gizmo if uncommented, if the first 1 is commented the gizmo will not work.
						//ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), m_currentGizmoOperation, ImGuizmo::LOCAL, transform2);

					}
					if (ImGuizmo::IsUsing())
					{
						if (m_mouseInScene && ImGui::IsMouseDown(0) && !moving)
						{
							moving = true;
							for (int i = 0; i < 16; ++i) {
								OldTransform[i] = transform[i];
							}
							currentTransform = ct;
						}

						float newScale[3];
						float newRot[3];
						float newPos[3];
						ImGuizmo::DecomposeMatrixToComponents(transform, newPos, newRot, newScale);

						ct.width = newScale[0];
						ct.height = newScale[1];

						// Update the translation only if we're not updating the rotation
						if (ImGuizmo::OPERATION::TRANSLATE == m_currentGizmoOperation
								|| ImGuizmo::OPERATION::TRANSLATE_X == m_currentGizmoOperation
								|| ImGuizmo::OPERATION::TRANSLATE_Y == m_currentGizmoOperation)
						{
							if (hasParent)
							{
								ct.relPosition.x = OldLocalX + newPos[0] - ct.position.x;
								ct.relPosition.y = OldLocalY +  newPos[1] - ct.position.y;
							}
							else
							{
								ct.position.x = newPos[0];
								ct.position.y = newPos[1];
							}
						}

						// Update relative transform values
						if (hasParent)
						{
							ct.relOrientation = glm::radians(newRot[2]);
						}
						else
						{
							ct.orientation = glm::radians(newRot[2]);
						}
					}
					
					if (moving && !ImGui::IsMouseDown(0))
					{
						if (!CompareFloat16Arrays(OldTransform, transform))
						{
							UndoStack::GetInstance().AddChange(new GuizmoUndo(currentTransform, ct, &ct, hasParent));
						}
						moving = false;
					}

}
			}



			// ----- SCREEN PICKING ----- //

			static ImVec2 clickedPosition;
			static ImVec2 screenPosition; // coordinates from the top left corner

			if (m_mouseInScene && !ImGuizmo::IsUsing())
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
						transformedCursor = Graphics::CameraManager::GetEditorCamera().GetViewToWorldMatrix() // screen to world position
								* transformedCursor;

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

				}

				std::cout << "\n";
			}
			// End of if (m_mouseInScene && !ImGuizmo::IsUsing())

			


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
				StopAndLoadScene();

				engine_logger.AddLog(false, "Entities loaded successfully from file.", __FUNCTION__);

				GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->SetCurrentLevel(0);
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
			for (const auto& layer : LayerView<>())
			{
				for (const auto& id : InternalView(layer))
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
			}
			if (!cnt)
				*p_active = false;
			ImGui::Separator();

			if (ImGui::Button("Apply"))
			{
				// exectue the changes!!
				EntityID pfid = serializationManager.LoadFromFile(prefabFP, true);
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

	void Editor::ShowLayerWindow(bool* p_active)
	{
		
		if (!ImGui::Begin("Layer Settings", p_active, ImGuiWindowFlags_NoCollapse)) // draw resource list
		{
			*p_active = false;
			ImGui::End(); //imgui close
		}
		else
		{
			if (IsEditorActive())
			{
				for (size_t i{}; i < LayerManager::GetInstance().GetLayerState().size(); ++i)
				{
					bool flag = LayerManager::GetInstance().GetLayerState(i);
					std::string txt = "Layer " + std::to_string(i) + ":  ";
					ImGui::Text(txt.c_str()); ImGui::SameLine(); ImGui::Checkbox(("##" + txt).c_str(), &flag);
					LayerManager::GetInstance().SetLayerState(i, flag);
				}
			}
			else
			{
				p_active = false;
			}
			ImGui::End(); //imgui close
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

	void Editor::ResetSelectedObject()
	{
		m_currentSelectedObject = -1;
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

	EntityID Editor::CountCanvas()
	{
		EntityID FirstCanvasID{};
		for (auto layer : LayerView<Canvas>())
		{
			for (EntityID objectID : InternalView(layer))
			{
				FirstCanvasID = objectID;
			}
			
		}
		return FirstCanvasID;
	}

	EntityID Editor::CheckCanvas()
	{
		EntityID NextCanvasID{};
		NextCanvasID = CountCanvas();
		if (NextCanvasID)
		{
			//if more than 1 canvas popup choose which canvas, to be done in the future
		}
		else
		{
			NextCanvasID = serializationManager.LoadFromFile("EditorDefaults/Canvas_Prefab.json");
		}

		return NextCanvasID;
	}

	void Editor::SaveAndPlayScene()
	{
		// save active scene name
		m_savedScene = SceneManager::GetInstance().GetActiveScene();
		serializationManager.SaveAllEntitiesToFile("Savestate/savestate.json");
		UndoStack::GetInstance().ClearStack();
	}

	void Editor::StopAndLoadScene()
	{
		m_currentSelectedObject = -1;

		// check if saved scene is same as the active scene
		if (m_savedScene == SceneManager::GetInstance().GetActiveScene())
		{
			// if active sccene is the same as active scene, restart scene
			SceneManager::GetInstance().RestartScene("Savestate/savestate.json");
		}
		else
		{
			// else need to reload assets for the scene
			SceneManager::GetInstance().LoadSceneToLoad("Savestate/savestate.json");
		}
		SceneManager::GetInstance().SetActiveScene(m_savedScene);
	}

	void Editor::SetImGUIStyle_Dark()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		// Base colors
		ImVec4 darkBg = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		ImVec4 lightText = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		ImVec4 subtleAccent = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		ImVec4 borderGray = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		ImVec4 brightAccent = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);

		// Text
		colors[ImGuiCol_Text] = lightText;
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

		// Backgrounds
		colors[ImGuiCol_WindowBg] = darkBg;
		colors[ImGuiCol_ChildBg] = darkBg;
		colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.94f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = darkBg;

		// Borders
		colors[ImGuiCol_Border] = borderGray;
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// Frames
		colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = subtleAccent;
		colors[ImGuiCol_FrameBgActive] = borderGray;

		// Titles
		colors[ImGuiCol_TitleBg] = darkBg;
		colors[ImGuiCol_TitleBgActive] = subtleAccent;
		colors[ImGuiCol_TitleBgCollapsed] = darkBg;

		// Buttons
		colors[ImGuiCol_Button] = subtleAccent;
		colors[ImGuiCol_ButtonHovered] = borderGray;
		colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

		// Headers
		colors[ImGuiCol_Header] = subtleAccent;
		colors[ImGuiCol_HeaderHovered] = borderGray;
		colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

		// Other UI elements
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.53f);
		colors[ImGuiCol_CheckMark] = lightText;
		colors[ImGuiCol_SliderGrab] = brightAccent;
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = subtleAccent;
		colors[ImGuiCol_ResizeGripHovered] = borderGray;
		colors[ImGuiCol_ResizeGripActive] = borderGray;
		colors[ImGuiCol_Tab] = subtleAccent;
		colors[ImGuiCol_TabHovered] = borderGray;
		colors[ImGuiCol_TabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = darkBg;
		colors[ImGuiCol_TabUnfocusedActive] = darkBg;
		colors[ImGuiCol_PlotLines] = lightText;
		colors[ImGuiCol_PlotLinesHovered] = subtleAccent;
		colors[ImGuiCol_PlotHistogram] = subtleAccent;
		colors[ImGuiCol_PlotHistogramHovered] = borderGray;
		colors[ImGuiCol_TextSelectedBg] = borderGray;
		colors[ImGuiCol_DragDropTarget] = subtleAccent;
		colors[ImGuiCol_NavHighlight] = subtleAccent;
		colors[ImGuiCol_NavWindowingHighlight] = subtleAccent;
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

		// Table colors
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = borderGray;
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = darkBg;
		colors[ImGuiCol_TableRowBgAlt] = darkBg;

		// Rounding
		float rounding = 5.0f;
		style->WindowRounding = rounding;
		style->ChildRounding = rounding;
		style->FrameRounding = rounding;
		style->PopupRounding = rounding;
		style->ScrollbarRounding = rounding;
		style->GrabRounding = rounding;
		style->TabRounding = rounding;

		// Padding
		style->WindowPadding = ImVec2(8.0f, 8.0f);
		style->FramePadding = ImVec2(5.0f, 2.0f);
		style->ItemSpacing = ImVec2(6.0f, 4.0f);
	}

	void Editor::SetImGUIStyle_Light()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		// Base colors
		ImVec4 lightBg = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		ImVec4 darkText = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		ImVec4 mediumAccent = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		ImVec4 lightBorder = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
		ImVec4 darkAccent = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

		// Text
		colors[ImGuiCol_Text] = darkText;
		colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);

		// Backgrounds
		colors[ImGuiCol_WindowBg] = lightBg;
		colors[ImGuiCol_ChildBg] = lightBg;
		colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.94f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = lightBg;

		// Borders
		colors[ImGuiCol_Border] = lightBorder;
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// Frames
		colors[ImGuiCol_FrameBg] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = mediumAccent;
		colors[ImGuiCol_FrameBgActive] = lightBorder;

		// Titles
		colors[ImGuiCol_TitleBg] = lightBg;
		colors[ImGuiCol_TitleBgActive] = mediumAccent;
		colors[ImGuiCol_TitleBgCollapsed] = lightBg;

		// Buttons
		colors[ImGuiCol_Button] = mediumAccent;
		colors[ImGuiCol_ButtonHovered] = lightBorder;
		colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);

		// Headers
		colors[ImGuiCol_Header] = mediumAccent;
		colors[ImGuiCol_HeaderHovered] = lightBorder;
		colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);

		// Other UI elements
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.92f, 0.92f, 0.92f, 0.53f);
		colors[ImGuiCol_CheckMark] = darkText;
		colors[ImGuiCol_SliderGrab] = darkAccent;
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = mediumAccent;
		colors[ImGuiCol_ResizeGripHovered] = lightBorder;
		colors[ImGuiCol_ResizeGripActive] = lightBorder;
		colors[ImGuiCol_Tab] = mediumAccent;
		colors[ImGuiCol_TabHovered] = lightBorder;
		colors[ImGuiCol_TabActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = lightBg;
		colors[ImGuiCol_TabUnfocusedActive] = lightBg;
		colors[ImGuiCol_PlotLines] = darkText;
		colors[ImGuiCol_PlotLinesHovered] = mediumAccent;
		colors[ImGuiCol_PlotHistogram] = mediumAccent;
		colors[ImGuiCol_PlotHistogramHovered] = lightBorder;
		colors[ImGuiCol_TextSelectedBg] = lightBorder;
		colors[ImGuiCol_DragDropTarget] = mediumAccent;
		colors[ImGuiCol_NavHighlight] = mediumAccent;
		colors[ImGuiCol_NavWindowingHighlight] = mediumAccent;
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		// Table colors
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.89f, 0.89f, 0.90f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = lightBorder;
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.93f, 0.93f, 0.95f, 1.00f);
		colors[ImGuiCol_TableRowBg] = lightBg;
		colors[ImGuiCol_TableRowBgAlt] = lightBg;

		// Rounding
		float rounding = 5.0f;
		style->WindowRounding = rounding;
		style->ChildRounding = rounding;
		style->FrameRounding = rounding;
		style->PopupRounding = rounding;
		style->ScrollbarRounding = rounding;
		style->GrabRounding = rounding;
		style->TabRounding = rounding;

		// Padding
		style->WindowPadding = ImVec2(8.0f, 8.0f);
		style->FramePadding = ImVec2(5.0f, 2.0f);
		style->ItemSpacing = ImVec2(6.0f, 4.0f);
	}

	void Editor::SetImGUIStyle_Kurumi()
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
	}

	void Editor::LoadSceneFromGivenPath(std::string const& path)
	{
		UndoStack::GetInstance().ClearStack();

		// deselect object
		m_currentSelectedObject = -1;
		SceneManager::GetInstance().LoadSceneToLoad(path);

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

	Editor::LogColors Editor::GetLogColorsForCurrentStyle() const
	{
		switch (m_currentStyle)
		{
		case GuiStyle::DARK:
			return
			{
				ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
				ImVec4(0.4f, 1.0f, 1.0f, 1.0f),
				ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
				ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
			};
		case GuiStyle::LIGHT:
			return
			{
				ImVec4(0.9f, 0.1f, 0.1f, 1.0f),
				ImVec4(0.1f, 0.2f, 0.9f, 1.0f),
				ImVec4(0.9f, 0.5f, 0.0f, 1.0f),
				ImVec4(0.0f, 0.5f, 0.0f, 1.0f)
			};

		// Dark Blue theme
		case GuiStyle::KURUMI:
			return
			{
				ImVec4(0.7f, 0.3f, 0.3f, 1.0f),
				ImVec4(0.3f, 0.7f, 0.7f, 1.0f),
				ImVec4(0.7f, 0.7f, 0.3f, 1.0f),
				ImVec4(0.5f, 0.5f, 0.5f, 1.0f)
			};

		// Grey
		default:
			return
			{
				ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
				ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
				ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
				ImVec4(0.5f, 0.5f, 0.5f, 1.0f)
			};
		}
	}

	std::string GenerateTimestampID()
	{
		auto now = std::chrono::system_clock::now();
		auto now_c = std::chrono::system_clock::to_time_t(now);
		return std::to_string(now_c);
	}

	// Function to convert a string to lowercase
	std::string Editor::ToLower(const std::string& str)
	{
		std::string lowerStr = str;
		std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return lowerStr;
	}

	// Function to check if one string is a substring of another, ignoring case
	bool Editor::CaseInsensitiveFind(const std::string& str, const std::string& toFind)
	{
		std::string lowerStr = ToLower(str);
		std::string lowerToFind = ToLower(toFind);
		return lowerStr.find(lowerToFind) != std::string::npos;
	}


}
