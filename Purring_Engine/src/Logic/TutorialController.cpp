/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     testScript.cpp
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definitions of testScript

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "TutorialController.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Animation/Animation.h"
#include "LogicSystem.h"


#include <limits>


# define M_PI           3.14159265358979323846 

namespace PE
{
	TutorialController::TutorialController()
	{
	}

	void TutorialController::Init(EntityID)
	{
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		p_rc = GETSCRIPTINSTANCEPOINTER(RatController_v2_0);
	}

	void TutorialController::Update(EntityID id, float)
	{
		if (p_gsc->currentState == GameStates_v2_0::PLANNING)
		{
			if (p_gsc->currentTurn == 0)
			{
				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel1))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel1).isActive = true;

				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel2))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel2).isActive = false;

				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3).isActive = false;
			}
			else if (p_gsc->currentTurn == 1)
			{
				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel2))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel2).isActive = true;

				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel1))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel1).isActive = false;

				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3).isActive = false;
			}
			else if (p_gsc->currentTurn >= 5)
			{
				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3).isActive = true;

				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel1))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel1).isActive = false;

				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel2))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel2).isActive = false;
			}

			if (p_rc->GetRats(p_rc->mainInstance).empty())
			{
				if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3))
					EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3).isActive = true;
			}

		}
		else
		{
			if(EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel1))
				EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel1).isActive = false;

			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel2))
				EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel2).isActive = false;

			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3))
				EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData.at(id).TutorialPanel3).isActive = false;
		}


	}

	void TutorialController::Destroy(EntityID)
	{
	}

	TutorialController::~TutorialController()
	{
	}

	void TutorialController::OnAttach(EntityID id)
	{
		m_scriptData[id] = TutorialControllerData();
	}

	void TutorialController::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);
	}

	std::map<EntityID, TutorialControllerData>& TutorialController::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance TutorialController::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

}