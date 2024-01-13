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
#include "Hierarchy/HierarchyManager.h"

#include "Input/InputSystem.h"
#include "GUI/Canvas.h"
#include "System.h"
#include "WindowManager.h"
#include "Hierarchy/HierarchyManager.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

std::map<std::string_view, std::function<void(::EntityID)>> PE::GUISystem::m_uiFunc;



namespace PE
{
	// Defining static variables
	std::unordered_set<EntityID> GUISystem::m_activeCanvases{};
	float GUISystem::m_targetResolutionWidth{};
	float GUISystem::m_targetResolutionHeight{};

	GUISystem::GUISystem(GLFWwindow* p_glfwWindow, float const width, float const height) : p_window{ p_glfwWindow }
	{ 
			m_activeCanvases.reserve(20); // Reserve a large amount of entities in advance
			m_targetResolutionWidth = width, m_targetResolutionHeight = height;
	}

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

		// Store the canvas objects
		m_activeCanvases.clear();
		for (EntityID canvasId : SceneView<Canvas>())
		{
				EntityManager::GetInstance().Get<Canvas>(canvasId).SetTargetResolution(m_targetResolutionWidth, m_targetResolutionHeight);
				if(EntityManager::GetInstance().Has<Transform>(canvasId))
						EntityManager::GetInstance().Get<Transform>(canvasId).position = vec2{ 0.f, 0.f };

				// Check if the canvas object has been enabled
				if (EntityManager::GetInstance().Get<EntityDescriptor>(canvasId).isActive && Hierarchy::GetInstance().AreParentsActive(canvasId))
				{
						// Check if the parent of the canvas object is active
						auto parentCanvas{ Hierarchy::GetInstance().GetParent(canvasId) };
						if (!parentCanvas.has_value() || EntityManager::GetInstance().Get<EntityDescriptor>(parentCanvas.value()).isActive)
						{
								m_activeCanvases.emplace(canvasId);
						}
				}
		}

		if (m_activeCanvases.empty()) { return; } // Don't bother with anything else if there are no canvases

#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
			for (EntityID objectID : SceneView<GUIButton>())
			{
				// Check if the object is childed to a canvas object, don't update if not
				if (!IsChildedToCanvas(objectID)) {  continue; }
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
				if (Editor::GetInstance().IsEditorActive()) 
				{
					if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).children.empty())
					{
						bool knobFound = false;
						for (auto& cid : EntityManager::GetInstance().Get<EntityDescriptor>(objectID).children)
						{
							if (EntityManager::GetInstance().Get<EntityDescriptor>(cid).name == "SliderKnob")
							{
								slider.m_knobID = cid;
								knobFound = true;

								if (EntityManager::GetInstance().Get<EntityDescriptor>(slider.m_knobID.value()).isAlive == false)
									continue;
								else
									break;
							}
						}

						//if there is children but no knob
						if (!knobFound || EntityManager::GetInstance().Get<EntityDescriptor>(slider.m_knobID.value()).isAlive == false)
						{
							SerializationManager sm;

							slider.m_knobID = sm.LoadFromFile(("EditorDefaults/SliderKnob_Prefab.json"));
							Hierarchy::GetInstance().AttachChild(objectID, slider.m_knobID.value());
						}
					}
					else
					{
						SerializationManager sm;

						slider.m_knobID = sm.LoadFromFile(("EditorDefaults/SliderKnob_Prefab.json"));
						Hierarchy::GetInstance().AttachChild(objectID, slider.m_knobID.value());
					}

					if (slider.m_knobID.has_value())
					{
						Transform& knobTransform = EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value());

						if (!slider.m_isHealthBar)
						{
							knobTransform.width = slider.m_currentWidth;

							//making sure always at the same height of the slider
							knobTransform.relPosition.y = 0;

							//start point will be
							//transform of slider - slider width/2 + knob width/2
							//however because we using knob we need to use relative coordinates, so start from 0
							//basically ignore the transform of the slider
							slider.m_startPoint = 0 - EntityManager::GetInstance().Get<Transform>(objectID).width / 2 + knobTransform.width / 2;

							//end point will be
							//transform of slider + slider width/2 - knob width/2
							//however because we using knob we need to use relative coordinates, so start from 0
							//basically ignore the transform of the slider
							slider.m_endPoint = EntityManager::GetInstance().Get<Transform>(objectID).width / 2 - knobTransform.width / 2;

							//if the knob is ever out of area
							if (knobTransform.relPosition.x < slider.m_startPoint)
							{
								knobTransform.relPosition.x = slider.m_startPoint;
							}
							else if (knobTransform.relPosition.x > slider.m_endPoint)
							{
								knobTransform.relPosition.x = slider.m_endPoint;
							}

							slider.CalculateKnobValue(knobTransform.relPosition.x);
						}
						else
						{
							knobTransform.relPosition.y = 0;
							slider.m_startPoint = 0 - EntityManager::GetInstance().Get<Transform>(objectID).width / 2;
							slider.m_endPoint = EntityManager::GetInstance().Get<Transform>(objectID).width / 2;


							float tv = slider.m_maxValue - slider.m_minValue;
							float ratio = slider.m_currentValue / tv;


							knobTransform.width = ratio * EntityManager::GetInstance().Get<Transform>(objectID).width;
							knobTransform.relPosition.x = slider.CalculateKnobCenter(slider.m_currentValue) - knobTransform.width/2;


							
						}
					}
				}
#endif

#ifndef GAMERELEASE
				if (Editor::GetInstance().IsRunTime()) {
#endif
					if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).children.empty())
					{
						if (!slider.m_isHealthBar)
						{
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

								slider.CalculateKnobValue(knobTransform.relPosition.x);

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
						}
						else
						{
							//only handles healthbar calculations
							Transform& knobTransform = EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value());

							knobTransform.relPosition.y = 0;
							slider.m_startPoint = 0 - EntityManager::GetInstance().Get<Transform>(objectID).width / 2;
							slider.m_endPoint = EntityManager::GetInstance().Get<Transform>(objectID).width / 2;

							float tv = slider.m_maxValue - slider.m_minValue;
							float ratio = slider.m_currentValue / tv;

							knobTransform.width = ratio * EntityManager::GetInstance().Get<Transform>(objectID).width;
							knobTransform.relPosition.x = slider.CalculateKnobCenter(slider.m_currentValue) - knobTransform.width / 2;
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


	bool GUISystem::IsImmediatelyChildedToCanvas(EntityID const uiId) const
	{
		auto parentCanvas{ Hierarchy::GetInstance().GetParent(uiId) };
		return (parentCanvas.has_value() && m_activeCanvases.find(parentCanvas.value()) != m_activeCanvases.end());
	}


	bool GUISystem::IsChildedToCanvas(EntityID uiId) const
	{
		// Loop through the parents of the object until we encounter a canvas
		bool isChilded{ false };
		while (Hierarchy::GetInstance().HasParent(uiId))
		{
			if(IsImmediatelyChildedToCanvas(uiId))
			{
				isChilded = true;
				break;
			}

			uiId = Hierarchy::GetInstance().GetParent(uiId).value();
		}
		return isChilded;
	}


	void GUISystem::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		if (m_activeCanvases.empty()) { return; } // Don't bother with anything else if there are no canvases

		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		//loop through all objects check for Button Component
#ifndef GAMERELEASE
		if (Editor::GetInstance().IsRunTime())
#endif
			for (EntityID objectID : SceneView<Transform, GUIButton>())
			{
				if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive || !IsChildedToCanvas(objectID))
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

			if (EntityManager::GetInstance().Get<EntityDescriptor>(objectID).children.empty())
				continue;
			//get the components


			GUISlider& slider = EntityManager::GetInstance().Get <GUISlider>(objectID);
			if (EntityManager::GetInstance().Has<Transform>(slider.m_knobID.value()))
			{
				Transform& transform = EntityManager::GetInstance().Get<Transform>(slider.m_knobID.value());

				if (slider.m_disabled || slider.m_isHealthBar)
					continue;

				float mouseX{ static_cast<float>(MBPE.x) }, mouseY{ static_cast<float>(MBPE.y) };
				InputSystem::ConvertGLFWToTransform(p_window, mouseX, mouseY);
				mouseX = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).x;
				mouseY = Graphics::CameraManager::GetUiWindowToScreenPosition(mouseX, mouseY).y;

				if (!IsInBound(static_cast<int>(mouseX), static_cast<int>(mouseY), transform))
					continue;

				slider.m_clicked = true;
			}
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
			if (!EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isActive || !EntityManager::GetInstance().Get<EntityDescriptor>(objectID).isAlive || !IsChildedToCanvas(objectID))
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

				if (EntityManager::GetInstance().Get<EntityDescriptor>(objectID).children.empty())
					continue;

				//get the components
				GUISlider& slider = EntityManager::GetInstance().Get <GUISlider>(objectID);

				if (EntityManager::GetInstance().Has<Transform>(slider.m_knobID.value()))
				{
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
	}

	void GUISystem::ButtonFunctionOne(EntityID)
	{
		std::cout << "function 1" << std::endl;

		if (EntityManager::GetInstance().Has<EntityDescriptor>(1))
		{
			EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(1);
			desc.isActive = !desc.isActive;
		}
		
	}

	void GUISystem::ButtonFunctionTwo(EntityID)
	{
		std::cout << "hi im function 2" << std::endl;
	}
	void GUISystem::AddFunction(std::string_view str, const std::function<void(EntityID)>& func)
	{
		m_uiFunc[str] = func;
	}


	// Serialize GUIButton
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

	// Deserialize GUIButton
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

	float GUISlider::CalculateKnobValue(float currentX)
	{
		//current value will be 
		//current x transform / total transform * (max value - min value)
		m_currentValue = ( (currentX+((m_endPoint - m_startPoint)/2) ) / (m_endPoint - m_startPoint) ) * (m_maxValue - m_minValue) + m_minValue;
		return m_currentValue;
	}

	float GUISlider::CalculateKnobCenter(float m_currentValue)
	{
		//current value will be 
		//current x transform / total transform * (max value - min value)	
		float totalVal = m_maxValue - m_minValue;
		float totalDistance = m_endPoint - m_startPoint;
		float currentTransform = (m_currentValue - m_minValue) / totalVal * totalDistance;
		m_currentXpos = m_startPoint + currentTransform;

		return m_currentXpos;
	}


	// Serialize GUISlider
	nlohmann::json GUISlider::ToJson(size_t) const
	{
		nlohmann::json j;

		j["m_Hovered"] = m_Hovered;
		j["m_disabled"] = m_disabled;
		j["m_clicked"] = m_clicked;
		j["m_startPoint"] = m_startPoint;
		j["m_endPoint"] = m_endPoint;
		j["m_currentValue"] = m_currentValue;
		j["m_minValue"] = m_minValue;
		j["m_maxValue"] = m_maxValue;

		j["m_defaultColor"] = { m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, m_defaultColor.w };
		j["m_hoveredColor"] = { m_hoveredColor.x, m_hoveredColor.y, m_hoveredColor.z, m_hoveredColor.w };
		j["m_pressedColor"] = { m_pressedColor.x, m_pressedColor.y, m_pressedColor.z, m_pressedColor.w };
		j["m_disabledColor"] = { m_disabledColor.x, m_disabledColor.y, m_disabledColor.z, m_disabledColor.w };

		j["m_defaultTexture"] = m_defaultTexture;
		j["m_hoveredTexture"] = m_hoveredTexture;
		j["m_pressedTexture"] = m_pressedTexture;
		j["m_disabledTexture"] = m_disabledTexture;

		j["m_knobID"] = m_knobID.value_or(0);

		return j;
	}

	// Deserialize GUISlider
	GUISlider GUISlider::Deserialize(const nlohmann::json& j)
	{
		GUISlider slider;

		// GUISlider Properties
		if (j.contains("m_Hovered")) slider.m_Hovered = j.at("m_Hovered").get<bool>();
		if (j.contains("m_disabled")) slider.m_disabled = j.at("m_disabled").get<bool>();
		if (j.contains("m_clicked")) slider.m_clicked = j.at("m_clicked").get<bool>();
		if (j.contains("m_startPoint")) slider.m_startPoint = j.at("m_startPoint").get<float>();
		if (j.contains("m_endPoint")) slider.m_endPoint = j.at("m_endPoint").get<float>();
		if (j.contains("m_currentValue")) slider.m_currentValue = j.at("m_currentValue").get<float>();
		if (j.contains("m_minValue")) slider.m_minValue = j.at("m_minValue").get<float>();
		if (j.contains("m_maxValue")) slider.m_maxValue = j.at("m_maxValue").get<float>();

		// Deserialize color vectors
		if (j.contains("m_defaultColor"))
		{
			auto& col = j["m_defaultColor"];
			slider.m_defaultColor = { col[0], col[1], col[2], col[3] };
		}
		if (j.contains("m_hoveredColor"))
		{
			auto& col = j["m_hoveredColor"];
			slider.m_hoveredColor = { col[0], col[1], col[2], col[3] };
		}
		if (j.contains("m_pressedColor"))
		{
			auto& col = j["m_pressedColor"];
			slider.m_pressedColor = { col[0], col[1], col[2], col[3] };
		}
		if (j.contains("m_disabledColor"))
		{
			auto& col = j["m_disabledColor"];
			slider.m_disabledColor = { col[0], col[1], col[2], col[3] };
		}

		// Deserialize string textures
		if (j.contains("m_defaultTexture")) slider.m_defaultTexture = j.at("m_defaultTexture").get<std::string>();
		if (j.contains("m_hoveredTexture")) slider.m_hoveredTexture = j.at("m_hoveredTexture").get<std::string>();
		if (j.contains("m_pressedTexture")) slider.m_pressedTexture = j.at("m_pressedTexture").get<std::string>();
		if (j.contains("m_disabledTexture")) slider.m_disabledTexture = j.at("m_disabledTexture").get<std::string>();

		if (j.contains("m_knobID"))
		{
			if (j["m_knobID"].is_null())
			{
				slider.m_knobID.reset();
			}
			else
			{
				slider.m_knobID = j.at("m_knobID").get<EntityID>();
			}
		}

		return slider;
	}


}