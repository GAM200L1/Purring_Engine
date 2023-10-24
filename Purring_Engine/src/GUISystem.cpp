#include "prpch.h"
#include "GUISystem.h"
#include "Events/EventHandler.h"
#include "ECS/EntityFactory.h"
#include "ECS/SceneView.h"
#include "Editor/Editor.h"
#define UI_CAST(type, ui) reinterpret_cast<type&>(ui)

std::map<std::string_view, std::function<void(void)>> PE::GUISystem::m_uiFunc;



namespace PE
{
	GUISystem::~GUISystem()
	{

	}

	void GUISystem::InitializeSystem()
	{
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GUISystem::OnMouseClick, this)
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseMoved, GUISystem::OnMouseHover, this)

		REGISTER_UI_FUNCTION(ButtonFunctionOne, GUISystem);
		REGISTER_UI_FUNCTION(ButtonFunctionTwo, GUISystem);
	}

	void GUISystem::UpdateSystem(float deltaTime)
	{
		deltaTime;
		if (Editor::GetInstance().IsRunTime())
			for (EntityID objectID : SceneView<GUI>())
			{
				GUI& gui = EntityManager::GetInstance().Get<GUI>(objectID);
				//gui.m_onClicked = "TestFunction";
				gui.Update();
				//gui.m_onClicked = m_uiFunc.at("ButtonFunctionOne").target<void()>();
				if (gui.m_UIType == UIType::Button)
				{
					Button btn = UI_CAST(Button, gui);
					if (btn.m_Hovered)
					btn.OnHover();
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
		if (Editor::GetInstance().IsRunTime())
			for (EntityID objectID : SceneView<Transform, GUI>())
			{
				//get the components
				Transform& transform = EntityManager::GetInstance().Get<Transform>(objectID);
				GUI& gui = EntityManager::GetInstance().Get<GUI>(objectID);
				
				if (!IsInBound(MBPE.x, MBPE.y, transform))
					return;

				if (gui.m_UIType == UIType::Button)
				{
					Button btn = UI_CAST(Button, gui);
					btn.OnClick();
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
		if (Editor::GetInstance().IsRunTime())
		for (EntityID objectID : SceneView<Transform, GUI>())
		{
			//get the components
			Transform& transform = EntityManager::GetInstance().Get<Transform>(objectID);
			GUI& gui = EntityManager::GetInstance().Get<GUI>(objectID);

			//check mouse coordinate against transform here
			if (IsInBound(MME.x, MME.y, transform))
			{
				gui.m_Hovered = true;
			}
			else
			{
				gui.m_Hovered = false;
			}
		}
	}

	void GUISystem::ButtonFunctionOne()
	{
		std::cout << "hi im function 1" << std::endl;
	}

	void GUISystem::ButtonFunctionTwo()
	{
		std::cout << "hi im function 2" << std::endl;
	}
	void GUISystem::AddFunction(std::string_view str, const std::function<void(void)>& func)
	{
		m_uiFunc[str] = func;
	}
}