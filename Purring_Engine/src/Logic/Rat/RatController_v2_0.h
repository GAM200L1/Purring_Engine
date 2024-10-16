/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatController_v2_0.h
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations for functions used to manage tje rats in the scene.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "../Script.h"
#include "../StateManager.h"
#include "RatScript_v2_0.h"
#include "../RatScript.h"

namespace PE
{
	//! struct with variables needed by cat scripts
	struct RatController_v2_0_Data
	{
		// reference entities
		EntityID myID{ 0 }; // ID of entity that this script belongs to
		std::map<EntityID, RatScriptData>* p_ratsMap;
		std::map<EntityID, RatScript_v2_0_Data>* p_ratsV2Map;
	};

	
	class RatController_v2_0 : public Script
	{
		// If you wish to have persistent data, store it in the script class

		// ----- Public Members ----- //
	public:
		std::map<EntityID, RatController_v2_0_Data> m_scriptData;
		
		// ID of the main script instance to reference 
		// NOTE: I'm assuming that the object that this script 
		// is tied to will never be zero, and that there'll 
		// only be one main rat controller in each scene!
		static EntityID mainInstance;

		// Container of IDs and rat types of active rats in the scene
		std::vector<std::pair<EntityID, EnumRatType>> m_cachedActiveRats{};

		bool ratsPrinted{ false }; // temp boolean to print the rats when the scene is first played

		// ----- Constructors ----- //
	public:
		/*!***********************************************************************************
		 \brief Does nothing.
		*************************************************************************************/
		virtual ~RatController_v2_0() {};


		// ----- Public Getters ----- //
	public:
		/*!***********************************************************************************
		 \brief Returns a container of pairs containing the IDs and types of all the rats 
						active in the scene.

		 \return Returns a container of pairs containing the IDs and types of all the rats 
						active in the scene.
		*************************************************************************************/
		std::vector<std::pair<EntityID, EnumRatType>> const& GetRats(EntityID id);
				
		/*!***********************************************************************************
		 \brief Returns the number of rats active in the scene.

		 \return Returns the number of rats active in the scene.
		*************************************************************************************/
		unsigned int RatCount(EntityID id);


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
		 \brief Reduces the health of a rat.

		 \param[in] ratID - ID of the rat to damage
		 \param[in] attackID - ID of the entity dealing damage to the rat
		 \param[in] damage - Amount of damage to deal to the rat
		*************************************************************************************/
		void ApplyDamageToRat(EntityID ratID, EntityID attackId, int damage);

		bool IsRatAndIsAlive(EntityID id) const;

		int GetRatHealth(EntityID id) const;
		int GetRatMaxHealth(EntityID id) const;

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \return std::map<EntityID, RatController_v2_0_Data>& Map of script data.
		*************************************************************************************/
		std::map<EntityID, RatController_v2_0_Data> &GetScriptData();

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id - ID of the script to get the data from
		 \return rttr::instance Instance of the script to get the data from
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		// ----- Private Members ----- //
	private:
		bool refreshedThisFrame{false}; // set to true if the rat data has been refreshed once this frame

		// ----- Private Functions ----- //
	private:
		/*!***********************************************************************************
		 \brief Helper function to en/disables an entity.

		 \param[in] id - EntityID of the entity to en/disable.
		 \param[in] setToActive - Whether this entity should be set to active or inactive.
		*************************************************************************************/
		static void ToggleEntity(EntityID id, bool setToActive);

		/*!***********************************************************************************
		 \brief Helper function to refresh the vector of cached rats.

		 \param[in,out] id - ID of the script to get the data from.
		*************************************************************************************/
		void RefreshRats(EntityID id);

	}; // end of class 

}