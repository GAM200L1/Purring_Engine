/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GUISystem.h
 \date     02-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Checks for events and updates the states and calls the functions of all 
	GUI objects in the scene.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#pragma once

#include "System.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"
#include "Math/Transform.h"
#include "WindowManager.h"
#include <optional>
#define	REGISTER_UI_FUNCTION(func,namespace) GUISystem::AddFunction(#func, std::bind(&##namespace::##func, this, std::placeholders::_1)) 
#define HEX(hexcode)    hexcode/255.f // to convert colors
typedef unsigned long long EntityID;
namespace PE 
{

	class GUISystem : public System
	{
	public:
		/*!***********************************************************************************
		 \brief     Single arg constructor for the GUI system that takes in a pointer to the 
								GLFW window.

		 \param [in,out] p_glfwWindow - Pointer to the GLFW window that the GUI system is 
														rendering to.
		*************************************************************************************/
		GUISystem(GLFWwindow* p_glfwWindow);

		/*!***********************************************************************************
		 \brief     Virtual destructor for proper cleanup of derived systems.
		*************************************************************************************/
		virtual ~GUISystem();

		/*!***********************************************************************************
		 \brief     Initialize the system. Should be called once before any updates.
		*************************************************************************************/
		virtual void InitializeSystem() override;

		/*!***********************************************************************************
		 \brief     Update the system each frame.
		 \param     deltaTime Time passed since the last frame, in seconds.
		*************************************************************************************/
		virtual void UpdateSystem(float deltaTime) override;

		/*!***********************************************************************************
		 \brief     Clean up the system resources. Should be called once after all updates.
		*************************************************************************************/
		virtual void DestroySystem() override;

		/*!***********************************************************************************
		 \brief     Get the system's name, useful for debugging and identification.
		 \return    std::string The name of the system.
		*************************************************************************************/
		virtual std::string GetName() override;

		/*!***********************************************************************************
		 \brief Checks if the mouse cursor is within the bounds of any GUI objects
		 
		 \param[in,out] r_ME mouse click event details
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

		/*!***********************************************************************************
		 \brief Checks if the mouse cursor is within the bounds of any GUI objects

		 \param[in,out] r_ME mouse click event details
		*************************************************************************************/
		void OnMouseRelease(const Event<MouseEvents>& r_ME);

		/*!***********************************************************************************
		 \brief Checks if a point is within the square bounds of a transform
		 
		 \param[in,out] x - x coordinate in view space
		 \param[in,out] y - y coordinate in view space
		 \param[in,out] t - Transform to use as the bounds
		 \return true - Within the bounds
		 \return false - Not within the bounds
		*************************************************************************************/
		bool IsInBound(int x, int y, Transform t);

		/*!***********************************************************************************
		 \brief Checks if the  mouse is hovering over any buttons
		 
		 \param[in,out] r_ME mouse click event details
		*************************************************************************************/
		void OnMouseHover(const Event<MouseEvents>& r_ME);

		/*!***********************************************************************************
		 \brief Sample function to call from a button. Prints a message to the console.		 
		*************************************************************************************/
		void ButtonFunctionOne(::EntityID);

		/*!***********************************************************************************
		 \brief Sample function to call from a button. Prints a message to the console.		 
		*************************************************************************************/
		void ButtonFunctionTwo(::EntityID);

		/*!***********************************************************************************
		 \brief Adds a function to the UI element.
		 
		 \param[in,out] func - Function to add to the UI element
		*************************************************************************************/
		static void AddFunction(std::string_view, const std::function<void(EntityID)>& func);

	public:
			static std::map<std::string_view, std::function<void(EntityID)>> m_uiFunc;

	private:
			GLFWwindow* p_window{};
	};

	//enum to tell type of UI to make
	enum class UIType { Button = 0, Slider };

	struct GUIButton
	{		
		/*!***********************************************************************************
		 \brief Constructor. Does nothing 
		*************************************************************************************/
		GUIButton(){}
		/*!***********************************************************************************
		 \brief Initializes the UI element.		 
		*************************************************************************************/
		void Init() {}
		/*!***********************************************************************************
		 \brief Update the UI element.		 
		*************************************************************************************/
		void Update() {}
		/*!***********************************************************************************
		 \brief Destroy the UI element.		 
		*************************************************************************************/
		void Destroy() {}
		/*!***********************************************************************************
		 \brief On hovering over the UI element	 
		*************************************************************************************/
		void OnHover(EntityID id) 
		{
			if (m_onHovered != "")
				GUISystem::m_uiFunc[m_onHovered](id);
		}
		/*!***********************************************************************************
		 \brief On clicking the UI element	 
		*************************************************************************************/
		void OnClick(EntityID id) 
		{
			if (m_onClicked != "")
				GUISystem::m_uiFunc[m_onClicked](id);
		}
		/*!***********************************************************************************
		 \brief Destructor	 
		*************************************************************************************/
		virtual ~GUIButton() {};
	public:
		std::string m_onClicked{""};
		std::string m_onHovered{""};
		bool m_Hovered{};
		bool disabled{false};
		UIType m_UIType{0};

		vec4 m_defaultColor{ HEX(255),HEX(255) ,HEX(255),HEX(255) };
		std::string m_defaultTexture{"../Assets/Textures/Button_White_128px.png"};
		vec4 m_hoveredColor{HEX(220.f),HEX(220.f) ,HEX(220.f),HEX(255) };
		std::string m_hoveredTexture{ m_defaultTexture };
		vec4 m_pressedColor{ HEX(200),HEX(200) ,HEX(200),HEX(255) };
		std::string m_pressedTexture{ m_defaultTexture };
		vec4 m_disabledColor{ HEX(100),HEX(100) ,HEX(100),HEX(255) };
		std::string m_disabledTexture{ m_defaultTexture };
		float m_clickedTimer{};
	public:
		/*!***********************************************************************************
		 \brief Serializes the GUIButton component data	 
		*************************************************************************************/
		virtual nlohmann::json ToJson(size_t id) const;
		/*!***********************************************************************************
		 \brief Deserializes the GUIButton component data	 
		*************************************************************************************/
		static GUIButton Deserialize(const nlohmann::json& j);

	};

	struct GUISlider
	{
		/*!***********************************************************************************
		 \brief Constructor. Does nothing
		*************************************************************************************/
		GUISlider() {}
		/*!***********************************************************************************
		 \brief Initializes the UI element.
		*************************************************************************************/
		void Init() {}
		/*!***********************************************************************************
		 \brief Update the UI element.
		*************************************************************************************/
		void Update();
		/*!***********************************************************************************
		 \brief Destroy the UI element.
		*************************************************************************************/
		void Destroy();

		/*!***********************************************************************************
		 \brief Calculate the current value of the slider based on given min and max values.
		*************************************************************************************/
		float CalculateValue(float currentX);

		/*!***********************************************************************************
		 \brief Destructor
		*************************************************************************************/
		~GUISlider() {};
	public:
		//if knob is hovered
		bool m_Hovered{};
		bool m_disabled{ false };

		//if knob is clicked to be changed by events
		bool m_clicked{ false };


		//texture for knob
		vec4 m_defaultColor{ HEX(100),HEX(100) ,HEX(100),HEX(100) };
		std::string m_defaultTexture{};
		vec4 m_hoveredColor{ HEX(90),HEX(90) ,HEX(90),HEX(255) };
		std::string m_hoveredTexture{ m_defaultTexture };
		vec4 m_pressedColor{ HEX(60),HEX(60) ,HEX(60),HEX(255) };
		std::string m_pressedTexture{ m_defaultTexture };
		vec4 m_disabledColor{ HEX(20),HEX(20) ,HEX(20),HEX(255) };
		std::string m_disabledTexture{ m_defaultTexture };

		//to be assigned on creation
		std::optional<EntityID> m_knobID;
		//calculated on update
		float m_startPoint, m_endPoint;
		float m_currentValue;

		//set on editor
		float m_minValue{ 0 }, m_maxValue{100};

		/*!***********************************************************************************
		 \brief Serializes the GUISlider component data
		*************************************************************************************/
		virtual nlohmann::json ToJson(size_t id) const;
		/*!***********************************************************************************
		 \brief Deserializes the GUISlider component data
		*************************************************************************************/
		static GUISlider Deserialize(const nlohmann::json& j);


	};
}