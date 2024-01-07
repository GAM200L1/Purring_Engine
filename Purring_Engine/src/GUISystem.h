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
#include <unordered_set>  // this is in the precompiled header but the compiler was complaining

#define HEX(hexcode)    hexcode/255.f // to convert colors
#define	REGISTER_UI_FUNCTION(func,namespace) GUISystem::AddFunction(#func, std::bind(&##namespace::##func, this, std::placeholders::_1))
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
		 \param [in] width - Target width of the canvas.
		 \param [in] height - Target height of the canvas.
		*************************************************************************************/
		GUISystem(GLFWwindow* p_glfwWindow, float const width, float const height);

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
		 \brief     Returns the container of canvases that are active in the scene.
		 \return		std::unordered_set<EntityID> - Returns the container of canvases that 
										are active in the scene.
		*************************************************************************************/
		static inline auto const& GetActiveCanvases() { return m_activeCanvases; }

		/*!***********************************************************************************
		 \brief     Returns true if there are any active canvases in the scene, false otherwise.
		 \return		bool - Returns true if there are any active canvases in the scene, 
										false otherwise.
		*************************************************************************************/
		inline bool AreThereActiveCanvases() const { return !m_activeCanvases.empty(); }

		/*!***********************************************************************************
		 \brief     Returns true if the object with the ID passed in is childed to an 
								active canvas in the scene, false otherwise.
		 \param[in] uiId - ID of the UI object to check.
		 \return		bool - Returns true if the object with the ID passed in is childed to an 
										active canvas in the scene, false otherwise.
		*************************************************************************************/
		bool IsChildedToCanvas(EntityID uiId) const;

		/*!***********************************************************************************
		 \brief     Returns true if the object with the ID passed in is the immediate child 
								to an active canvas in the scene, false otherwise.
		 \param[in] uiId - ID of the UI object to check.
		 \return		bool - Returns true if the object with the ID passed in is the immediate 
										child to an active canvas in the scene, false otherwise.
		*************************************************************************************/
		bool IsImmediatelyChildedToCanvas(EntityID const uiId) const;

		/*!***********************************************************************************
		 \brief Checks if the mouse cursor is within the bounds of any GUI objects
		 
		 \param[in,out] r_ME mouse click event details
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

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

			// Stores the IDs of the active canvases
			static std::unordered_set<EntityID> m_activeCanvases;
			static float m_targetResolutionWidth, m_targetResolutionHeight; // Dimensions the canvases should have
	};

	//enum to tell type of UI to make
	enum class UIType { Button = 0, TextBox };

	struct GUI
	{		
		/*!***********************************************************************************
		 \brief Constructor. Does nothing 
		*************************************************************************************/
		GUI(){}
		/*!***********************************************************************************
		 \brief Initializes the UI element.		 
		*************************************************************************************/
		virtual void Init() {}
		/*!***********************************************************************************
		 \brief Update the UI element.		 
		*************************************************************************************/
		virtual void Update() {}
		/*!***********************************************************************************
		 \brief Destroy the UI element.		 
		*************************************************************************************/
		virtual void Destroy() {}
		/*!***********************************************************************************
		 \brief On hovering over the UI element	 
		*************************************************************************************/
		virtual void OnHover(EntityID) {}
		/*!***********************************************************************************
		 \brief On clicking the UI element	 
		*************************************************************************************/
		virtual void OnClick(EntityID) {}
		/*!***********************************************************************************
		 \brief Destructor	 
		*************************************************************************************/
		virtual ~GUI() {};
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
		 \brief Serializes the UI element data	 
		*************************************************************************************/
		virtual nlohmann::json ToJson(size_t id) const;
		/*!***********************************************************************************
		 \brief Deserializes the UI element data	 
		*************************************************************************************/
		static GUI Deserialize(const nlohmann::json& j);


	};


	struct Button : public GUI
	{
		/*!***********************************************************************************
		 \brief Serializes the UI element data	 
		*************************************************************************************/
		virtual void Init() override {}
		/*!***********************************************************************************
		 \brief Update the button		 
		*************************************************************************************/
		virtual void Update() override {}
		/*!***********************************************************************************
		 \brief Destory the button		 
		*************************************************************************************/
		virtual void Destroy() override {}
		/*!***********************************************************************************
		 \brief Calls the onhover function 
		*************************************************************************************/
		inline virtual void OnHover(EntityID id) override
		{
			if (m_onHovered != "")
				GUISystem::m_uiFunc[m_onHovered](id);
		}
		/*!***********************************************************************************
		 \brief Calls the onClick function 
		*************************************************************************************/
		inline virtual void OnClick(EntityID id) override
		{
			if (m_onClicked != "")
				GUISystem::m_uiFunc[m_onClicked](id);
		}
		/*!***********************************************************************************
		 \brief Does nothing
		*************************************************************************************/
		virtual ~Button() {};

	};
}