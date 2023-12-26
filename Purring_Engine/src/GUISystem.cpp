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
#include "Graphics/CameraManager.h"
#include "Events/EventHandler.h"
#include "ECS/EntityFactory.h"
#include "ECS/SceneView.h"
#include "Input/InputSystem.h"
#include "System.h"
#include "WindowManager.h"
#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

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
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, GUISystem::OnMouseRelease, this)

		REGISTER_UI_FUNCTION(ButtonFunctionOne, PE::GUISystem);
		REGISTER_UI_FUNCTION(ButtonFunctionTwo, PE::GUISystem);
	}

	void GUISystem::UpdateSystem(float deltaTime)
	{
		deltaTime; // Prevent warnings

#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
			for (EntityID objectID : SceneView<GUIButton>())
			{
				GUIButton& gui = EntityManager::GetInstance().Get<GUIButton>(objectID);
				gui.Update();
				gui.m_clickedTimer -= deltaTime;

				if (gui.disabled)
				{			
						if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(objectID)) 
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetColor(gui.m_disabledColor.x, gui.m_disabledColor.y, gui.m_disabledColor.z, gui.m_disabledColor.w);
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetTextureKey(gui.m_disabledTexture);
						}
					continue;
				}
				if (gui.m_Hovered)
				{
					gui.OnHover(objectID);
					if (EntityManager::GetInstance().Has(objectID, EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()) && gui.m_clickedTimer <= 0)
					{
						EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetColor(gui.m_hoveredColor.x, gui.m_hoveredColor.y, gui.m_hoveredColor.z, gui.m_hoveredColor.w);
						EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetTextureKey(gui.m_hoveredTexture);
					}
				}					
				else if(gui.m_clickedTimer <= 0)
				{
					if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(objectID)) {
						EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetColor(gui.m_defaultColor.x, gui.m_defaultColor.y, gui.m_defaultColor.z, gui.m_defaultColor.w);
						EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetTextureKey(gui.m_defaultTexture);
					}
				}
			}

			//check through all sliders, make sure all slider has a knob during editor runtime
			for (EntityID objectID : SceneView<GUISlider>())
			{
				GUISlider& slider = EntityManager::GetInstance().Get <GUISlider>(objectID);
#ifndef GAMERELEASE
				if (Editor::GetInstance().IsEditorActive()) {
					if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).children.empty())
					{
						for (auto& cid : EntityManager::GetInstance().Get<EntityDescriptor>(objectID).children)
						{
							if (EntityManager::GetInstance().Get<EntityDescriptor>(cid).name == "SliderKnob")
							{
								slider.m_knobID = cid;
								break;
							}
						}
					}
					if (!slider.m_knobID.has_value())
					{
						SerializationManager sm;

						slider.m_knobID = sm.LoadFromFile(("EditorDefaults/SliderKnob_Prefab.json"));
						EntityManager::GetInstance().Get<EntityDescriptor>(objectID).children.emplace(slider.m_knobID.value());
						EntityManager::GetInstance().Get<EntityDescriptor>(slider.m_knobID.value()).parent = objectID;
					}
					else
					{
						Transform& knobTransform = EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value());

						//making sure always at the same height of the slider
						knobTransform.relPosition.y = 0;

						//start point will be
						//transform of slider - slider width/2 + knob width/2
						//however because we using knob we need to use relative coordinates, so start from 0
						//basically ignore the transform of the slider
						slider.m_startPoint = 0 - EntityManager::GetInstance().Get<Transform>(objectID).width / 2 + EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value()).width / 2;

						//end point will be
						//transform of slider + slider width/2 - knob width/2
						//however because we using knob we need to use relative coordinates, so start from 0
						//basically ignore the transform of the slider
						slider.m_endPoint = EntityManager::GetInstance().Get<Transform>(objectID).width / 2 - EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value()).width / 2;

						//if the knob is ever out of area
						if (knobTransform.relPosition.x < slider.m_startPoint)
						{
							knobTransform.relPosition.x = slider.m_startPoint;
						}
						else if (knobTransform.relPosition.x > slider.m_endPoint)
						{
							knobTransform.relPosition.x = slider.m_endPoint;
						}

						slider.CalculateValue(knobTransform.relPosition.x);
					}
				}
#endif

#ifndef GAMERELEASE
				if (Editor::GetInstance().IsRunTime()) {
#endif
					if (slider.m_disabled)
					{
						if (EntityManager::GetInstance().Has(slider.m_knobID.value(), EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(slider.m_knobID.value()).SetColor(slider.m_disabledColor.x, slider.m_disabledColor.y, slider.m_disabledColor.z, slider.m_disabledColor.w);
							//EntityManager::GetInstance().Get<Graphics::GUIRenderer>(slider.m_knobID.value()).SetTextureKey(slider.m_disabledTexture);
						}
						continue;
					}

					if (slider.m_clicked)
					{
						float mouseX{}, mouseY{};
						InputSystem::GetCursorViewportPosition(p_window, mouseX, mouseY);
						vec2 CurrentMousePos = GETCAMERAMANAGER()->GetUiWindowToScreenPosition(mouseX, mouseY);
						//vec2 CurrentMousePos = GETCAMERAMANAGER()->GetUiCamera().GetViewportToWorldPosition(mouseX, mouseY);

						Transform& knobTransform = EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value());
						Transform& sliderTransform = EntityManager::GetInstance().Get<Transform>(objectID);

						knobTransform.relPosition.x = CurrentMousePos.x - sliderTransform.position.x;

						if (knobTransform.relPosition.x < slider.m_startPoint)
						{ 
							knobTransform.relPosition.x = slider.m_startPoint;
						}
						else if (knobTransform.relPosition.x > slider.m_endPoint)
						{
							knobTransform.relPosition.x = slider.m_endPoint;
						}

						slider.CalculateValue(knobTransform.relPosition.x);

						if (EntityManager::GetInstance().Has(slider.m_knobID.value(), EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(slider.m_knobID.value()).SetColor(slider.m_pressedColor.x, slider.m_pressedColor.y, slider.m_pressedColor.z, slider.m_pressedColor.w);
							//EntityManager::GetInstance().Get<Graphics::GUIRenderer>(slider.m_knobID.value()).SetTextureKey(slider.m_pressedTexture);
						}
					}
					else if (slider.m_Hovered && !slider.m_clicked)
					{
						if (EntityManager::GetInstance().Has(slider.m_knobID.value(), EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(slider.m_knobID.value()).SetColor(slider.m_hoveredColor.x, slider.m_hoveredColor.y, slider.m_hoveredColor.z, slider.m_hoveredColor.w);
							//EntityManager::GetInstance().Get<Graphics::GUIRenderer>(slider.m_knobID.value()).SetTextureKey(slider.m_hoveredTexture);
						}
					}
					else
					{
						if (EntityManager::GetInstance().Has(slider.m_knobID.value(), EntityManager::GetInstance().GetComponentID<Graphics::GUIRenderer>()))
						{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(slider.m_knobID.value()).SetColor(slider.m_defaultColor.x, slider.m_defaultColor.y, slider.m_defaultColor.z, slider.m_defaultColor.w);
							//EntityManager::GetInstance().Get<Graphics::GUIRenderer>(slider.m_knobID.value()).SetTextureKey(slider.m_defaultTexture);
						}
					}
#ifndef GAMERELEASE
				}
#endif
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
			for (EntityID objectID : SceneView<Transform, GUIButton>())
			{
				if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive)
					continue;
				//get the components
				Transform& transform = EntityManager::GetInstance().Get<Transform>(objectID);
				GUIButton& gui = EntityManager::GetInstance().Get<GUIButton>(objectID);

				if (gui.disabled)
					continue;

				float mouseX{ static_cast<float>(MBPE.x) }, mouseY{ static_cast<float>(MBPE.y) };
				InputSystem::ConvertGLFWToTransform(p_window, mouseX, mouseY);
				mouseX = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).x;
				mouseY = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).y;

				if (gui.m_UIType == UIType::Button)
				{
					if (!IsInBound(static_cast<int>(mouseX), static_cast<int>(mouseY), transform))
						continue;
					if (gui.m_clickedTimer <= 0)
					{
						gui.OnClick(objectID);
						gui.m_clickedTimer = .3f;
						if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(objectID)) {
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetColor(gui.m_pressedColor.x, gui.m_pressedColor.y, gui.m_pressedColor.z, gui.m_pressedColor.w);
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(objectID).SetTextureKey(gui.m_pressedTexture);
						}
						return;
					}
				}
			}
#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
		for (EntityID objectID : SceneView<Transform, GUISlider>())
		{
			if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive)
				continue;
			//get the components
			GUISlider& slider = EntityManager::GetInstance().Get <GUISlider>(objectID);
			Transform& transform = EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value());

			if (slider.m_disabled)
				continue;

			float mouseX{ static_cast<float>(MBPE.x) }, mouseY{ static_cast<float>(MBPE.y) };
			InputSystem::ConvertGLFWToTransform(p_window, mouseX, mouseY);
			mouseX = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).x;
			mouseY = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).y;

			if (!IsInBound(static_cast<int>(mouseX), static_cast<int>(mouseY), transform))
				continue;

			slider.m_clicked = true;
			std::cout << "clicked" << std::endl;
		}
	}

	void GUISystem::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		r_ME; // to stop error

#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
		for (EntityID objectID : SceneView<Transform, GUISlider>())
		{
			GUISlider& slider = EntityManager::GetInstance().Get <GUISlider>(objectID);
			std::cout << "released" << std::endl;
			slider.m_clicked = false;
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
		for (EntityID objectID : SceneView<Transform, GUIButton>())
		{
			if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive)
				continue;
			//get the components
			Transform& transform = EntityManager::GetInstance().Get<Transform>(objectID);
			GUIButton& gui = EntityManager::GetInstance().Get<GUIButton>(objectID);
			if (gui.disabled)
				continue;
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

#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
			for (EntityID objectID : SceneView<Transform, GUISlider>())
			{
				if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive)
					continue;
				//get the components
				GUISlider& slider = EntityManager::GetInstance().Get <GUISlider>(objectID);
				Transform& transform = EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value());

				if (slider.m_disabled)
					continue;

				float mouseX{ static_cast<float>(MME.x) }, mouseY{ static_cast<float>(MME.y) };
				InputSystem::ConvertGLFWToTransform(p_window, mouseX, mouseY);
				mouseX = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).x;
				mouseY = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).y;

				//check mouse coordinate against transform here
				if (IsInBound(static_cast<int>(mouseX), static_cast<int>(mouseY), transform))
				{
					slider.m_Hovered = true;
				}
				else
				{
					slider.m_Hovered = false;
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
	nlohmann::json GUIButton::ToJson(size_t id) const
	{
		UNREFERENCED_PARAMETER(id);

		nlohmann::json j;
		// Serialize properties
		j["m_onClicked"] = m_onClicked;
		j["m_onHovered"] = m_onHovered;
		j["m_UIType"] = static_cast<int>(m_UIType);
		j["disabled"] = disabled;

		// Serialize textures and colors
		j["m_defaultTexture"] = m_defaultTexture;
		j["m_hoveredTexture"] = m_hoveredTexture;
		j["m_pressedTexture"] = m_pressedTexture;
		j["m_disabledTexture"] = m_disabledTexture;

		// Serialize color vectors as arrays
		j["m_defaultColor"] = { m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, m_defaultColor.w };
		j["m_hoveredColor"] = { m_hoveredColor.x, m_hoveredColor.y, m_hoveredColor.z, m_hoveredColor.w };
		j["m_pressedColor"] = { m_pressedColor.x, m_pressedColor.y, m_pressedColor.z, m_pressedColor.w };
		j["m_disabledColor"] = { m_disabledColor.x, m_disabledColor.y, m_disabledColor.z, m_disabledColor.w };

		return j;
	}

	// Deserialize GUI
	GUIButton GUIButton::Deserialize(const nlohmann::json& r_j)
	{
		GUIButton gui;
		gui.m_onClicked = r_j["m_onClicked"];
		gui.m_onHovered = r_j["m_onHovered"];
		gui.m_UIType = static_cast<UIType>(r_j["m_UIType"].get<int>());
		gui.disabled = r_j["disabled"].get<bool>();

		// Deserialize textures
		gui.m_defaultTexture = r_j["m_defaultTexture"];
		gui.m_hoveredTexture = r_j["m_hoveredTexture"];
		gui.m_pressedTexture = r_j["m_pressedTexture"];
		gui.m_disabledTexture = r_j["m_disabledTexture"];

		// Deserialize colors
		gui.m_defaultColor = vec4(r_j["m_defaultColor"][0], r_j["m_defaultColor"][1], r_j["m_defaultColor"][2], r_j["m_defaultColor"][3]);
		gui.m_hoveredColor = vec4(r_j["m_hoveredColor"][0], r_j["m_hoveredColor"][1], r_j["m_hoveredColor"][2], r_j["m_hoveredColor"][3]);
		gui.m_pressedColor = vec4(r_j["m_pressedColor"][0], r_j["m_pressedColor"][1], r_j["m_pressedColor"][2], r_j["m_pressedColor"][3]);
		gui.m_disabledColor = vec4(r_j["m_disabledColor"][0], r_j["m_disabledColor"][1], r_j["m_disabledColor"][2], r_j["m_disabledColor"][3]);

		return gui;
	}

	void GUISlider::Update()
	{

	}

	void GUISlider::Destroy()
	{

	}
	float GUISlider::CalculateValue(float currentX)
	{
		//current value will be 
		//current x transform / total transform * (max value - min value)
		m_currentValue = ( (currentX+((m_endPoint - m_startPoint)/2) ) / (m_endPoint - m_startPoint) ) * (m_maxValue - m_minValue) + m_minValue;
		return m_currentValue;
	}
}