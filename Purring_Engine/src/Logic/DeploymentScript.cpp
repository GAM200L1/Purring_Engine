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
		m_ScriptData[id].keyEventHandlerId = ADD_MOUSE_EVENT_LISTENER(MouseEvents::MouseButtonPressed, DeploymentScript::OnMouseClick, this);

		m_currentDeploymentScriptEntityID = id;
		m_catPlaced = 0;
	}

	void DeploymentScript::Update(EntityID id, float)
	{
		GameStateController_v2_0* gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		if (m_catPlaced >= 2 && gsc->currentState == GameStates_v2_0::DEPLOYMENT)
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_ScriptData[id].DeploymentArea))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_ScriptData[id].DeploymentArea).SetColor(1, 1, 1, 0);

			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_ScriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].FollowingTextureObject).isActive = false;

			gsc->currentState = GameStates_v2_0::PLANNING;
			gsc->prevState = GameStates_v2_0::DEPLOYMENT;

			return;
		}

		if (gsc->currentState != GameStates_v2_0::DEPLOYMENT)
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_ScriptData[id].DeploymentArea))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_ScriptData[id].DeploymentArea).SetColor(1,1, 1, 0);

			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_ScriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].FollowingTextureObject).isActive = false;

			m_inNoGoArea = true;

			return;
		}

		if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_ScriptData[id].DeploymentArea))
			EntityManager::GetInstance().Get<Graphics::Renderer>(m_ScriptData[id].DeploymentArea).SetColor(1,1,1,1);

		if (EntityManager::GetInstance().Has<EntityDescriptor>(m_ScriptData[id].FollowingTextureObject))
			EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].FollowingTextureObject).isActive = true;

		GetMouseCurrentPosition(m_mousepos);

		if (EntityManager::GetInstance().Has<Transform>(m_ScriptData[id].DeploymentArea))
			m_deploymentZone = EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].DeploymentArea);
		else
			return;

		if (EntityManager::GetInstance().Has<Transform>(m_ScriptData[id].FollowingTextureObject))
			EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingTextureObject).position = m_mousepos;
		else
			return;

		//for setting texture, check what cat is is rn and set it as texture.
			/*if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_ScriptData[id].FollowingTextureObject))
			EntityManager::GetInstance().Get<Graphics::Renderer>(m_ScriptData[id].FollowingTextureObject).SetTextureKey(ResourceManager::GetInstance().LoadTexture());*/
		
		CircleCollider cc;

		if (EntityManager::GetInstance().Has<Collider>(m_ScriptData[id].FollowingTextureObject))
			cc = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(m_ScriptData[id].FollowingTextureObject).colliderVariant);
		else
			return;

		if (CheckArea(m_deploymentZone, m_mousepos, cc.radius))
		{
			for (auto cid : EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].NoGoArea).children)
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
			for (auto cid : m_ScriptData[id].AddedCats)
			{
				if (EntityManager::GetInstance().Has<EntityDescriptor>(cid))
				{
					if (!EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].FollowingTextureObject).isActive)
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
			std::cout<<"not in deployment area"<<std::endl;
			m_inNoGoArea = true;
		}

		if(m_inNoGoArea)
		{ 
			std::cout << "in no go area" << std::endl;
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_ScriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_ScriptData[id].FollowingTextureObject).SetColor(1,0,0,.65f);
		}
		else
		{
			if (EntityManager::GetInstance().Has<Graphics::Renderer>(m_ScriptData[id].FollowingTextureObject))
				EntityManager::GetInstance().Get<Graphics::Renderer>(m_ScriptData[id].FollowingTextureObject).SetColor(1, 1, 1, .65f);
		}


	}

	void DeploymentScript::Destroy(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
		{
			REMOVE_KEY_EVENT_LISTENER(m_ScriptData[id].keyEventHandlerId);
		}
	}

	void DeploymentScript::OnAttach(EntityID id)
	{
		m_ScriptData[id] = DeploymentScriptData();
	}

	void DeploymentScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	std::map<EntityID, DeploymentScriptData>& DeploymentScript::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance DeploymentScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	DeploymentScript::~DeploymentScript()
	{

	}

	void DeploymentScript::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE;
		MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		GameStateController_v2_0* gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		if (MBPE.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (!m_inNoGoArea && gsc->currentState == GameStates_v2_0::DEPLOYMENT)
			{
				SerializationManager sm;

				//will need to load based on next cat with its cat type when i eventually get it.
				EntityID NewCatID =  sm.LoadFromFile(("DeploymentTest_Prefab.json"));
				EntityManager::GetInstance().Get<Transform>(NewCatID).position = m_mousepos;
				EntityManager::GetInstance().Get<EntityDescriptor>(NewCatID).toSave = false;

				m_ScriptData[m_currentDeploymentScriptEntityID].AddedCats.push_back(NewCatID);
				m_catPlaced++;
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