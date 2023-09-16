/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     ImGuiWindow.h
 \date     9/5/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
Header file containing the declaration and definition of the event and event dispatcher template

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "../Events/EventHandler.h"
#include <vector>
#include <string>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

#include "Graphics/GLHeaders.h"


class ImGuiWindow
{
	// ----- Constructors ----- // 
public:
	/*!***********************************************************************************
	\brief	Constructor of the class ImGuiWindow();
	*************************************************************************************/
	ImGuiWindow();
	/*!***********************************************************************************
	\brief	Destructor of the class ImGuiWindow();
	*************************************************************************************/
	virtual ~ImGuiWindow();

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
	void showLogs(bool* Active);
	/*!***********************************************************************************
	 \brief render the console window

	 \param[in] bool* reference to the boolean that sets the window active
	*************************************************************************************/
	void showConsole(bool* Active);
	/*!***********************************************************************************
	 \brief render the object list window

	 \param[in] bool* reference to the boolean that sets the window active
	*************************************************************************************/
	void showObject(bool* Active);
	/*!***********************************************************************************
	 \brief render the docking port window

	 \param[in] bool* reference to the boolean that sets the window active (Ill add soon maybe)
	*************************************************************************************/
	void setDockingPort();
	/*!***********************************************************************************
	 \brief render the sceneview window

	 \param[in] bool* reference to the boolean that sets the window active
	*************************************************************************************/
	void showSceneView(GLuint texture_id, bool* active);

	// ----- ImGui Logging Functions ----- // 
public:
	/*!***********************************************************************************
	 \brief add logs to the log vector to print on the log window

	 \param[in] std::string the string to print on the log window
	*************************************************************************************/
	void addLog(std::string text);
	/*!***********************************************************************************
	 \brief add text to the console vector to print on the console window

	 \param[in] std::string the string to print on the console window
	*************************************************************************************/
	void addCommand(std::string text);
private:
	/*!***********************************************************************************
	 \brief empty the log vector
	*************************************************************************************/
	void clearLog();
	/*!***********************************************************************************
	 \brief empty the console vector
	*************************************************************************************/
	void clearConsole();

	// -----Event Callbacks ----- // 
public:
	/*!***********************************************************************************
	 \brief the callback function for an onkeyevent
	 \param[in] const temp::Event<temp::KeyEvents>& event called
	*************************************************************************************/
	void OnKeyEvent(const temp::Event<temp::KeyEvents>& e);

	// ----- Getters ----- // 
public:
	/*!***********************************************************************************
	 \brief Get the instance of imguiwindow, if it does not exist, create it.
	 \return ImGuiWindow* 	 pointer to ImGuiWindow
	*************************************************************************************/
	static ImGuiWindow* GetInstance();
	/*!***********************************************************************************
	 \brief Get the instance of imguiwindow, if it does not exist, create it.
	 \param[out] float x value
	 \param[out] float y value
	*************************************************************************************/
	void GetWindowSize(float&, float&);
	/*!***********************************************************************************
	 \brief Get the instance of imguiwindow, if it does not exist, create it.
	 \return bool whether sceneview is active so we know we need to transfer the buffer
	*************************************************************************************/
	bool isSceneViewActive();
	// ----- Private Variables ----- // 
private:
	bool m_logsActive;
	bool m_objectActive;
	bool m_consoleActive;
	bool m_sceneViewActive;
	static std::unique_ptr<ImGuiWindow> s_Instance;
	std::vector<std::string> m_logOutput;
	std::vector<std::string> m_consoleOutput;
	std::string m_input;
	int m_currentSelectedIndex;
	std::vector<std::string> m_items;
	float m_time;
	float m_renderWindowWidth, m_renderWindowHeight;
};