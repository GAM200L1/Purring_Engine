/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GUISystem.cpp
 \date     02-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Checks for events and updates the states and calls the functions of all 
	GUI objects in the scene.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "GUISystem.h"
#include "Events/EventHandler.h"
#include "ECS/EntityFactory.h"
#include "ECS/SceneView.h"
#include "Input/InputSystem.h"
#include "Graphics/CameraManager.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

#define UI_CAST(type, ui) reinterpret_cast<type&>(ui)

std::map<std::string_view, std::function<void(::EntityID)>> PE::GUISystem::m_uiFunc;



namespace PE
{
	GUISystem::GUISystem(GLFWwindow* p_glfwWindow) : p_window{ p_glfwWindow }
	{ /* Empty by design */ }

	GUISystem::~GUISystem()
	{ /* Empty by design */	}

	void GUISystem::InitializeSystem()
	{
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GUISystem::OnMouseClick, this)
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseMoved, GUISystem::OnMouseHover, this)

		REGISTER_UI_FUNCTION(ButtonFunctionOne, PE::GUISystem);
		REGISTER_UI_FUNCTION(ButtonFunctionTwo, PE::GUISystem);
	}

	void GUISystem::UpdateSystem(float deltaTime)
	{
		deltaTime; // Prevent warnings

#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
			for (EntityID objectID : SceneView<GUI>())
			{
				GUI& gui = EntityManager::GetInstance().Get<GUI>(objectID);
				//gui.m_onClicked = "TestFunction";
				gui.Update();
				//gui.m_onClicked = m_uiFunc.at("ButtonFunctionOne").target<void()>();
				if (gui.m_UIType == UIType::Button)
				{
					Button btn = UI_CAST(Button, gui);
					gui.m_clickedTimer -= deltaTime;
					if (btn.m_Hovered)
					{
						btn.OnHover(objectID);
						if (EntityManager::GetInstance().Has(objectID, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()) && gui.m_clickedTimer <= 0)
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetColor(gui.m_hoveredColor.x, gui.m_hoveredColor.y, gui.m_hoveredColor.z, gui.m_hoveredColor.w);
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetTextureKey(gui.m_hoveredTexture);
						}
					}					
					else if(gui.m_clickedTimer <= 0)
					{
						EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetColor(gui.m_defaultColor.x, gui.m_defaultColor.y, gui.m_defaultColor.z, gui.m_defaultColor.w);
						EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetTextureKey(gui.m_defaultTexture);
					}

				}
			}
	}

	void GUISystem::DestroySystem()
	{
	}

	std::string GUISystem::GetName()
	{
		return "GUISystem";
	}

	void GUISystem::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		//loop through all objects check for Button Component
#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
			for (EntityID objectID : SceneView<Transform, GUI>())
			{
				if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive)
					continue;
				//get the components
				Transform& transform = EntityManager::GetInstance().Get<Transform>(objectID);
				GUI& gui = EntityManager::GetInstance().Get<GUI>(objectID);

				float mouseX{ static_cast<float>(MBPE.x) }, mouseY{ static_cast<float>(MBPE.y) };
				InputSystem::ConvertGLFWToTransform(p_window, mouseX, mouseY);
				mouseX = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).x;
				mouseY = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).y;

				if (!IsInBound(static_cast<int>(mouseX), static_cast<int>(mouseY), transform))
					continue;

				if (gui.m_UIType == UIType::Button)
				{
					Button btn = UI_CAST(Button, gui);
					btn.OnClick(objectID);
					gui.m_clickedTimer = .3f;
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetColor(gui.m_pressedColor.x, gui.m_pressedColor.y, gui.m_pressedColor.z, gui.m_pressedColor.w);
					EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetTextureKey(gui.m_pressedTexture);
				}
			}
	}

	bool GUISystem::IsInBound(int x, int y, Transform t)
	{
		if (x >= t.position.x - t.width / 2 && x <= t.position.x + t.width/2 &&
			y >= t.position.y - t.height / 2 && y <= t.position.y + t.height/2) 
		{
			return true;
		}
		else 
		{
			return false; 
		}
	}

	void GUISystem::OnMouseHover(const Event<MouseEvents>& r_ME)
	{
		MouseMovedEvent MME = dynamic_cast<const MouseMovedEvent&>(r_ME);
#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
		for (EntityID objectID : SceneView<Transform, GUI>())
		{
			if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive)
				continue;
			//get the components
			Transform& transform = EntityManager::GetInstance().Get<Transform>(objectID);
			GUI& gui = EntityManager::GetInstance().Get<GUI>(objectID);

			float mouseX{ static_cast<float>(MME.x) }, mouseY{ static_cast<float>(MME.y) };
			InputSystem::ConvertGLFWToTransform(p_window, mouseX, mouseY);
			mouseX = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).x;
			mouseY = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).y;

			//check mouse coordinate against transform here
			if (IsInBound(static_cast<int>(mouseX), static_cast<int>(mouseY), transform))
			{
				gui.m_Hovered = true;
			}
			else
			{
				gui.m_Hovered = false;
			}
		}
	}

	void GUISystem::ButtonFunctionOne(EntityID)
	{
		std::cout << "function 1" << std::endl;
	}

	void GUISystem::ButtonFunctionTwo(EntityID)
	{
		std::cout << "hi im function 2" << std::endl;
	}
	void GUISystem::AddFunction(std::string_view str, const std::function<void(EntityID)>& func)
	{
		m_uiFunc[str] = func;
	}


	// Serialize GUI
	nlohmann::json GUI::ToJson(size_t id) const
	{
		UNREFERENCED_PARAMETER(id);

		nlohmann::json j;
		// Serialize properties
		j["m_onClicked"] = m_onClicked;
		j["m_onHovered"] = m_onHovered;
		j["m_UIType"] = static_cast<int>(m_UIType);
		return j;
	}

	// Deserialize GUI
	GUI GUI::Deserialize(const nlohmann::json& j)
	{
		GUI gui;
		gui.m_onClicked = j["m_onClicked"];
		gui.m_onHovered = j["m_onHovered"];
		gui.m_UIType = static_cast<UIType>(j["m_UIType"].get<int>());
		return gui;
	}
}