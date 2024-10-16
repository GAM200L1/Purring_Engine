/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     CameraShakeScript.h
 \date     04-06-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu


 \brief  This file contains the declarations of CameraShakeScript

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "Math/MathCustom.h"

namespace PE
{
	struct CameraShakeScriptData
	{
		float shakeDuration{0.5f};
		float shakeAmount{20.f};
		float elapsedTime{};
		vec2 originalPosition{};
		bool shakeEnabled{ false };
		bool isShaking{ false };
	};

	class CameraShakeScript : public Script
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
		std::map<EntityID, CameraShakeScriptData>& GetScriptData();

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
		~CameraShakeScript();

		/*!***********************************************************************************
		 \brief Constructor for the test script

		 \return void
		*************************************************************************************/
		CameraShakeScript();

		/*!***********************************************************************************
		 \brief Enables shaking for the script

		 \param[in] id - The unique EntityID to destroy the script for
		 \return void
		*************************************************************************************/
		void Shake(EntityID);
	private:
		std::map<EntityID, CameraShakeScriptData> m_ScriptData;
	};


}