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
	constexpr size_t MAXFOLLOWERS = 4;
	struct FollowScriptData_v2_0 
	{
		int Size{ 64 }; // fixed size of each object
		float Rotation{};
		vec2 CurrentPosition;
		std::vector<vec2> NextPosition;

		//for attaching through code
		bool IsAttaching{ false };
		//int NumberOfAttachers{ 0 };
		std::vector<EntityID> ToAttach{};

		//look towards movement
		bool LookTowardsMovement{ false };

		// new followers stack
		std::vector<EntityID> followers;
		
		//sound
		EntityID SoundID{};
	};


	class FollowScript_v2_0 : public Script
	{
	public:
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

		/*!***********************************************************************************
		 \brief Called when a collision event has occured. 

		 \param[in,out] r_event - Collision event data.
		*************************************************************************************/
		void CollisionCheck(const Event<CollisionEvents>& r_event);

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \return std::map<EntityID, FollowScriptData>& Map of the script data
		*************************************************************************************/
		std::map<EntityID, FollowScriptData_v2_0>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id ID of script to return
		 \return rttr::instance Script instance
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);
		
		// ----- Destructor ----- //
		virtual ~FollowScript_v2_0();
	private:
		std::map<EntityID, FollowScriptData_v2_0> m_ScriptData;
		GameStateController_v2_0* p_gamestateController;
		// Event keys
		int m_collisionEventListener{};
	};
}