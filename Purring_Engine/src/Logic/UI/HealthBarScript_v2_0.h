/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     HealthBarScript_v2_0.h
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains functions to update the position and visuals of the health bar UI.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "../StateManager.h"
#include "../Script.h"
#include "../Math/MathCustom.h"

namespace PE
{
		struct HealthBarScript_v2_0_Data
		{
				EntityID myID{ 0 };
				EntityID followObjectID{ 0 }; // ID of the object whose position we should follow
				std::array<float, 4> fillColor{ 1.f, 0.f, 0.f, 1.f };
		};

		class HealthBarScript_v2_0 : public Script
		{
				// If you wish to have persistent data, store it in the script class

				// ----- Public Members ----- //
		public:
				std::map<EntityID, HealthBarScript_v2_0_Data> m_scriptData;


				// ----- Constructors ----- //
		public:
				/*!***********************************************************************************
				 \brief Does nothing.
				*************************************************************************************/
				virtual ~HealthBarScript_v2_0() {};


				// ----- Public Functions ----- //
		public:
				/*!***********************************************************************************
				\brief Called when the object is first spawned into the scene

				\param[in,out] id - ID of the script to update
				*************************************************************************************/
				virtual void Init(EntityID id);

				/*!***********************************************************************************
				 \brief Called every frame.

				 \param[in,out] id - ID of the instance to update
				 \param[in,out] deltaTime - delta time used to update the state
				*************************************************************************************/
				virtual void Update(EntityID id, float deltaTime);

				/*!***********************************************************************************
				 \brief Does nothing
				*************************************************************************************/
				virtual void Destroy(EntityID) {}

				/*!***********************************************************************************
				 \brief Called when the script is attached to an entity (NOTE: even within the editor!).
						DO NOT create objects here.

				 \param[in,out] id - ID of Instance of script to update
				*************************************************************************************/
				virtual void OnAttach(EntityID id);

				/*!***********************************************************************************
				 \brief Called when the entity is destroyed and/or this script is removed from
						the entity. Delete the script data and statemanager here.

				 \param[in,out] id - ID of instance of script to clear
				*************************************************************************************/
				virtual void OnDetach(EntityID id);

				/*!***********************************************************************************
				 \brief Get the Script Data object

				 \return std::map<EntityID, nameScriptData>& Map of script data.
				*************************************************************************************/
				std::map<EntityID, HealthBarScript_v2_0_Data>& GetScriptData();

				/*!***********************************************************************************
				 \brief Get the Script Data object

				 \param[in,out] id - ID of the script to get the data from
				 \return rttr::instance Instance of the script to get the data from
				*************************************************************************************/
				rttr::instance GetScriptData(EntityID id);

		public:
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

				/*!***********************************************************************************
				 \brief Helper function to en/disables an entity.

				 \param[in] id - EntityID of the entity to en/disable.
				 \param[in] setToActive - Whether this entity should be set to active or inactive.
				*************************************************************************************/
				static void ToggleEntity(EntityID id, bool setToActive);

				/*!***********************************************************************************
				 \brief Adjusts the position of the transform to the value passed in.

				 \param[in] transformId - ID of the entity to update the transform of.
				 \param[in] r_position - Position to set the transform to.
				*************************************************************************************/
				static void PositionEntity(EntityID const transformId, vec2 const& r_position);

				/*!***********************************************************************************
				 \brief Adjusts the relative position of the transform to the value passed in.

				 \param[in] transformId - ID of the entity to update the transform of.
				 \param[in] r_position - Position to set the transform's relative position to.
				*************************************************************************************/
				static void PositionEntityRelative(EntityID const transformId, vec2 const& r_position);

		}; // end of class 

} // End of namespace PE