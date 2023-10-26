/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Editor.h
 \date     30-08-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	header file containing the declaration of the editor class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "../Events/EventHandler.h"


#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

#include "Singleton.h"

#include "Graphics/GLHeaders.h"

#include "ECS//EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Data/SerializationManager.h"
#include "Data/json.hpp"


namespace PE {

	class Editor : public Singleton<Editor>
	{
		// ----- Singleton ----- // 
		friend class Singleton<Editor>;

		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		\brief	Constructor of the class Editor();
		*************************************************************************************/
		Editor();
		/*!***********************************************************************************
		\brief	Destructor of the class Editor();
		*************************************************************************************/
		virtual ~Editor();

		// ----- System Functions ----- // 
	public:
		/*!***********************************************************************************
		 \brief Runs ImGui init functions

		 \param[in] GLFWwindow 	the glfw window that we are drawing onto
		*************************************************************************************/
		void Init(GLFWwindow* p_window);
		/*!***********************************************************************************
		 \brief Render all imgui windows

		 \param[in] GLuint 	the id of the texture to be drawn on the scene view
		*************************************************************************************/
		void Render(GLuint texture_id);

		// ----- Public Getters ----- // 
	public:
		/*!***********************************************************************************
		 \brief Return the size of the imgui window
		 \param[out] float x value
		 \param[out] float y value
		*************************************************************************************/
		void GetWindowSize(float&, float&);
		/*!***********************************************************************************
		 \brief get the boolean to show if the editor is active
		 \return bool whether editor is active so we know we need to transfer the buffer
		*************************************************************************************/
		bool IsEditorActive();
		/*!***********************************************************************************
		 \brief get the boolean to know if we need to render debug lines
		 \return bool return whether debug drawing is on or off
		*************************************************************************************/
		bool IsRenderingDebug();
		/*!***********************************************************************************
		 \brief get the boolean to know editor is in runtime
		 \return bool return whether it is editor mode or runtime
		*************************************************************************************/
		bool IsRunTime();
		/*!***********************************************************************************
		 \brief get the boolean to know editor is in runtime
		 \return bool return whether it is editor mode or runtime
		*************************************************************************************/
		bool IsMouseInScene();

		// ----- Public Logging Functions ----- // 
	public:
		/*!***********************************************************************************
		 \brief add logs to the log vector to print on the log window

		 \param[in] std::string the string to print on the log window
		*************************************************************************************/
		void AddLog(std::string_view text);
		/*!***********************************************************************************
		 \brief add text to the console vector to print on the console window

		 \param[in] std::string the string to print on the console window
		*************************************************************************************/
		void AddConsole(std::string_view text);
		/*!***********************************************************************************
		\brief add text appeneded with error to the console vector to print on the console window

		\param[in] std::string the string to print on the console window
		*************************************************************************************/
		void AddErrorLog(std::string_view text);

		/*!***********************************************************************************
		\brief add text appeneded with info to the console vector to print on the console window

		\param[in] std::string the string to print on the console window
		*************************************************************************************/
		void AddInfoLog(std::string_view text);
		/*!***********************************************************************************
		\brief add text appeneded with event to the console vector to print on the console window

		\param[in] std::string the string to print on the console window
		*************************************************************************************/
		void AddEventLog(std::string_view text);

		/*!***********************************************************************************
		\brief add text appeneded with warning to the console vector to print on the console window

		\param[in] std::string the string to print on the console window
		*************************************************************************************/
		void AddWarningLog(std::string_view text);

		// -----Event Callbacks ----- // 
	public:
		/*!***********************************************************************************
		 \brief the callback function for an onkeyevent
		 \param[in] const temp::Event<temp::KeyEvents>& event called
		*************************************************************************************/
		void OnKeyTriggeredEvent(const PE::Event<PE::KeyEvents>& r_e);
		// ----- ImGui Window Functions ----- // 
	private:
		/*!***********************************************************************************
		 \brief render the logs window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowLogsWindow(bool* Active);
		/*!***********************************************************************************
		 \brief render the console window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowConsoleWindow(bool* Active);
		/*!***********************************************************************************
		 \brief render the object list window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowObjectWindow(bool* Active);
		/*!***********************************************************************************
		 \brief render the test windows (Temp for milestone 1 for elie to test)

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowDemoWindow(bool* Active);
		/*!***********************************************************************************
		 \brief render the object component window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowComponentWindow(bool* Active);
		/*!***********************************************************************************
		 \brief render the resource list window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowResourceWindow(bool* Active);
		/*!***********************************************************************************
		 \brief render the peformance profile window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowPerformanceWindow(bool* Active);
		/*!***********************************************************************************
		 \brief render the docking port window

		 \param[in] bool* reference to the boolean that sets the window active (Ill add soon maybe)
		*************************************************************************************/
		void SetDockingPort(bool* Active);
		/*!***********************************************************************************
		 \brief render the sceneview window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowSceneView(GLuint texture_id, bool* active);
		
		void SetImGUIStyle();

		// ----- Private Logging Functions ----- // 
	private:
		/*!***********************************************************************************
		 \brief empty the log vector
		*************************************************************************************/
		void ClearLog();
		/*!***********************************************************************************
		 \brief empty the console vector
		*************************************************************************************/
		void ClearConsole();

		// ----- Private Methods ----- //
	private:
		/*!***********************************************************************************
		 \brief toggle rendering of debug lines
		*************************************************************************************/
		void ToggleDebugRender();

		// ----- ImGui Command Functions ----- // 
	private:
		/*!***********************************************************************************
		 \brief command line ping that replies pong
		*************************************************************************************/
		void ping();
		/*!***********************************************************************************
		 \brief command line test that opens the test window
		*************************************************************************************/
		void test();

		// ----- Private Functions ----- // 
		/*!***********************************************************************************
		 \brief Clear all the objects on the scene
		*************************************************************************************/
		void ClearObjectList();

		static void HotLoadingNewFiles(GLFWwindow* p_window, int count, const char** paths);

		// ----- Private Variables ----- // 
	private:
		//boolean for windows
		bool m_showLogs;
		bool m_showObjectList;
		bool m_showConsole;
		bool m_showSceneView;
		bool m_showTestWindows;
		bool m_showEditor;
		bool m_showComponentWindow;
		bool m_showResourceWindow;
		bool m_showPerformanceWindow;
		bool m_firstLaunch;

		//boolean for rendering
		bool m_renderDebug;
		bool m_isRunTime;

		//variables for logging
		std::vector<std::string> m_logOutput;
		std::vector<std::string> m_consoleOutput;
		std::string m_input;
		std::string m_findText;
		std::map<std::string_view, void (PE::Editor::*)()> m_commands;

		//variable for objects
		bool m_objectIsSelected;
		int m_currentSelectedObject;

		//variable for assets browser
		float m_time;
		float m_renderWindowWidth, m_renderWindowHeight;
		GLFWwindow* p_window;
		bool m_mouseInScene;
		static std::filesystem::path m_parentPath;
		std::vector<std::filesystem::path> m_files;

	};
}

