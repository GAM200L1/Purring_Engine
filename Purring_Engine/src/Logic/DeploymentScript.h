/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     DeploymentScript.h
 \date     02-01-2024

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Definition for the script to deploy cats

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#pragma once
#include "Script.h"
#include "StateManager.h"
#include "Events/EventHandler.h"
#include "Math/MathCustom.h"
#include "Math/Transform.h"

namespace PE {

	struct DeploymentScriptData
	{
		EntityID DeploymentArea;
		EntityID NoGoArea;
		EntityID FollowingTextureObject;
		std::vector<EntityID> AddedCats;
	};

	class DeploymentScript : public Script
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
		 \brief Get the Script Data object

		 \return std::map<EntityID, FollowScriptData>& Map of the script data
		*************************************************************************************/
		std::map<EntityID, DeploymentScriptData>& GetScriptData();
		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id ID of script to return
		 \return rttr::instance Script instance
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);
		/*!***********************************************************************************
		 \brief Does nothing
		*************************************************************************************/
		virtual ~DeploymentScript();
	
	private:
		/*!***********************************************************************************
		 \brief Checks if the mouse cursor is within the bounds of any GUI objects

		 \param[in,out] r_ME mouse click event details
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);
		void GetMouseCurrentPosition(vec2& Output);
		bool CheckArea(Transform const& area, vec2 const& mousePos, float textureWidth);
	private:
		std::map<EntityID, DeploymentScriptData> m_ScriptData;
		vec2 m_mousepos;
		Transform m_deploymentZone;
		EntityID m_currentDeploymentScriptEntityID;
		bool m_inNoGoArea;
	};


}