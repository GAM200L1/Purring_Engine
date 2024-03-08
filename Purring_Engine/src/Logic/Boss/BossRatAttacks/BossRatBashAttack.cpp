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
#include "Logic/Cat/CatController_v2_0.h"
namespace PE
{
	BossRatBashAttack::BossRatBashAttack(EntityID closestCat) : m_closestCat{ closestCat }
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, p_script->currentBoss);

		m_attackActivationTime = p_data->activationTime;
	}

	void BossRatBashAttack::DrawTelegraphs(EntityID id)
	{
		Transform closestCatTransform;

		if (EntityManager::GetInstance().Has<Transform>(m_closestCat))
			closestCatTransform = EntityManager::GetInstance().Get<Transform>(m_closestCat);

		Transform BossTransform;

		if (EntityManager::GetInstance().Has<Transform>(id))
			BossTransform = EntityManager::GetInstance().Get<Transform>(id);

		//Direction of Boss to Furthest Cat
		vec2 direction = closestCatTransform.position - BossTransform.position;
		vec2 unitDirection = direction.GetNormalized();

		SerializationManager sm;


		//to be put into a while loop, while not hitting obstacle
		//Draw Telegraphs
		
		//atleast 1
		EntityID telegraph = sm.LoadFromFile(m_telegraphPrefab);
		m_telegraphPoitions.push_back(telegraph);
		vec2 NextPosition;
		
		Transform* TelegraphTransform;

		if (EntityManager::GetInstance().Has<Transform>(telegraph))
		{
			TelegraphTransform = &EntityManager::GetInstance().Get<Transform>(telegraph);
			TelegraphTransform->position = NextPosition = BossTransform.position + unitDirection * (BossTransform.width/2 + TelegraphTransform->width /2);
			m_noOfAttack++;
			for (auto ie : EntityManager::GetInstance().Get<EntityDescriptor>(telegraph).children)
			{
				m_attackAnimations.push_back(ie);
				if (EntityManager::GetInstance().Has<AnimationComponent>(ie))
				{
					EntityManager::GetInstance().Get<AnimationComponent>(ie).PlayAnimation();
				}
			}
		}

		NextPosition += unitDirection * TelegraphTransform->width;

		//more than 1
		while (CheckOutsideOfWall(NextPosition))
		{
			telegraph = sm.LoadFromFile(m_telegraphPrefab);
			m_telegraphPoitions.push_back(telegraph);

			if (EntityManager::GetInstance().Has<Transform>(telegraph))
			{
				TelegraphTransform = &EntityManager::GetInstance().Get<Transform>(telegraph);
				TelegraphTransform->position = NextPosition;

				for (auto ie : EntityManager::GetInstance().Get<EntityDescriptor>(telegraph).children)
				{
					m_attackAnimations.push_back(ie);
					if (EntityManager::GetInstance().Has<AnimationComponent>(ie))
					{
						EntityManager::GetInstance().Get<AnimationComponent>(ie).PlayAnimation();
					}
				}

				NextPosition += unitDirection * TelegraphTransform->width;

			}
			m_noOfAttack++;

			//just incase somehow it goes over all the obstacles
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
			for (auto& iz : m_attackAnimations)
			{
				if (EntityManager::GetInstance().Has<AnimationComponent>(iz))
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(iz).HasAnimationEnded())
					{
						if(EntityManager::GetInstance().Has<EntityDescriptor>(iz))
							EntityManager::GetInstance().Get<EntityDescriptor>(iz).isActive = false;
					}
				}
			}

			if (m_attackDelay <= 0 && m_endExecutionTimer == m_endExecutionTime)
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

			CheckCollisionWithTelegraphs();

			if (m_attacksActivated == m_noOfAttack)
			{
				if (m_endExecutionTimer <= 0)
				{
					m_attackActivationTime = p_data->activationTime;
					p_data->finishExecution = true;
				}
				else
				{
					m_endExecutionTimer -= dt;
				}

			}

		}


	}
	void BossRatBashAttack::ExitAttack(EntityID)
	{
		//std::vector<EntityID> childs;
		for (auto& id : m_telegraphPoitions)
		{
			std::cout << "deleting telegraph" << std::endl;
			//for (auto ie : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
			//{
			//	childs.push_back(ie);
			//}
			EntityManager::GetInstance().RemoveEntity(id);
		}
		for (auto& iz : m_attackAnimations)
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

	bool BossRatBashAttack::CheckCollisionWithTelegraphs()
	{
		CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);

		for (int i = 0; i < m_attacksActivated; ++i)
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_attackAnimations[i]))
			{
				if (!EntityManager::GetInstance().Get<EntityDescriptor>(m_attackAnimations[i]).isActive)
					continue;
			}
			//check circle circle collision with each cat
			for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
			{
				Transform cattransform = EntityManager::GetInstance().Get<Transform>(CatID);
				Transform telegraphTransform = EntityManager::GetInstance().Get<Transform>(m_telegraphPoitions[i]);
				float distance = cattransform.position.Distance(telegraphTransform.position);
				float radiusSum = cattransform.width / 2 + telegraphTransform.width / 2;

				if (distance <= radiusSum)
				{
					GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(CatID);
				}
			}

		}
		return false;
	}
}