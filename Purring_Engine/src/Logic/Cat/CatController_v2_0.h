/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatController_v2_0.cpp
 \date     15-1-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains definitions for functions that manage cats in a scene.

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
		EntityID mainInstance;
		
		std::map<EntityID, CatController_v2_0Data> m_scriptData; // data associated with each instance of the script

	public:
		// ----- Public Functions ----- //
		virtual ~CatController_v2_0();

		virtual void Init(EntityID id);

		virtual void Update(EntityID id, float deltaTime);

		virtual void Destroy(EntityID id);

		virtual void OnAttach(EntityID id);

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
		static void KillCat(EntityID id);

		void RemoveCatFromVector(EntityID id);

		inline void SetCurrentCats(std::vector<std::pair<EntityID, EnumCatType>> const& r_vectorOfNewCats)
		{
			m_currentCats = r_vectorOfNewCats;
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
		int GetCurrentMovementEnergy(EntityID id);
		int GetMaxMovementEnergy(EntityID id);
		std::vector<std::pair<EntityID, EnumCatType>> GetCurrentCats(EntityID id);

		std::map<EntityID, CatController_v2_0Data>& GetScriptData() { return m_scriptData; }
		rttr::instance GetScriptData(EntityID id) { return rttr::instance(m_scriptData.at(id)); }

	private:
		std::vector<std::pair<EntityID, EnumCatType>> m_currentCats;
		std::vector<std::pair<EntityID, EnumCatType>> m_cachedCats;
	};
}