/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Editor.cpp
 \date     28-10-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu

 \brief			Definition of the callback functions for events for the editor class.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "Editor.h"

namespace PE {
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
