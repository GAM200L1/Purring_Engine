/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FollowScript.h
 \date     02-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Script that allows the cats to follow after a leader. For M2 demo purposes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#pragma once
#include "Script.h"
#include "Math/MathCustom.h"

namespace PE
{
	struct FollowScriptData
	{
		int Size{ 100 }; // fixed size of each object
		float Speed{ 100 };
		int NumberOfFollower{1};
		std::vector<EntityID> FollowingObject;
		float Rotation;
		vec2 CurrentPosition;
		std::vector<vec2> NextPosition;

		//for attaching through code
		bool IsAttaching{ false };
		int NumberOfAttachers{ 1 };
		std::vector<EntityID> ToAttach{};

		//sound
		EntityID SoundID;
	};


	class FollowScript : public Script
	{
	public:
		/*!***********************************************************************************
		\brief Does nothing		
		*************************************************************************************/
		virtual void Init(EntityID) override;
		
		/*!***********************************************************************************
		\brief Updates the positions and rotations of the followers		
		*************************************************************************************/
		virtual void Update(EntityID,float) override;
		
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
		 \brief Get the Script Data object
		 
		 \return std::map<EntityID, FollowScriptData>& Map of the script data
		*************************************************************************************/
		std::map<EntityID, FollowScriptData>& GetScriptData();
		/*!***********************************************************************************
		 \brief Get the Script Data object
		 
		 \param[in,out] id ID of script to return
		 \return rttr::instance Script instance
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);
		/*!***********************************************************************************
		 \brief Does nothing		 
		*************************************************************************************/
		virtual ~FollowScript();
	private:
		std::map<EntityID, FollowScriptData> m_ScriptData;
	};
}