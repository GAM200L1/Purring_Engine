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
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, FollowScript_v2_0::CollisionCheck, this);
	}

	void FollowScript_v2_0::Update(EntityID id, float deltaTime)
	{
		if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->currentState == GameStates_v2_0::DEPLOYMENT)
		{ 
			scriptData[id].prevPosition = CatHelperFunctions::GetEntityPosition(id);
			return;
		}
		
		vec2 const& r_currPos = CatHelperFunctions::GetEntityPosition(id);
		
		if (!(r_currPos.x == scriptData[id].prevPosition.x && r_currPos.y == scriptData[id].prevPosition.y))
		{
			scriptData[id].nextPosition.clear();

			// adds the current position of the main cat
			scriptData[id].nextPosition.emplace_back(r_currPos);

			// adds the current positions and scales of the following cats
			for (EntityID followerID : scriptData[id].followers)
			{
				scriptData[id].nextPosition.emplace_back(CatHelperFunctions::GetEntityPosition(followerID));
			}

			int index{ 1 };

			for (auto follower : scriptData[id].followers)
			{
				vec2 directionalvector = scriptData[id].nextPosition[index - 1] - scriptData[id].nextPosition[index];
				float newRotation = atan2(directionalvector.y, directionalvector.x);
				float xScale{ std::abs(CatHelperFunctions::GetEntityScale(follower).x) * scriptData[id].distanceScale };

				// set position of this follower
				EntityManager::GetInstance().Get<Transform>(follower).position = scriptData[id].nextPosition[index - 1] + 
					vec2{ xScale * cosf(newRotation - static_cast<float>(M_PI)), xScale * sinf(newRotation - static_cast<float>(M_PI)) };

				if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->currentState == GameStates_v2_0::EXECUTE)
				{
					// Ensure the cat is facing the direction of their movement
					vec2 newScale{ CatHelperFunctions::GetEntityScale(follower) };
					newScale.x = std::abs(newScale.x) * ((directionalvector.Dot(vec2{ 1.f, 0.f }) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the cat is facing left
					CatHelperFunctions::ScaleEntity(follower, newScale.x, newScale.y);
				}

				++index;
			}
		}
		
		// save the previous position of the main cat
		scriptData[id].prevPosition = r_currPos;
	}

	void FollowScript_v2_0::Destroy(EntityID id)
	{
		scriptData.clear();
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
		while (!scriptData[id].cacheFollowerPosition.empty())
			scriptData[id].cacheFollowerPosition.pop();

		std::vector<vec2> temp;
		for (EntityID followerID : scriptData[id].followers)
		{
			// save position of follower cats
			temp.emplace_back(CatHelperFunctions::GetEntityPosition(followerID));
		}
		scriptData[id].cacheFollowerPosition.push(temp);
	}

	void FollowScript_v2_0::ResetToSavePositions(EntityID id)
	{
		if (scriptData[id].cacheFollowerPosition.empty()) { return; }
		std::vector<vec2> const& r_undoPositions = scriptData[id].cacheFollowerPosition.top();
		
		// reset positions
		for (int i{ 0 }; i < scriptData[id].followers.size(); ++i)
		{
			CatHelperFunctions::PositionEntity(scriptData[id].followers[i], r_undoPositions[i]);
		}
		scriptData[id].prevPosition = CatHelperFunctions::GetEntityPosition(id);

		scriptData[id].cacheFollowerPosition.pop();
	}

	void FollowScript_v2_0::CollisionCheck(const Event<CollisionEvents>& r_event)
	{
		// if not in execute state dont allow pick up
		if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->currentState != GameStates_v2_0::EXECUTE) { return; }
		
		// pick up cats
		if (r_event.GetType() == CollisionEvents::OnTriggerEnter)
		{
			OnTriggerEnterEvent OTEE = static_cast<const OnTriggerEnterEvent&>(r_event);

			CatScript_v2_0Data* p_catData{ nullptr };

			if (OTEE.Entity1 == GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID() && 
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(OTEE.Entity2) && 
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(OTEE.Entity2))
			{
				// add to followers list
				scriptData[OTEE.Entity1].followers.emplace_back(OTEE.Entity2);
				p_catData = GETSCRIPTDATA(CatScript_v2_0, OTEE.Entity2);
				GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->IncrementCatRescued();

				// set cat position to the end of the chain
				CatHelperFunctions::PositionEntity(OTEE.Entity2, scriptData[OTEE.Entity1].nextPosition.back());
			}
			else if (OTEE.Entity2 == GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID() && 
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(OTEE.Entity1) && 
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(OTEE.Entity1))
			{
				// add to followers list
				scriptData[OTEE.Entity2].followers.emplace_back(OTEE.Entity1);
				p_catData = GETSCRIPTDATA(CatScript_v2_0, OTEE.Entity1);
				GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->IncrementCatRescued();

				// set cat position to the end of the chain
				CatHelperFunctions::PositionEntity(OTEE.Entity1, scriptData[OTEE.Entity2].nextPosition.back());
			}

			if (p_catData != nullptr)
			{
				// toggle is caged to false
				p_catData->isCaged = false;
				for (EntityID childrenID : Hierarchy::GetInstance().GetChildren(p_catData->catID))
				{
					// all children will be inactive anyway - heart, cage, telegraphs, and projectile
					CatHelperFunctions::ToggleEntity(childrenID, false);
					
				}
				// play adopt audio
				CatScript_v2_0::PlayRescueCatAudio(p_catData->catType);
				p_catData = nullptr;
			}
		}
	}
}