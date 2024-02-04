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

namespace PE
{
	enum EnumCatType
	{
		MAINCAT,
		GREYCAT,
		ORANGECAT,
		FLUFFYCAT
	};

	enum EnumCatState
	{
		PLANMOVEMENT,
		PLANATTACK,
		EXECUTEMOVEMENT,
		EXECUTEMOVEMENT,
		FOLLOWING,
		DEAD
	};

	struct CatControllerScript_v2_0Data
	{
		// vector of pairs <catID, catType>
		std::vector<std::pair<EntityID, EnumCatType>> cats;
	};

	class CatControllerScript_v2_0 : public Script
	{
	public:
		// ----- Public Variables ----- //
		std::map<EntityID, CatControllerScript_v2_0Data> m_scriptData; // data associated with each instance of the script

	public:
		// ----- Public Functions ----- //
		virtual ~CatControllerScript_v2_0();

		virtual void Init(EntityID id);

		virtual void Update(EntityID id, float deltaTime);

		virtual void Destroy(EntityID id) {}

		virtual void OnAttach(EntityID id);

		virtual void OnDetach(EntityID id);

		// getters
		int GetCurrentMovementEnergy(EntityID id);
		int GetMaxMovementEnergy(EntityID id);
		std::vector<std::pair<EntityID, EnumCatType>> GetCatVector(EntityID id);
		EnumCatState GetCatState(EntityID id);
	};

	// ----- Cat Attack Base Class ----- //
	class CatAttackBaseScript : public Script
	{
	public:
		// additional functions
		virtual void ToggleAttackTelegraphs() = 0;
		virtual void ExecuteCatAttack() = 0;
	};
}