/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Editor.cpp
 \date     28-10-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu
 \par      code %:     50%
 \par      changes:    Defined the callback function for key events.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     50%
 \par      changes:    Defined the callback function for mouse events.

 \brief			Definition of the callback functions for events for the editor class.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "Editor.h"

namespace PE {
		void Editor::OnMouseEvent(const PE::Event<PE::MouseEvents>&)
		{
				//Screen picking testing code
				//if (ImGui::IsMouseClicked(0))
				//{
				//    //where i need to start doing the screen picking
				//    //get the mouse position relative to the top - left corner of the ImGui window.
				//    ImVec2 cursorToMainWindow = ImGui::GetCursorScreenPos(); // get current window position (top left corner)
				//    ImVec2 CurrentWindowPosition = ImGui::GetWindowPos(); // seems to get the same thing
				//    ImVec2 CursorToImGuiWindow = ImGui::GetMousePos();  // get mouse position but relative to your screen
				//    ImVec2 windowSize = ImGui::GetWindowSize();

				//    double glfwMouseX, glfwMouseY;
				//    glfwGetCursorPos(p_window, &glfwMouseX, &glfwMouseY); //glfw position

				//    std::cout << "[Get current window top left position w title] x screen: " << cursorToMainWindow[0] << " y screen: " << cursorToMainWindow[1] << std::endl;
				//    std::cout << "[Get Mouse Pos] x : " << CursorToImGuiWindow[0] << " y : " << CursorToImGuiWindow[1] << std::endl;
				//    std::cout << "[Get Current Window View Top left position] x : " << CurrentWindowPosition[0] << " y i: " << CurrentWindowPosition[1] << std::endl;
				//    //this tells you mouse position relative to imgui window seems the most useful for now
				//    std::cout << "[Gui mouse pos - cursorscreen pos] x:" << CursorToImGuiWindow[0] - cursorToMainWindow[0] << " y: " << CursorToImGuiWindow[1] - cursorToMainWindow[1] << std::endl;
				//    std::cout << "[GLFW] x:" << glfwMouseX << " y: " << glfwMouseY << std::endl;    
				//}
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
}
