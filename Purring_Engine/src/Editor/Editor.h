/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     Editor.h
 \date     8/30/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	header file containing the declaration of the imguiwindow class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "../Events/EventHandler.h"
#include <vector>
#include <string>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>
#include <iostream>

#include "Graphics/GLHeaders.h"

namespace PE {

	class Editor
	{
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
		void Init(GLFWwindow*);
		/*!***********************************************************************************
		 \brief Render all imgui windows

		 \param[in] GLuint 	the id of the texture to be drawn on the scene view
		*************************************************************************************/
		void Render(GLuint texture_id);

		// ----- ImGui Window Functions ----- // 
	private:
		/*!***********************************************************************************
		 \brief render the logs window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowLogs(bool* Active);
		/*!***********************************************************************************
		 \brief render the console window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowConsole(bool* Active);
		/*!***********************************************************************************
		 \brief render the object list window

		 \param[in] bool* reference to the boolean that sets the window active
		*************************************************************************************/
		void ShowObject(bool* Active);

		void ShowDebugTests(bool* Active);

		void ShowComponentWindow(bool* Active);

		void ShowResourceWindow(bool* Active);
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

		// ----- ImGui Logging Functions ----- // 
	public:
		/*!***********************************************************************************
		 \brief add logs to the log vector to print on the log window

		 \param[in] std::string the string to print on the log window
		*************************************************************************************/
		void AddLog(std::string text);
		/*!***********************************************************************************
		 \brief add text to the console vector to print on the console window

		 \param[in] std::string the string to print on the console window
		*************************************************************************************/
		void AddConsole(std::string text);
		/*!***********************************************************************************
		\brief add text appeneded with error to the console vector to print on the console window

		\param[in] std::string the string to print on the console window
		*************************************************************************************/
		void AddError(std::string text);
	private:
		/*!***********************************************************************************
		 \brief empty the log vector
		*************************************************************************************/
		void ClearLog();
		/*!***********************************************************************************
		 \brief empty the console vector
		*************************************************************************************/
		void ClearConsole();

		// -----Event Callbacks ----- // 
	public:
		/*!***********************************************************************************
		 \brief the callback function for an onkeyevent
		 \param[in] const temp::Event<temp::KeyEvents>& event called
		*************************************************************************************/
		void OnKeyPressedEvent(const temp::Event<temp::KeyEvents>& e);
		void OnMousePressedEvent(const temp::Event<temp::MouseEvents>& e);

		// ----- Getters ----- // 
	public:
		/*!***********************************************************************************
		 \brief Get the instance of imguiwindow, if it does not exist, create it.
		 \return Editor* 	 pointer to Editor
		*************************************************************************************/
		static Editor* GetInstance();
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
		 \brief set the editor on or off
		*************************************************************************************/
		void ToggleEditor();
		// ----- Private Variables ----- // 
	private:
		bool m_showLogs;
		bool m_showObjectList;
		bool m_showConsole;
		bool m_showSceneView;
		bool m_showTestWindows;
		bool m_showEditor;
		bool m_showComponentWindow;
		bool m_showResourceWindow;
		bool m_firstLaunch;
		static std::unique_ptr<Editor> s_Instance;
		std::vector<std::string> m_logOutput;
		std::vector<std::string> m_consoleOutput;
		std::string m_input;
		bool m_objectIsSelected;
		int m_currentSelectedIndex;
		std::vector<std::string> m_items;
		float m_time;
		float m_renderWindowWidth, m_renderWindowHeight;
		GLFWwindow* p_window;
	};
}

