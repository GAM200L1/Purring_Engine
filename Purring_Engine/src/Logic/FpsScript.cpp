/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     FpsScript.cpp 
 \date     7-2-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu


 \brief  This file contains the definitions of FpsScript that displays the FPS of the
		 game through a text object.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "FpsScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Time/TimeManager.h"

namespace PE 
{
	void FpsScript::Init(EntityID id)
	{

	}

	void FpsScript::Update(EntityID id, float deltaTime)
	{
		m_elapsedTime += deltaTime;

		if (m_elapsedTime >= 1.f)
		{
			m_elapsedTime = 0.f;
			if (EntityManager::GetInstance().Has<TextComponent>(id))
			{
				std::string fpsString = "FPS: " + std::to_string(static_cast<int>(TimeManager::GetInstance().m_frameRateController.GetFps()));
				EntityManager::GetInstance().Get<TextComponent>(id).SetText(fpsString);
			}
		}
	}
	void FpsScript::Destroy(EntityID id)
	{
		
	}

	void FpsScript::OnAttach(EntityID id)
	{

	}

	void FpsScript::OnDetach(EntityID id)
	{

	}

	rttr::instance FpsScript::GetScriptData(EntityID id)
	{
		return rttr::instance();
	}
}