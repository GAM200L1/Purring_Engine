/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     testScript.h
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu


 \brief  This file contains the declarations of testScript

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Logic/Script.h"
#include "BossRatScript.h"

namespace PE
{
	struct BossRatHealthBarScriptData
	{
		vec4 fillColorFull{ 0.36f, 0.98f, 0.21f, 1.f };  // Color of the healthbar when it's full
		vec4 fillColorHalf{ 0.92f, 0.98f, 0.21f, 1.f };  // Color of the healthbar when it's half full
		vec4 fillColorAlmostEmpty{ 0.98f, 0.23f, 0.21f, 1.f }; // Color of the healthbar when it's almost empty
	};

	class BossRatHealthBarScript : public Script
	{
	public:
		/*!***********************************************************************************
		 \brief Initialize the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to initialize the script for
		 \return void
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Update the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to update the script for
		 \param[in] deltaTime - The time passed since the last update
		 \return void
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Destroy the test script for an entity (RTTR)

		 \param[in] id - The unique EntityID to destroy the script for
		 \return void
		*************************************************************************************/
		virtual void Destroy(EntityID id);

		/*!***********************************************************************************
		 \brief Attach the test script to an entity (RTTR)

		 \param[in] id - The unique EntityID to attach the script to
		 \return void
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Detach the test script from an entity (RTTR)

		 \param[in] id - The unique EntityID to detach the script from
		 \return void
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Get the script data for all entities

		 \return std::map<EntityID, TestScriptData>& - A map of EntityID to TestScriptData
		*************************************************************************************/
		std::map<EntityID, BossRatHealthBarScriptData>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the script data instance for a specific entity (RTTR)

		 \param[in] id - The unique EntityID to retrieve the script data for
		 \return rttr::instance - The script data instance for the specified entity
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		/*!***********************************************************************************
		 \brief Destructor for the test script

		 \return void
		*************************************************************************************/
		~BossRatHealthBarScript();

		/*!***********************************************************************************
		 \brief Gets the fill amount of the healthbar.

		 \param[in] id - EntityID of the healthbar.
		*************************************************************************************/
		float GetFillAmount(EntityID id) const; // on a scale of 0 to 1

		/*!***********************************************************************************
		 \brief Sets the fill amount of the healthbar.

		 \param[in] id - EntityID of the healthbar.
		 \param[in] fillAmount - Value from 0 to 1 that represents how full the bar should be.
		*************************************************************************************/
		void SetFillAmount(EntityID id, float const fillAmount); // on a scale of 0 to 1

	private:
		std::map<EntityID, BossRatHealthBarScriptData> m_scriptData;
		BossRatScript* p_bsr;
	};


}