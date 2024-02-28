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
#include "Logic/Boss/BossRatScript.h"
#include "Logic/LogicSystem.h"
#include "Hierarchy/HierarchyManager.h"

namespace PE
{
	BossRatBashAttack::BossRatBashAttack(EntityID furthestCat) : m_FurthestCat{furthestCat}
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, p_script->currentBoss);

		m_attackActivationTime = p_data->activationTime;
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
		
		//atleast 1
		EntityID telegraph = sm.LoadFromFile("RatBossBashAttackTelegraphwAnim_Prefab.json");
		m_telegraphPoitions.push_back(telegraph);
		vec2 NextPosition;
		
		Transform* TelegraphTransform = &EntityManager::GetInstance().Get<Transform>(telegraph);

		if (EntityManager::GetInstance().Has<Transform>(telegraph))
		{

			TelegraphTransform->position = NextPosition = BossTransform.position + unitDirection * (BossTransform.width/2 + TelegraphTransform->width /2);
			m_noOfAttack++;
		}

		NextPosition += unitDirection * TelegraphTransform->width;

		while (CheckOutsideOfWall(NextPosition))
		{
			telegraph = sm.LoadFromFile("RatBossBashAttackTelegraphwAnim_Prefab.json");
			m_telegraphPoitions.push_back(telegraph);

			TelegraphTransform = &EntityManager::GetInstance().Get<Transform>(telegraph);
			TelegraphTransform->position = NextPosition;

			NextPosition += unitDirection * TelegraphTransform->width;

			m_noOfAttack++;

			if (m_noOfAttack >= 8)
				return;
		}
		

	}
	void BossRatBashAttack::EnterAttack(EntityID)
	{
		for (auto& id : m_telegraphPoitions)
		{
			Graphics::Renderer* tele_r = &EntityManager::GetInstance().Get<Graphics::Renderer>(id);
			tele_r->SetColor(1, 1, 1, 0);
		}
	}
	void BossRatBashAttack::UpdateAttack(EntityID id, float dt)
	{	
		if (m_attackActivationTime > 0)
		{
			m_attackActivationTime -= dt;
			return;
		}
		else
		{
			if (m_attackDelay <= 0 && endExecutionTimer == endExecutionTime)
			{	
				EntityID tid = m_telegraphPoitions[m_attacksActivated++];
				for (auto ie : EntityManager::GetInstance().Get<EntityDescriptor>(tid).children)
				{
					EntityManager::GetInstance().Get<EntityDescriptor>(ie).isActive = true;
				}
				m_attackDelay = p_data->attackDelay;
			}
			else
			{
				m_attackDelay -= dt;
			}

			if (m_attacksActivated == m_noOfAttack)
			{
				if (endExecutionTimer <= 0)
				{
					m_attackActivationTime = p_data->activationTime;
					endExecutionTimer = endExecutionTimer;
					p_data->finishExecution = true;
				}
				else
				{
					endExecutionTimer -= dt;
				}

			}

		}


	}
	void BossRatBashAttack::ExitAttack(EntityID)
	{
		std::vector<EntityID> childs;
		for (auto& id : m_telegraphPoitions)
		{
			std::cout << "deleting telegraph" << std::endl;
			for (auto ie : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
			{
				childs.push_back(ie);
			}
			EntityManager::GetInstance().RemoveEntity(id);
		}
		for (auto& iz : childs)
		{
			EntityManager::GetInstance().RemoveEntity(iz);
		}
		m_telegraphPoitions.clear();
	}
	BossRatBashAttack::~BossRatBashAttack()
	{
	}

	bool BossRatBashAttack::CheckOutsideOfWall(vec2 Position)
	{
		std::vector<EntityID>obstacles = p_script->GetAllObstacles();

		for (auto& id : obstacles)
		{
			Transform wallTransform;

			if (EntityManager::GetInstance().Has<Transform>(id))
				wallTransform = EntityManager::GetInstance().Get<Transform>(id);

			if (Position.x >= wallTransform.position.x - wallTransform.width / 2 && Position.x <= wallTransform.position.x + wallTransform.width / 2 &&
				Position.y >= wallTransform.position.y - wallTransform.height / 2 && Position.y <= wallTransform.position.y + wallTransform.height / 2)
			{
				return false;
			}
		}
		return true;
	}
}