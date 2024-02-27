/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatBashAttack.cpp
\date     27 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Definition for BossRatBashAttack class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "prpch.h"
#include "BossRatBashAttack.h"

namespace PE
{
	BossRatBashAttack::BossRatBashAttack(EntityID furthestCat) : m_FurthestCat{furthestCat}
	{
	}
	void BossRatBashAttack::DrawTelegraphs(EntityID id)
	{
		Transform furthestCatTransform;

		if (EntityManager::GetInstance().Has<Transform>(m_FurthestCat))
			furthestCatTransform = EntityManager::GetInstance().Get<Transform>(m_FurthestCat);



	}
	void BossRatBashAttack::EnterAttack(EntityID)
	{
	}
	void BossRatBashAttack::UpdateAttack(EntityID)
	{
	}
	void BossRatBashAttack::ExitAttack(EntityID)
	{
	}
	BossRatBashAttack::~BossRatBashAttack()
	{
	}
}