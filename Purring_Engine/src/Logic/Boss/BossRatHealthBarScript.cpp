/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     BossRatHealthBarScript.cpp 
 \date     03-11-2024

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definitions of BossRatHealthBarScript

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "BossRatHealthBarScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Logic/LogicSystem.h"
#include "GUISystem.h"

namespace PE 
{
	void BossRatHealthBarScript::Init(EntityID)
	{
		p_bsr = GETSCRIPTINSTANCEPOINTER(BossRatScript);
	}
	void BossRatHealthBarScript::Update(EntityID id, float deltaTime)
	{
		if (EntityManager::GetInstance().Has<GUISlider>(id))
		{
				GUISlider& healthBar{ EntityManager::GetInstance().Get<GUISlider>(id) };
				healthBar.m_currentValue = p_bsr->m_scriptData[p_bsr->currentBoss].currenthealth;
				healthBar.m_maxValue = p_bsr->m_scriptData[p_bsr->currentBoss].maxHealth;

				SetFillAmount(id, (healthBar.m_currentValue <= 0.f || healthBar.m_maxValue == 0.f) ? 0.f : healthBar.m_currentValue / healthBar.m_maxValue);
		}
	}
	void BossRatHealthBarScript::Destroy(EntityID )
	{
	}

	BossRatHealthBarScript::~BossRatHealthBarScript()
	{
	}

	float BossRatHealthBarScript::GetFillAmount(EntityID id) const
	{
		// Get the GUI slider component
		if (EntityManager::GetInstance().Has<GUISlider>(id))
		{
			return EntityManager::GetInstance().Get<GUISlider>(id).m_currentValue;
		}
		return 0.f;
	}

	void BossRatHealthBarScript::SetFillAmount(EntityID id, float const fillAmount)
	{
		// Get the GUI slider component
		if (EntityManager::GetInstance().Has<GUISlider>(id))
		{
			// set the healthbar fill
			GUISlider& healthBar{ EntityManager::GetInstance().Get<GUISlider>(id) };

			// set the color of the healthbar according to the fill amount 
			if (healthBar.m_knobID.has_value())
			{
				EntityID fillId{ healthBar.m_knobID.value() };
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id))
				{
					vec4 fillColor{ *(GETSCRIPTDATA(BossRatHealthBarScript, id).fillColorFull) };

					if (fillAmount < 0.333f) // only a third of the health is left
					{
						fillColor = *(GETSCRIPTDATA(BossRatHealthBarScript, id).fillColorAlmostEmpty);
					}
					else if (fillAmount < 0.666f) // two thirds of the health is left
					{
						fillColor = *(GETSCRIPTDATA(BossRatHealthBarScript, id).fillColorHalf);
					}

					if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(fillId))
					{
						EntityManager::GetInstance().Get<Graphics::GUIRenderer>(fillId).SetColor(fillColor.x, fillColor.y, fillColor.z, fillColor.w);
					}
				}
			}
		}
	} // end of HealthBarScript_v2_0::SetFillAmount

	void BossRatHealthBarScript::OnAttach(EntityID id)
	{
		m_scriptData[id] = BossRatHealthBarScriptData();
	}

	void BossRatHealthBarScript::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);
	}

	std::map<EntityID, BossRatHealthBarScriptData>& BossRatHealthBarScript::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance BossRatHealthBarScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

}