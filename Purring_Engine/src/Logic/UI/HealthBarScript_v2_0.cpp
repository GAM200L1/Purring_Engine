/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     HealthBarScript_v2_0.cpp
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions to update the position and visuals of the health bar UI.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "HealthBarScript_v2_0.h"
#include "../LogicSystem.h"
#include "../ECS/Entity.h"
#include "../ECS/EntityFactory.h"
#include "../Graphics/CameraManager.h"
//#include "../Rat/RatScript_v2_0.h"
#include "../RatScript.h"

namespace PE
{
	// ---------- FUNCTION DEFINITIONS ---------- //

	void HealthBarScript_v2_0::Init(EntityID id)
	{
		// Reset the values
		SetFillAmount(id, 1.f);
	}

	void HealthBarScript_v2_0::Update(EntityID id, float deltaTime)
	{
		// Get the transform of the object we're supposed to be following
		if (EntityManager::GetInstance().Has<Transform>(m_scriptData[id].followObjectID))
		{
			// Update the position of the healthbar
			vec2 newPosition{ EntityManager::GetInstance().Get<Transform>(m_scriptData[id].followObjectID).position }; 
			newPosition = GETCAMERAMANAGER()->GetWorldToWindowPosition(newPosition.x, newPosition.y);
			newPosition = GETCAMERAMANAGER()->GetUiWindowToScreenPosition(newPosition.x, newPosition.y + 32);
			PositionEntityRelative(id, newPosition);
		}

		if (EntityManager::GetInstance().Has<GUISlider>(id))
		{
			if (EntityManager::GetInstance().Has<ScriptComponent>(m_scriptData[id].followObjectID))
			{
				EntityManager::GetInstance().Get<GUISlider>(id).m_currentValue = static_cast<float>(*GETSCRIPTDATA(RatScript, m_scriptData[id].followObjectID).health);
				EntityManager::GetInstance().Get<GUISlider>(id).m_maxValue = 3;
			}
		}
	}

	void HealthBarScript_v2_0::OnAttach(EntityID id)
	{
		//// Add gui slider component
		//if (!EntityManager::GetInstance().Has<GUISlider>(id))
		//{
		//	EntityFactory::GetInstance().Assign(id, {EntityManager::GetInstance().GetComponentID<GUISlider>()});
		//	EntityManager::GetInstance().Get<GUISlider>(id).m_isHealthBar = true;
		//}
		//else
		//{
		//	EntityManager::GetInstance().Get<GUISlider>(id).m_isHealthBar = true;
		//}

		// Create script instance data
		m_scriptData[id] = HealthBarScript_v2_0_Data();
		m_scriptData[id].myID = id;
	}

	void HealthBarScript_v2_0::OnDetach(EntityID id)
	{
		// Delete this instance's script data
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			m_scriptData.erase(id);
		}
	}

	std::map<EntityID, HealthBarScript_v2_0_Data> &HealthBarScript_v2_0::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance HealthBarScript_v2_0::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

	float HealthBarScript_v2_0::GetFillAmount(EntityID id) const
	{
		// Get the GUI slider component
		if (EntityManager::GetInstance().Has<GUISlider>(id))
		{
			return EntityManager::GetInstance().Get<GUISlider>(id).m_currentValue;
		}
		return 0.f;
	}

	void HealthBarScript_v2_0::SetFillAmount(EntityID id, float const fillAmount)
	{
		// Get the GUI slider component
		if (EntityManager::GetInstance().Has<GUISlider>(id))
		{
			// set the healthbar fill
			GUISlider& healthBar{ EntityManager::GetInstance().Get<GUISlider>(id) };
			healthBar.m_currentValue = fillAmount;

			// set the color of the healthbar according to the fill amount 
			if(healthBar.m_knobID.has_value())
			{
				EntityID fillId{ healthBar.m_knobID.value() };
				if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id))
				{
					vec4 fillColor{ *(GETSCRIPTDATA(HealthBarScript_v2_0, id).fillColorFull) };

					if (fillAmount < 0.333f) // only a third of the health is left
					{
							fillColor = *(GETSCRIPTDATA(HealthBarScript_v2_0, id).fillColorAlmostEmpty);
					}
					else if (fillAmount < 0.666f) // two thirds of the health is left
					{
							fillColor = *(GETSCRIPTDATA(HealthBarScript_v2_0, id).fillColorHalf);
					}

					if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(fillId))
					{
							EntityManager::GetInstance().Get<Graphics::GUIRenderer>(fillId).SetColor(fillColor.x, fillColor.y, fillColor.z, fillColor.w);
					}
				}
			}
		}
	} // end of HealthBarScript_v2_0::SetFillAmount

	void HealthBarScript_v2_0::ToggleEntity(EntityID id, bool setToActive)
	{
		// Exit if the entity is not valid
		if (!EntityManager::GetInstance().IsEntityValid(id))
		{
			return;
		}

		// Toggle the entity
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
	}

	void HealthBarScript_v2_0::PositionEntity(EntityID const transformId, vec2 const &r_position)
	{
		try
		{
			Transform &r_transform{EntityManager::GetInstance().Get<Transform>(transformId)}; // Get the transform of the player
			r_transform.position = r_position;
		}
		catch (...)
		{
			return;
		}
	}

	void HealthBarScript_v2_0::PositionEntityRelative(EntityID const transformId, vec2 const &r_position)
	{
		try
		{
			Transform &r_transform{EntityManager::GetInstance().Get<Transform>(transformId)}; // Get the transform of the player
			r_transform.relPosition = r_position;
		}
		catch (...)
		{
			return;
		}
	}

} // End of namespace PE