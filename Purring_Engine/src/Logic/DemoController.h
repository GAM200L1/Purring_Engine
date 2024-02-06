/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     DemoController.h
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations for functions used for rats.

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "StateManager.h"
#include "DemoController.h"
#include "CatScript.h"
#include "RatScript.h"

namespace PE
{
	//! struct with variables needed by cat scripts
	struct DemoControllerData
	{
		// reference entities
		EntityID myID{ 0 }; // ID of entity that this script belongs to
		std::map<EntityID, RatScriptData>* p_ratsMap;
		std::map<EntityID, CatScriptData>* p_catsMap;
	};

	
	class DemoController : public Script
	{
		// If you wish to have persistent data, store it in the script class

		// ----- Public Members ----- //
	public:
		std::map<EntityID, DemoControllerData> m_scriptData;
		
		// ID of the main script instance to reference 
		// NOTE: I'm assuming that the object that this script 
		// is tied to will never be zero, and that there'll 
		// only be one main rat controller in each scene!
		static EntityID mainInstance;

		// Container of IDs and rat types of active rats in the scene
		std::vector<std::pair<EntityID, EnumOldRatType>> m_cachedActiveRats{};
		std::vector<std::pair<EntityID, EnumCatTypes>> m_cachedActiveCats{};
		std::vector<std::pair<EntityID, EnumCatTypes>> m_aliveCatStartScene{};

		bool ratsPrinted{ false }; // temp boolean to print the rats when the scene is first played
		bool catsPrinted{ false }; // temp boolean to print the cats when the scene is first played

		// ----- Constructors ----- //
	public:
		/*!***********************************************************************************
		 \brief Does nothing.
		*************************************************************************************/
		virtual ~DemoController() {};


		// ----- Public Getters ----- //
	public:
		/*!***********************************************************************************
		 \brief Returns a container of pairs containing the IDs and types of all the rats 
						active in the scene.

		 \return Returns a container of pairs containing the IDs and types of all the rats 
						active in the scene.
		*************************************************************************************/
		std::vector<std::pair<EntityID, EnumOldRatType>> const& GetRats(EntityID id);
				
		/*!***********************************************************************************
		 \brief Returns a container of pairs containing the IDs and types of all the rats 
						active in the scene.

		 \return Returns a container of pairs containing the IDs and types of all the rats 
						active in the scene.
		*************************************************************************************/
		std::vector<std::pair<EntityID, EnumCatTypes>> const& GetCats(EntityID id);

		/*!***********************************************************************************
		 \brief Save current cats into a vector
		*************************************************************************************/
		void SaveCats();
				
		/*!***********************************************************************************
		 \brief Returns the number of rats active in the scene.

		 \return Returns the number of rats active in the scene.
		*************************************************************************************/
		unsigned int RatCount(EntityID id);
				
		/*!***********************************************************************************
		 \brief Returns the number of cats active in the scene.

		 \return Returns the number of cats active in the scene.
		*************************************************************************************/
		unsigned int CatCount(EntityID id);


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

		 \return std::map<EntityID, DemoControllerData>& Map of script data.
		*************************************************************************************/
		std::map<EntityID, DemoControllerData> &GetScriptData();

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id - ID of the script to get the data from
		 \return rttr::instance Instance of the script to get the data from
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);


		// ----- Private Functions ----- //
	private:
		/*!***********************************************************************************
		 \brief Helper function to en/disables an entity.

		 \param[in] id - EntityID of the entity to en/disable.
		 \param[in] setToActive - Whether this entity should be set to active or inactive.
		*************************************************************************************/
		static void ToggleEntity(EntityID id, bool setToActive);
	}; // end of class 

}