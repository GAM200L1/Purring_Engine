/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     FollowScript_v2_0.h
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Planning State.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include <vector>
#include <stack>
#include "../Script.h"
#include "Math/MathCustom.h"
#include "../GameStateController_v2_0.h"
#include "Events/CollisionEvent.h"

namespace PE
{
	struct FollowScriptData_v2_0 
	{
		// scaling value, multiplied with object scale to decide how far from main cat it should be
		float distanceScale{ 1.f };
		// previous position of main cat
		vec2 prevPosition;
		// vector of positions to set for each cat
		std::vector<vec2> nextPosition;

		// new followers stack
		std::vector<EntityID> followers;
		// saves the follower position for undoing etc.
		std::stack<std::vector<vec2>> cacheFollowerPosition;
	};


	class FollowScript_v2_0 : public Script
	{
	public:
		std::map<EntityID, FollowScriptData_v2_0> scriptData;
		
	public:
		// ----- Destructor ----- //
		/*!***********************************************************************************
		 \brief Destructor for Follow_Script_v2_0
		*************************************************************************************/
		virtual ~FollowScript_v2_0();

		/*!***********************************************************************************
		\brief Does nothing
		*************************************************************************************/
		virtual void Init(EntityID) override;

		/*!***********************************************************************************
		\brief Updates the positions and rotations of the followers
		*************************************************************************************/
		virtual void Update(EntityID, float) override;

		/*!***********************************************************************************
		\brief Does nothing
		*************************************************************************************/
		virtual void Destroy(EntityID) override;

		/*!***********************************************************************************
		\brief Create followers
		*************************************************************************************/
		virtual void OnAttach(EntityID) override;

		/*!***********************************************************************************
		 \brief Clears the script data
		*************************************************************************************/
		virtual void OnDetach(EntityID) override;

	public:
		/*!***********************************************************************************
		 \brief Caches the positions of all the follower cats

		 \param[in] id - id of the main cat that can pick up cats
		*************************************************************************************/
		void SavePositions(EntityID id);

		/*!***********************************************************************************
		 \brief Sets position of all follower cats to cached positions

		 \param[in] id - id of the main cat that can pick up cats
		*************************************************************************************/
		void ResetToSavePositions(EntityID id);

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \return std::map<EntityID, FollowScriptData>& Map of the script data
		*************************************************************************************/
		std::map<EntityID, FollowScriptData_v2_0>& GetScriptData() { return scriptData; }

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id ID of script to return
		 \return rttr::instance Script instance
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id) { return rttr::instance(scriptData.at(id)); }

	private:
		/*!***********************************************************************************
		 \brief Called when a collision event has occured. 

		 \param[in,out] r_event - Collision event data.
		*************************************************************************************/
		void CollisionCheck(const Event<CollisionEvents>& r_event);
		
	private:
		// Event keys
		int m_collisionEventListener{};
	};
}