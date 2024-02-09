/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatController_v2_0.h
 \date     15-1-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains declarations for functions that manage cats in a scene.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Logic/Script.h"
#include "CatScript_v2_0.h"

namespace PE
{

	struct CatController_v2_0Data
	{
		// vector of pairs <catID, catType>
		//EntityID mainCatID{ 0 };
	};

	class CatController_v2_0 : public Script
	{
	public:
		// ----- Public Variables ----- //
		static EntityID mainInstance;
		
		std::map<EntityID, CatController_v2_0Data> m_scriptData; // data associated with each instance of the script

	public:
		// ----- Public Functions ----- //

		// ----- Constructor ----- //
		virtual ~CatController_v2_0();

		/*!***********************************************************************************
		 \brief					The Initialise Function for a script, to initialize any variables
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief					The update function of the script
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		\brief					The function that is called on the end of the script's lifetime
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void Destroy(EntityID id);

		/*!***********************************************************************************
		\brief					The function that is called when the script is attached onto an object
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		\brief					The function that is called when the script is detached from an object
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Gets Cats that have been collected and are deployable

		 \param[out] std::vector<EnumCatType> - Vector of Deployable Cats
		*************************************************************************************/
		std::vector<EnumCatType> GetDeployableCats();

		/*!***********************************************************************************
		 \brief Sets cat to play death animation, effectively removing it from scene

		 \param[in] id - ID of cat to remove
		*************************************************************************************/
		void KillCat(EntityID id);

		/*!***********************************************************************************
		 \brief Removes cats from current cats in the scene

		 \param[in] id - ID of cat to remove
		*************************************************************************************/
		void RemoveCatFromVector(EntityID id);

		/*!***********************************************************************************
		 \brief Sets the current cats in the scene

		 \param[in] r_vectorOfNewCats - A pair that holds the ID of current cat and cat type
		*************************************************************************************/
		inline void SetCurrentCats(std::vector<std::pair<EntityID, EnumCatType>> const& r_vectorOfNewCats)
		{
			m_currentCats = r_vectorOfNewCats;
			for (auto const& [catID, catType] : m_currentCats)
			{
				if (catType == EnumCatType::MAINCAT)
					m_mainCatID = catID;
			}
		}

		/*!***********************************************************************************
		 \brief Checks if entity is a cat

		 \param[in] id - ID to check
		 \param[out] bool - true if is cat, false if not
		*************************************************************************************/
		inline bool IsCat(EntityID id)
		{
			for (auto const& [catID, type] : m_currentCats)
			{
				if (catID == id) // cat is in vector
					return true;
			}
			return false; // is not cat
		}

		/*!***********************************************************************************
		 \brief Checks if cat is caged. Throws if not a cat

		 \param[in] id - catID to check
		 \param[out] bool - true if cat is caged, false if not
		*************************************************************************************/
		inline bool IsCatCaged(EntityID id)
		{
			if (!IsCat(id)) { throw; }
			return (GETSCRIPTDATA(CatScript_v2_0, id))->isCaged;
		}
		
		// getters
		/*!***********************************************************************************
		 \brief Gets current movement energy of cat

		 \param[in] id - catID to get energy of
		 \param[out] int - energy of the cat
		*************************************************************************************/
		int GetCurrentMovementEnergy(EntityID id);
		
		/*!***********************************************************************************
		 \brief Gets max movement energy of cat

		 \param[in] id - catID to get energy of
		 \param[out] int - energy of the cat
		*************************************************************************************/
		int GetMaxMovementEnergy(EntityID id);

		/*!***********************************************************************************
		 \brief Gets current vector of cat ids and types

		 \param[in] id - id of the cat controller
		 \param[out] std::vector<std::pair<EntityID, EnumCatType>> - current vector of cats
		*************************************************************************************/
		std::vector<std::pair<EntityID, EnumCatType>> GetCurrentCats(EntityID id);

		/*!***********************************************************************************
		 \brief Gets script data

		 \param[out] std::map<EntityID, CatController_v2_0Data>& Reference to script data variable
		*************************************************************************************/
		std::map<EntityID, CatController_v2_0Data>& GetScriptData() { return m_scriptData; }

		/*!***********************************************************************************
		 \brief Gets rttr instance of script data

		 \param[out] rttr::instance - rttr instance to script data variable
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id) { return rttr::instance(m_scriptData.at(id)); }

	private:
		EntityID m_mainCatID{ 0 };
		bool m_lostGame;
		std::vector<std::pair<EntityID, EnumCatType>> m_currentCats;
		std::vector<std::pair<EntityID, EnumCatType>> m_cachedCats;
	};
}