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
#include "Script.h"

namespace PE
{
	enum EnumCatType
	{
		MAINCAT,
		GREYCAT,
		ORANGECAT,
		FLUFFYCAT
	};

	class CatAttackBaseScript : public Script
	{
	public:
		// additional functions
		virtual void ToggleAttackTelegraphs() = 0;
		virtual void ExecuteCatAttack() = 0;
	};

	struct CatController_v2_0ScriptData
	{
		// vector of cats
	};
}