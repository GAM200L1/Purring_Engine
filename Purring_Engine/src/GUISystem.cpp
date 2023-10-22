#include "prpch.h"
#include "GUISystem.h"
#include "Events/EventHandler.h"
#include "ECS/EntityFactory.h"
#include "ECS/SceneView.h"
#include "Editor/Editor.h"
namespace PE
{
	GUISystem::~GUISystem()
	{
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GUISystem::OnMouseClick, this)
	}

	void GUISystem::InitializeSystem()
	{
	}

	void GUISystem::UpdateSystem(float deltaTime)
	{
		if (Editor::GetInstance().IsRunTime())
			for (EntityID objectID : SceneView<GUI>())
			{
				GUI& gui = EntityManager::GetInstance().Get<GUI>(objectID);
				gui.Update();
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
		//loop through all objects check for Button Component
		if (Editor::GetInstance().IsRunTime())
			for (EntityID objectID : SceneView<Transform,GUI>())
			{
				Transform& transform = EntityManager::GetInstance().Get<Transform>(objectID);
				GUI& gui = EntityManager::GetInstance().Get<GUI>(objectID);
				//check mouse coordinate against transform here
				gui.OnClick();
			}
	}


}