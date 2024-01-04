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
#include "Data/json.hpp"
#include "Data/SerializationManager.h"
#include "Logging/Logger.h"
#include "Logic/LogicSystem.h"
#include "UndoStack.h"
extern SerializationManager serializationManager;  // Create an instance
extern Logger engine_logger;

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

				if (KTE.keycode == GLFW_KEY_F5)
					m_showResourceWindow = !m_showResourceWindow;

				if (KTE.keycode == GLFW_KEY_F6)
					m_showPerformanceWindow = !m_showPerformanceWindow;

				if (KTE.keycode == GLFW_KEY_F7)
						m_showComponentWindow = !m_showComponentWindow;

				if (KTE.keycode == GLFW_KEY_F8)
					m_showPhysicsWindow = !m_showPhysicsWindow;

				if (KTE.keycode == GLFW_KEY_F9)
					m_showAnimationWindow = !m_showAnimationWindow;

				if (KTE.keycode == GLFW_KEY_P)
					m_showEditor = !m_showEditor;


				if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_CONTROL) && KTE.keycode == GLFW_KEY_Z)
				{
					UndoStack::GetInstance().UndoChange();
				}

				if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_CONTROL) && KTE.keycode == GLFW_KEY_Y)
				{
					UndoStack::GetInstance().RedoChange();
				}

				if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_SHIFT) && KTE.keycode == GLFW_KEY_F10)
				{
					m_showTestWindows = !m_showTestWindows;
				}
				else if (KTE.keycode == GLFW_KEY_F10)
				{
					ToggleDebugRender();
				}

				if (KTE.keycode == GLFW_KEY_DELETE)
				{
					for (const auto& id : SceneView())
					{
						if (EntityManager::GetInstance().Get<EntityDescriptor>(id).parent && EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.value() == m_currentSelectedObject)
							EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.reset();
					}
					if (m_currentSelectedObject != -1)
					{
						EntityManager::GetInstance().Get<EntityDescriptor>(m_currentSelectedObject).HandicapEntity();
						UndoStack::GetInstance().AddChange(new DeleteObjectUndo(m_currentSelectedObject));
					}
					//if not first index
					m_currentSelectedObject = -1; // just reset it
					//if object selected
					m_objectIsSelected = false;

					if (EntityManager::GetInstance().GetEntitiesInPool(ALL).empty()) m_currentSelectedObject = -1;//if nothing selected
				}

				if (InputSystem::IsKeyHeld(GLFW_KEY_R))
				{

					if (KTE.keycode == GLFW_KEY_X)
					{
						m_currentGizmoOperation = ImGuizmo::OPERATION::ROTATE_X;
					}
					else if (KTE.keycode == GLFW_KEY_Y)
					{
						m_currentGizmoOperation = ImGuizmo::OPERATION::ROTATE_Y;
					}
					else
					{
						m_currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
					}
				}

				if (InputSystem::IsKeyHeld(GLFW_KEY_S))
				{
					if (KTE.keycode == GLFW_KEY_X)
					{
						m_currentGizmoOperation = ImGuizmo::OPERATION::SCALE_X;
					}
					else if (KTE.keycode == GLFW_KEY_Y)
					{
						m_currentGizmoOperation = ImGuizmo::OPERATION::SCALE_Y;
					}
					else
					{
						m_currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
					}
				}

				if (InputSystem::IsKeyHeld(GLFW_KEY_T))
				{
					if (KTE.keycode == GLFW_KEY_X)
					{
						m_currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE_X;
					}
					else if (KTE.keycode == GLFW_KEY_Y)
					{
						m_currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE_Y;
					}
					else
					{
					m_currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
					}
				}
		}
}
