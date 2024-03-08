/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatChargeAttack.cpp
\date     08 - 03 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Definition for BossRatChargeAttack class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "prpch.h"
#include "BossRatChargeAttack.h"
#include "Logic/Boss/BossRatScript.h"
#include "Logic/LogicSystem.h"
#include "Hierarchy/HierarchyManager.h"
#include "Logic/Cat/CatController_v2_0.h"
namespace PE
{
	BossRatChargeAttack::BossRatChargeAttack(EntityID furthestCat) : m_FurthestCat{furthestCat}
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, p_script->currentBoss);

	}

	void BossRatChargeAttack::DrawTelegraphs(EntityID id)
	{
	
	}
	void BossRatChargeAttack::EnterAttack(EntityID)
	{
	}
	void BossRatChargeAttack::UpdateAttack(EntityID id, float dt)
	{	


	}
	void BossRatChargeAttack::ExitAttack(EntityID)
	{

	}
	BossRatChargeAttack::~BossRatChargeAttack()
	{
	}

}