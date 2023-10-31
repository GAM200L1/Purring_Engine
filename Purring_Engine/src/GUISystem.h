#pragma once

#include "System.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"
#include "Math/Transform.h"
#include "WindowManager.h"

#define	REGISTER_UI_FUNCTION(func,namespace) GUISystem::AddFunction(#func, std::bind(&##namespace::##func, this))
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

		void OnMouseClick(const Event<MouseEvents>& r_ME);

		// assumes that all the coordinates are in view space
		bool IsInBound(int x, int y, Transform t);

		void OnMouseHover(const Event<MouseEvents>& r_ME);

		void ButtonFunctionOne();

		void ButtonFunctionTwo();

		static void AddFunction(std::string_view, const std::function<void(void)>& func);

	public:
			static std::map<std::string_view, std::function<void(void)>> m_uiFunc;

	private:
			GLFWwindow* p_window{};
	};

	enum class UIType { Button = 0, TextBox };

	struct GUI
	{
		//enum to tell type of UI to make
		GUI() {}
		virtual void Init() {}
		virtual void Update() {}
		virtual void Destroy() {}
		virtual void OnHover() {}
		virtual void OnClick() {}
		virtual ~GUI() {};

		std::string m_onClicked{""};
		std::string m_onHovered{""};
		bool m_Hovered{};
		UIType m_UIType{0};

	public:
		virtual nlohmann::json ToJson() const;
		static GUI Deserialize(const nlohmann::json& j);
	};


	struct Button : public GUI
	{
		virtual void Init() override {}
		virtual void Update() override {}
		virtual void Destroy() override {}
		//use this for now idk how are we gonna bind functions later
		inline virtual void OnHover() override 
		{
			if (m_onHovered != "")
				GUISystem::m_uiFunc[m_onHovered]();
		}
		inline virtual void OnClick() override
		{
			if (m_onClicked != "")
				GUISystem::m_uiFunc[m_onClicked]();
		}
		virtual ~Button() {};

	};
}