#include "prpch.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "AudioManager/AudioComponent.h"

#include "FollowScript_v2_0.h"
#include "../LogicSystem.h"
#include "../GameStateController_v2_0.h"
#include "CatScript_v2_0.h"

# define M_PI           3.14159265358979323846 // temp definition of pi, will need to discuss where shld we leave this later on

namespace PE
{
	void FollowScript_v2_0::Init(EntityID)
	{
		p_gamestateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, FollowScript_v2_0::CollisionCheck, this);

	}

	void FollowScript_v2_0::Update(EntityID id, float)
	{
		if (m_ScriptData[id].IsAttaching)
		{
			if (p_gamestateController->currentState == GameStates_v2_0::EXECUTE)
			for (const auto& flw : m_ScriptData[id].ToAttach)
			{
				if (m_ScriptData[id].followers.size() == 4)
					continue;

				if (EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.find("CatScript_v2_0") != EntityManager::GetInstance().Get<ScriptComponent>(id).m_scriptKeys.end())
				{
					if (std::find(m_ScriptData[id].followers.begin(), m_ScriptData[id].followers.end(), flw) == m_ScriptData[id].followers.end())
						m_ScriptData[id].followers.emplace_back(flw);
					SerializationManager serializationManager;
					EntityID sound = serializationManager.LoadFromFile("AudioObject/Cat Rescue SFX_Prefab.json");
					if (EntityManager::GetInstance().Has<AudioComponent>(sound))
						EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
					EntityManager::GetInstance().RemoveEntity(sound);


					// Flag the cat if so it knows it has been attached 
					CatScript_v2_0Data* catData{ GETSCRIPTDATA(CatScript_v2_0, flw) };
					catData->isCaged = false;
				}
			}
			m_ScriptData[id].ToAttach.clear();
			m_ScriptData[id].IsAttaching = false;
		}

		/*if (p_gamestateController->currentState != GameStates_v2_0::EXECUTE)
			return;*/

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

			//if (m_ScriptData[id].NumberOfFollower > 1)
			{
				//for (int index = 1; index < m_ScriptData[id].NumberOfFollower; ++index)
				//{
				//	//to get rotation new position - current position which we set previously
				//	vec2 NewPosition2 = savedLocation; //new position is the position of the previous mouse
				//	//calculate new rotation since previous location

				//	vec2 directionalvector2 = m_ScriptData[id].NextPosition[index - 1] - m_ScriptData[id].NextPosition[index];


				//	float newRotation2 = atan2(directionalvector2.y, directionalvector2.x);

				//	//saving current position as 
				//	savedLocation = m_ScriptData[id].NextPosition[index];
				//	m_ScriptData[id].NextPosition[index] = NewPosition2 + vec2(m_ScriptData[id].Size * cosf(newRotation2 - static_cast<float>(M_PI)), m_ScriptData[id].Size * sinf(newRotation2 - static_cast<float>(M_PI)));

				//	if (m_ScriptData[id].FollowingObject[index] != -1)
				//		EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingObject[index]).position = m_ScriptData[id].NextPosition[index];
				//	//checking rotation to set can ignore this for now lets get position to work
				//	if (m_ScriptData[id].FollowingObject[index] != -1)
				//	{
				//		vec2 directionalvector3 = m_ScriptData[id].NextPosition[index - 1] - m_ScriptData[id].NextPosition[index];
				//		float newRot = atan2(directionalvector3.y, directionalvector3.x);
				//		if (m_ScriptData[id].LookTowardsMovement)
				//			EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingObject[index]).orientation = newRot;
				//		else
				//			EntityManager::GetInstance().Get<Transform>(m_ScriptData[id].FollowingObject[index]).width = EntityManager::GetInstance().Get<Transform>(id).width;
				//	}
				//}

				int index{ 1 };

				for (auto follower : m_ScriptData[id].followers)
				{
					//to get rotation new position - current position which we set previously
					vec2 NewPosition2 = savedLocation; //new position is the position of the previous mouse
					//calculate new rotation since previous location

					vec2 directionalvector2 = m_ScriptData[id].NextPosition[index - 1] - m_ScriptData[id].NextPosition[index];


					float newRotation2 = atan2(directionalvector2.y, directionalvector2.x);

					//saving current position as 
					savedLocation = m_ScriptData[id].NextPosition[index];
					m_ScriptData[id].NextPosition[index] = NewPosition2 + vec2(m_ScriptData[id].Size * cosf(newRotation2 - static_cast<float>(M_PI)), m_ScriptData[id].Size * sinf(newRotation2 - static_cast<float>(M_PI)));

					
					EntityManager::GetInstance().Get<Transform>(follower).position = m_ScriptData[id].NextPosition[index];
					++index;
					//checking rotation to set can ignore this for now lets get position to work
					
						vec2 directionalvector3 = m_ScriptData[id].NextPosition[index - 1] - m_ScriptData[id].NextPosition[index];
						float newRot = atan2(directionalvector3.y, directionalvector3.x);
						if (m_ScriptData[id].LookTowardsMovement)
							EntityManager::GetInstance().Get<Transform>(follower).orientation = newRot;
						else
							EntityManager::GetInstance().Get<Transform>(follower).width = EntityManager::GetInstance().Get<Transform>(id).width;
					
				}
			}
		}
	}

	void FollowScript_v2_0::Destroy(EntityID)
	{

	}

	void FollowScript_v2_0::OnAttach(EntityID id)
	{
		m_ScriptData[id] = FollowScriptData_v2_0();
		//hardcoded based on unity demo
		m_ScriptData[id].NextPosition.resize(10);
		//m_ScriptData[id].FollowingObject.resize(6);
		// emplace a dummy object
		//m_ScriptData[id].folowers.push(MAXSIZE_T);
		//std::fill(m_ScriptData[id].FollowingObject.begin(), m_ScriptData[id].FollowingObject.end(), static_cast<EntityID>(-1));
		//m_ScriptData[id].ToAttach.resize(6);
		//std::fill(m_ScriptData[id].ToAttach.begin(), m_ScriptData[id].ToAttach.end(), static_cast<EntityID>(-1));
	}

	void FollowScript_v2_0::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	void FollowScript_v2_0::Adopt(EntityID owner, EntityID adopt)
	{
		m_ScriptData[owner].IsAttaching = true;
		m_ScriptData[owner].ToAttach.emplace_back(adopt);
	}

	void FollowScript_v2_0::CollisionCheck(const Event<CollisionEvents>& r_event)
	{
		if (r_event.GetType() == CollisionEvents::OnTriggerEnter)
		{
			OnTriggerEnterEvent event = static_cast<const OnTriggerEnterEvent&>(r_event);
			EntityID id1{ event.Entity1 }, id2{ event.Entity2 };

			// unable to find the follow script on id1
			if (!m_ScriptData.count(id1))
			{
				// unable to find the follow script on id2
				if (!m_ScriptData.count(id2))
					return; // return, none of the entities in this event were relevant
				
				// entity id of id2 is the follow script holder, swap values so can reuse code
				std::swap(id1, id2);
			}

			if (EntityManager::GetInstance().Has<ScriptComponent>(id2))
			{
				try
				{
					auto data = GETSCRIPTDATA(CatScript_v2_0, id2);
					if (data->isCaged)
					{
						m_ScriptData[id1].IsAttaching = true;
						m_ScriptData[id1].ToAttach.emplace_back(id2);
					}
				}
				catch (...)
				{
					// failed to find the cat script on the target
				}
			}
			
		}
	}


	std::map<EntityID, FollowScriptData_v2_0>& FollowScript_v2_0::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance FollowScript_v2_0::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	FollowScript_v2_0::~FollowScript_v2_0()
	{
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
	}
}