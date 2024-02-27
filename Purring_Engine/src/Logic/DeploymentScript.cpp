/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     DeploymentScript.cpp
 \date     02-01-2024

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	declaration for the script to deploy cats

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "DeploymentScript.h"
#include "System.h"
#include <Graphics/CameraManager.h>
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ResourceManager/ResourceManager.h"
#include "Physics/CollisionManager.h"
#include "GameStateController_v2_0.h"
#include "LogicSystem.h"
namespace PE
{

	void PE::DeploymentScript::Init(EntityID id)
	{
		m_scriptData[id].mouseClickEventID = ADD_MOUSE_EVENT_LISTENER(MouseEvents::MouseButtonPressed, DeploymentScript::OnMouseClick, this);
		m_catController = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
		m_catPlaced = static_cast<int>(m_catController->GetDeployableCats(m_catController->mainInstance).size()-1);
		m_gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
	}

	void DeploymentScript::Update(EntityID id, float)
	{
		//std::cout << "catleft" << m_catPlaced << std::endl;
		if (m_catPlaced < 0 && m_gameStateController->currentState == GameStates_v2_0::DEPLOYMENT)
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].DeploymentArea))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].DeploymentArea).SetColor(1, 1, 1, 0);

			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].FollowingTextureObject).isActive = false;

				m_gameStateController->StartGameLoop();
				m_catController->UpdateCurrentCats(m_catController->mainInstance);
			return;
		}

		if (m_gameStateController->currentState != GameStates_v2_0::DEPLOYMENT)
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].DeploymentArea))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].DeploymentArea).SetColor(1,1, 1, 0);

			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].FollowingTextureObject).isActive = false;

			m_inNoGoArea = true;

			return;
		}

		if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].DeploymentArea))
			EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].DeploymentArea).SetColor(1,1,1,1);

		if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].FollowingTextureObject))
			EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].FollowingTextureObject).isActive = true;

		GetMouseCurrentPosition(m_mousepos);

		if (EntityManager::GetInstance().Has<Transform>(m_scriptData[id].DeploymentArea))
			m_deploymentZone = EntityManager::GetInstance().Get<Transform>(m_scriptData[id].DeploymentArea);
		else
			return;

		if (EntityManager::GetInstance().Has<Transform>(m_scriptData[id].FollowingTextureObject))
			EntityManager::GetInstance().Get<Transform>(m_scriptData[id].FollowingTextureObject).position = m_mousepos;
		else
			return;

		auto test = m_catController->GetDeployableCats(m_catController->mainInstance);

		switch (test[m_catPlaced])
		{
		case EnumCatType::MAINCAT:
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject).SetTextureKey(ResourceManager::GetInstance().LoadTexture("Cat_Meowsalot_512px.png"));
			break;
		}
		case EnumCatType::GREYCAT:
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject).SetTextureKey(ResourceManager::GetInstance().LoadTexture("Cat_Grey_512px.png"));
			break;
		}
		case EnumCatType::ORANGECAT:
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject).SetTextureKey(ResourceManager::GetInstance().LoadTexture("Cat_Orange_512px.png"));
			break;
		}
		/*case EnumCatType::FLUFFYCAT:
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject).SetTextureKey(ResourceManager::GetInstance().LoadTexture("Cat_Meowsalot_512px.png"));
			break;
		}*/
		}
		
		CircleCollider cc;

		if (EntityManager::GetInstance().Has<Collider>(m_scriptData[id].FollowingTextureObject))
			cc = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(m_scriptData[id].FollowingTextureObject).colliderVariant);
		else
			return;

		if (CheckArea(m_deploymentZone, m_mousepos, cc.radius))
		{
			for (auto cid : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].NoGoArea).children)
			{

				AABBCollider aabb;

				if (EntityManager::GetInstance().Has<Collider>(cid))
					aabb = std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(cid).colliderVariant);
				else
					continue;

				Contact contact;

				m_inNoGoArea = CollisionIntersection(cc, aabb, contact);

				if (m_inNoGoArea)
					break;
			}

			if(!m_inNoGoArea)
			for (auto& [cid,catType] : m_scriptData[id].AddedCats)
			{
				if (EntityManager::GetInstance().Has<EntityDescriptor>(cid))
				{
					if (!EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].FollowingTextureObject).isActive)
					{
						continue;
					}
				}
				CircleCollider cc2;

				if (EntityManager::GetInstance().Has<Collider>(cid))
					cc2 = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(cid).colliderVariant);
				else
					continue;

				Contact contact;

				m_inNoGoArea = CollisionIntersection(cc, cc2, contact);

				if (m_inNoGoArea)
					break;
			}
		}
		else
		{
			//std::cout<<"not in deployment area"<<std::endl;
			m_inNoGoArea = true;
		}

		if(m_inNoGoArea)
		{ 
			//std::cout << "in no go area" << std::endl;
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject).SetColor(1,0,0,.65f);
		}
		else
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_scriptData[id].FollowingTextureObject).SetColor(1, 1, 1, .65f);
		}


	}

	void DeploymentScript::Destroy(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			REMOVE_MOUSE_EVENT_LISTENER(m_scriptData[id].mouseClickEventID);
		}
	}

	void DeploymentScript::OnAttach(EntityID id)
	{
		m_scriptData[id] = DeploymentScriptData();

		m_currentDeploymentScriptEntityID = id;
	}

	void DeploymentScript::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);
	}

	std::map<EntityID, DeploymentScriptData>& DeploymentScript::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance DeploymentScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

	DeploymentScript::~DeploymentScript()
	{

	}

	void DeploymentScript::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE;
		MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);

		if (MBPE.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (!m_inNoGoArea && m_gameStateController->currentState == GameStates_v2_0::DEPLOYMENT)
			{
				SerializationManager sm;
				EntityID NewCatID{};
				auto test = m_catController->GetDeployableCats(m_catController->mainInstance);
					switch (test[m_catPlaced])
					{
						case EnumCatType::MAINCAT:
						{							//to be replaced
							NewCatID = sm.LoadFromFile(("Meowsalot_Prefab.json"));
							break;
						}
						case EnumCatType::GREYCAT:
						{							//to be replaced
							NewCatID = sm.LoadFromFile(("Grey Cat_Prefab.json"));
							break;
						}
						case EnumCatType::ORANGECAT:
						{							//to be replaced
							NewCatID = sm.LoadFromFile(("OrangeCat_Prefab.json"));
							break;
						}
						//case EnumCatType::FLUFFYCAT:
						//{
						//	//to be replaced
						//	NewCatID = sm.LoadFromFile(("DeploymentTest_Prefab.json"));
						//	break;
						//}
					}
					EntityManager::GetInstance().Get<Transform>(NewCatID).position = m_mousepos;
					EntityManager::GetInstance().Get<EntityDescriptor>(NewCatID).toSave = false;

					m_scriptData[m_currentDeploymentScriptEntityID].AddedCats.push_back(std::make_pair(NewCatID, m_catController->GetDeployableCats(m_catController->mainInstance)[m_catPlaced]));
					--m_catPlaced;
					return;
				}
			
		}
	}

	void DeploymentScript::GetMouseCurrentPosition(vec2& Output)
	{
		InputSystem::GetCursorViewportPosition(WindowManager::GetInstance().p_currWindow, Output.x, Output.y);
		Output = GETCAMERAMANAGER()->GetWindowToWorldPosition(Output.x, Output.y);
	}

	bool DeploymentScript::CheckArea(Transform const& area, vec2 const& mousePos, float textureWidth)
	{
		if (mousePos.x - textureWidth >= area.position.x - area.width / 2 && mousePos.x + textureWidth <= area.position.x + area.width / 2 &&
			mousePos.y - textureWidth >= area.position.y - area.height / 2 && mousePos.y + textureWidth  <= area.position.y + area.height / 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}




}