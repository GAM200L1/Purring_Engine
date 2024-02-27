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

		Transform BossTransform;

		if (EntityManager::GetInstance().Has<Transform>(id))
			BossTransform = EntityManager::GetInstance().Get<Transform>(id);

		//Direction of Boss to Furthest Cat
		vec2 direction = furthestCatTransform.position - BossTransform.position;
		vec2 unitDirection = direction.GetNormalized();

		SerializationManager sm;


		//to be put into a while loop, while not hitting obstacle
		//Draw Telegraphs
		EntityID telegraph = sm.LoadFromFile("RatBossBashAttackTelegraph_Prefab.json");
		m_telegraphPoitions.push_back(telegraph);


		if (EntityManager::GetInstance().Has<Transform>(telegraph))
		{
			Transform* TelegraphTransform = &EntityManager::GetInstance().Get<Transform>(telegraph);

			TelegraphTransform->position = BossTransform.position + unitDirection * 250;
		}



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

	void BossRatBashAttack::CheckCollisionWithWall(vec2 Position, float width)
	{

	}
}