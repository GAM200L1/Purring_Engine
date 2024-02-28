/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     FollowScript_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the definitions for the functions for the Cat Planning State.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
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
#include "CatHelperFunctions.h"
#include "CatController_v2_0.h"
#include "Hierarchy/HierarchyManager.h"

# define M_PI           3.14159265358979323846 // temp definition of pi, will need to discuss where shld we leave this later on

namespace PE
{
	FollowScript_v2_0::~FollowScript_v2_0()
	{
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
	}

	void FollowScript_v2_0::Init(EntityID)
	{
		p_gamestateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);		
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, FollowScript_v2_0::CollisionCheck, this);
	}

	void FollowScript_v2_0::Update(EntityID id, float)
	{
		if (p_gamestateController->currentState == GameStates_v2_0::DEPLOYMENT) 
		{ 
			scriptData[id].prevPosition = CatHelperFunctions::GetEntityPosition(id);
			return;
		}
		
		vec2 newPosition = CatHelperFunctions::GetEntityPosition(id);
		
		if (!(newPosition.x == scriptData[id].prevPosition.x && newPosition.y == scriptData[id].prevPosition.y))
		{
			//for object 1 to 2
				//to get rotation
			//vec2 directionalvector = newPosition - scriptData[id].CurrentPosition;
			//float newRotation = atan2(directionalvector.y, directionalvector.x);

			////setting previous position as the current position of the next cat
			//vec2 savedLocation = scriptData[id].NextPosition[0];

			////setting current new position for the next object
			//scriptData[id].NextPosition[0] = EntityManager::GetInstance().Get<Transform>(id).position;

			////checking rotation to set
			//float rotationOffset = newRotation - scriptData[id].Rotation;

			//if (rotationOffset != 0 && scriptData[id].LookTowardsMovement)
			//	EntityManager::GetInstance().Get<Transform>(id).orientation = EntityManager::GetInstance().Get<Transform>(id).orientation + rotationOffset;

			//scriptData[id].Rotation = newRotation;
			//scriptData[id].current = EntityManager::GetInstance().Get<Transform>(id).position;
			
			scriptData[id].nextPosition.clear();

			// adds the current position of the main cat
			scriptData[id].nextPosition.emplace_back(newPosition);

			// adds the current positions of the following cats
			for (EntityID followerID : scriptData[id].followers)
			{
				scriptData[id].nextPosition.emplace_back(CatHelperFunctions::GetEntityPosition(followerID));
			}

			int index{ 1 };

			for (auto follower : scriptData[id].followers)
			{
				//to get rotation new position - current position which we set previously
				//vec2 NewPosition2 = savedLocation; //new position is the position of the previous mouse
				//calculate new rotation since previous location

				vec2 directionalvector = scriptData[id].nextPosition[index - 1] - scriptData[id].nextPosition[index];

				float newRotation = atan2(directionalvector.y, directionalvector.x);

				//saving current position as 
				//savedLocation = scriptData[id].NextPosition[index];
				EntityManager::GetInstance().Get<Transform>(follower).position = scriptData[id].nextPosition[index-1] + vec2{scriptData[id].Size * cosf(newRotation - static_cast<float>(M_PI)), scriptData[id].Size * sinf(newRotation - static_cast<float>(M_PI))};

				++index;
				
				//checking rotation to set can ignore this for now lets get position to work
				if (scriptData[id].LookTowardsMovement)
					EntityManager::GetInstance().Get<Transform>(follower).orientation = newRotation;
				else
					EntityManager::GetInstance().Get<Transform>(follower).width = EntityManager::GetInstance().Get<Transform>(id).width;
			}
		}
		scriptData[id].prevPosition = newPosition;
	}

	void FollowScript_v2_0::Destroy(EntityID id)
	{
		scriptData[id].followers.clear();
	}

	void FollowScript_v2_0::OnAttach(EntityID id)
	{
		scriptData[id] = FollowScriptData_v2_0();
	}

	void FollowScript_v2_0::OnDetach(EntityID id)
	{		
		auto it = scriptData.find(id);
		if (it != scriptData.end())
		{
			scriptData.erase(id);
		}
	}

	void FollowScript_v2_0::SavePositions(EntityID id)
	{
		scriptData[id].cacheFollowerPosition.clear();
		for (EntityID followerID : scriptData[id].followers)
		{
			scriptData[id].cacheFollowerPosition.emplace_back(CatHelperFunctions::GetEntityPosition(followerID));
		}
	}

	void FollowScript_v2_0::ResetToSavePositions(EntityID id)
	{
		for (int i{ 0 }; i < scriptData[id].followers.size(); ++i)
		{
			CatHelperFunctions::PositionEntity(scriptData[id].followers[i], scriptData[id].cacheFollowerPosition[i]);
		}
	}

	void FollowScript_v2_0::PlayAdoptCatAudio()
	{
		int randomInteger = std::rand() % 2 + 1;
		SerializationManager m_serializationManager;
		EntityID sound{};

		switch (randomInteger)
		{
		case 1:
			sound = m_serializationManager.LoadFromFile("AudioObject/Cat Rescue SFX_Prefab.json");
			break;
		case 2:
			sound = m_serializationManager.LoadFromFile("AudioObject/Cat Rescue SFX2_Prefab.json");
			break;
		}

		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(sound);
	}

	void FollowScript_v2_0::CollisionCheck(const Event<CollisionEvents>& r_event)
	{
		if (p_gamestateController->currentState != GameStates_v2_0::EXECUTE) { return; }
		if (r_event.GetType() == CollisionEvents::OnTriggerEnter)
		{
			OnTriggerEnterEvent OTEE = static_cast<const OnTriggerEnterEvent&>(r_event);
			//EntityID id1{ event.Entity1 }, id2{ event.Entity2 };

			CatController_v2_0* p_catController = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);

			if (OTEE.Entity1 == p_catController->GetMainCatID() && p_catController->IsCatCaged(OTEE.Entity2))
			{
				scriptData[OTEE.Entity1].followers.emplace_back(OTEE.Entity2);
				PlayAdoptCatAudio();

				// Flag the cat so it knows it has been attached 
				CatScript_v2_0Data* catData{ GETSCRIPTDATA(CatScript_v2_0, OTEE.Entity2) };
				catData->isCaged = false;
				for (EntityID findCageID : Hierarchy::GetInstance().GetChildren(catData->catID))
				{
					if (EntityManager::GetInstance().Get<EntityDescriptor>(findCageID).name.find("Cage") != std::string::npos)
					{
						CatHelperFunctions::ToggleEntity(findCageID, false);
					}
				}

				CatHelperFunctions::PositionEntity(OTEE.Entity2, scriptData[OTEE.Entity1].nextPosition.back());
			}
			else if (OTEE.Entity2 == p_catController->GetMainCatID() && p_catController->IsCatCaged(OTEE.Entity1))
			{
				scriptData[OTEE.Entity2].followers.emplace_back(OTEE.Entity1);
				PlayAdoptCatAudio();

				// Flag the cat so it knows it has been attached 
				CatScript_v2_0Data* catData{ GETSCRIPTDATA(CatScript_v2_0, OTEE.Entity1) };
				catData->isCaged = false;
				for (EntityID findCageID : Hierarchy::GetInstance().GetChildren(catData->catID))
				{
					if (EntityManager::GetInstance().Get<EntityDescriptor>(findCageID).name.find("Cage") != std::string::npos)
					{
						CatHelperFunctions::ToggleEntity(findCageID, false);
					}
				}

				CatHelperFunctions::PositionEntity(OTEE.Entity1, scriptData[OTEE.Entity2].nextPosition.back());
			}
		}
	}
}