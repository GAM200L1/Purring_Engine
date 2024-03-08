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
	BossRatChargeAttack::BossRatChargeAttack(EntityID furthestCat) : m_furthestCat{furthestCat}
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, p_script->currentBoss);

	}

	void BossRatChargeAttack::DrawTelegraphs(EntityID id)
	{
		Transform furthestCatTransform;

		if (EntityManager::GetInstance().Has<Transform>(m_furthestCat))
			furthestCatTransform = EntityManager::GetInstance().Get<Transform>(m_furthestCat);

		Transform BossTransform;

		if (EntityManager::GetInstance().Has<Transform>(id))
			BossTransform = EntityManager::GetInstance().Get<Transform>(id);

		//Direction of Boss to Furthest Cat
		vec2 direction = furthestCatTransform.position - BossTransform.position;
		vec2 unitDirection = direction.GetNormalized();

		SerializationManager sm;
		EntityID telegraph = sm.LoadFromFile(m_telegraphPrefab);

		if (EntityManager::GetInstance().Has<Transform>(telegraph))
		{
			Transform* TelegraphTransform = &EntityManager::GetInstance().Get<Transform>(telegraph);
			TelegraphTransform->position  = BossTransform.position + unitDirection * (furthestCatTransform.position.Distance(BossTransform.position) / 2) ;
			TelegraphTransform->orientation = atan2(unitDirection.y, unitDirection.x);
		}	
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