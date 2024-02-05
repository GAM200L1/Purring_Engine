/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FollowScript.cpp
 \date     02-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Script that allows the cats to follow after a leader. For M2 demo purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "FollowScript.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "AudioManager/AudioComponent.h"
#include "LogicSystem.h"
#include "CatScript.h"
#include "GameStateController_v2_0.h"

# define M_PI           3.14159265358979323846 // temp definition of pi, will need to discuss where shld we leave this later on

namespace PE
{
	void FollowScript::Init(EntityID)
	{

	}

	void FollowScript::Update(EntityID id, float)
	{
		if (m_ScriptData[id].IsAttaching)
		{
			for (int index = 1; index < m_ScriptData[id].NumberOfAttachers; ++index)
			{
				GameStateController_v2_0* gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
				Transform& curT = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
				if (EntityManager::GetInstance().Has<Transform>(m_ScriptData[id].ToAttach[index]))
				{
					//if (EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.find("CatScript") != EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.end())
					//{
					//	if (gsc->currentState == GameStates_v2_0::EXECUTE)
					//		return;
					//}

					Transform& toCheck = PE::EntityManager::GetInstance().Get<PE::Transform>(m_ScriptData[id].ToAttach[index]);
					if ((curT.position.x <= toCheck.position.x + toCheck.width / 2 && curT.position.x >= toCheck.position.x - toCheck.width / 2)
						&& (curT.position.y <= toCheck.position.y + toCheck.height / 2 && curT.position.y >= toCheck.position.y - toCheck.height / 2))
					{
						if (gsc->currentState == GameStates_v2_0::EXECUTE)
						{
							// Have the cat follow behind
							EntityID followIndex{ m_ScriptData[id].ToAttach[index] };
							m_ScriptData[id].FollowingObject[m_ScriptData[id].NumberOfFollower] = followIndex;
							m_ScriptData[id].ToAttach.erase(m_ScriptData[id].ToAttach.begin() + index);
							++m_ScriptData[id].NumberOfFollower;
							--m_ScriptData[id].NumberOfAttachers;

							if (EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.find("CatScript") != EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.end())
							{
								//CatScript::SetMaximumEnergyLevel(CatScript::GetMaximumEnergyLevel() + 2);
								CatScriptData* cd = GETSCRIPTDATA(CatScript, id);

								SerializationManager serializationManager;
								EntityID sound = serializationManager.LoadFromFile("AudioObject/Cat Rescue SFX_Prefab.json");
								if (EntityManager::GetInstance().Has<AudioComponent>(sound))
									EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
								EntityManager::GetInstance().RemoveEntity(sound);
								cd->catHealth = m_ScriptData[id].NumberOfFollower;


								// Flag the cat if so it knows it has been attached 
								CatScriptData* catData{ GETSCRIPTDATA(CatScript, followIndex) };
								catData->isFollowing = true;
							}
						}
					}
				}
			}

			if (m_ScriptData[id].NumberOfAttachers == 1)
			{
				m_ScriptData[id].IsAttaching = false;

			}
		}
		if (EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.find("CatScript") != EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.end())
		{
			CatScriptData* cd = GETSCRIPTDATA(CatScript, id);
			if(cd->catHealth >= 1)
			if (cd->catHealth < m_ScriptData[id].NumberOfFollower)
			{
				for (int i = cd->catHealth; i < m_ScriptData[id].NumberOfFollower; ++i)
				{
					if(EntityManager::GetInstance().Has<EntityDescriptor>(m_ScriptData[id].FollowingObject[i]))
						EntityManager::GetInstance().Get<EntityDescriptor>(m_ScriptData[id].FollowingObject[i]).isActive = false;
					m_ScriptData[id].NumberOfFollower--;
					CatScript::SetMaximumEnergyLevel(CatScript::GetBaseMaximumEnergyLevel() + (m_ScriptData[id].NumberOfFollower - 1) * 2);
				}
			}
		}
		for (int i = m_ScriptData[id].NumberOfFollower; i < 5; ++i)
		{
			m_ScriptData[id].FollowingObject[i] = static_cast<EntityID>(-1);
		}

		vec2 NewPosition = EntityManager::GetInstance().Get<Transform>(id).position;

		if (!(NewPosition.x == m_ScriptData[id].CurrentPosition.x && NewPosition.y == m_ScriptData[id].CurrentPosition.y))
		{
			//for object 1 to 2
				//to get rotation
			vec2 directionalvector = NewPosition - m_ScriptData[id].CurrentPosition;
			float newRotation = atan2(directionalvector.y, directionalvector.x);

			//setting previous position as the current position of the next cat
			vec2 savedLocation = m_ScriptData[id].NextPosition[0];

			//setting current new position for the next object
			m_ScriptData[id].NextPosition[0] = EntityManager::GetInstance().Get<Transform>(id).position;
			



			//checking rotation to set
			float rotationOffset = newRotation - m_ScriptData[id].Rotation;

			if (rotationOffset != 0 && m_ScriptData[id].LookTowardsMovement)
				EntityManager::GetInstance().Get<Transform>(id).orientation = EntityManager::GetInstance().Get<Transform>(id).orientation + rotationOffset;

			

			m_ScriptData[id].Rotation = newRotation;
			m_ScriptData[id].CurrentPosition = EntityManager::GetInstance().Get<Transform>(id).position;

			if (m_ScriptData[id].NumberOfFollower > 1)
			{
				for (int index = 1; index < m_ScriptData[id].NumberOfFollower; ++index)
				{
					//to get rotation new position - current position which we set previously
					vec2 NewPosition2 = savedLocation; //new position is the position of the previous mouse
					//calculate new rotation since previous location

					vec2 directionalvector2 = m_ScriptData[id].NextPosition[index - 1] - m_ScriptData[id].NextPosition[index];


					float newRotation2 = atan2(directionalvector2.y, directionalvector2.x);

					//saving current position as 
					savedLocation = m_ScriptData[id].NextPosition[index];
					m_ScriptData[id].NextPosition[index] = NewPosition2 + vec2(m_ScriptData[id].Size * cosf(newRotation2 - static_cast<float>(M_PI)), m_ScriptData[id].Size * sinf(newRotation2 - static_cast<float>(M_PI)));

					if (m_ScriptData[id].FollowingObject[index] != -1)
						EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingObject[index]).position = m_ScriptData[id].NextPosition[index];
					//checking rotation to set can ignore this for now lets get position to work
					if (m_ScriptData[id].FollowingObject[index] != -1)
					{
						vec2 directionalvector3 = m_ScriptData[id].NextPosition[index - 1] - m_ScriptData[id].NextPosition[index];
						float newRot = atan2(directionalvector3.y, directionalvector3.x);
						if (m_ScriptData[id].LookTowardsMovement)
							EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingObject[index]).orientation = newRot;
						else
							EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingObject[index]).width = EntityManager::GetInstance().Get<Transform>(id).width;
					}
				}

			}

			
		}

		if (InputSystem::IsKeyTriggered(GLFW_KEY_Q))
		{
			if (EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.find("CatScript") != EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.end())
			{
				CatScriptData* cd = GETSCRIPTDATA(CatScript, id);
				cd->catHealth--;
			}
		}
	}

	void FollowScript::Destroy(EntityID)
	{

	}

	void FollowScript::OnAttach(EntityID id)
	{
		m_ScriptData[id] = FollowScriptData();
		//hardcoded based on unity demo
		m_ScriptData[id].NextPosition.resize(5);
		m_ScriptData[id].FollowingObject.resize(6);
		std::fill(m_ScriptData[id].FollowingObject.begin(), m_ScriptData[id].FollowingObject.end(), static_cast<EntityID>(-1));
		m_ScriptData[id].ToAttach.resize(6);
		std::fill(m_ScriptData[id].ToAttach.begin(), m_ScriptData[id].ToAttach.end(), static_cast<EntityID>(-1));
	}

	void FollowScript::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	std::map<EntityID, FollowScriptData>& FollowScript::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance FollowScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	FollowScript::~FollowScript()
	{
	}


}