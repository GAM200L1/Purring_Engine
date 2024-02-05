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

		virtual void Destroy(EntityID id) {}

		virtual void OnAttach(EntityID id);

		virtual void OnDetach(EntityID id);

		void CacheCurrentCats();

		// getters
		int GetCurrentMovementEnergy(EntityID id);
		int GetMaxMovementEnergy(EntityID id);
		std::vector<std::pair<EntityID, EnumCatType>> GetCurrentCats(EntityID id);
		std::vector<std::pair<EntityID, EnumCatType>> GetCachedCats(EntityID id);

		std::map<EntityID, CatController_v2_0Data>& GetScriptData() { return m_scriptData; }
		rttr::instance GetScriptData(EntityID id) { return rttr::instance(m_scriptData.at(id)); }

	private:
		std::vector<std::pair<EntityID, EnumCatType>> m_currentCats;
		std::vector<std::pair<EntityID, EnumCatType>> m_cacheCats; // vector for when stage is restarted
	};
}