/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatSlamAttack.cpp
\date     27 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Definition for BossRatSlamAttack class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "prpch.h"
#include "BossRatSlamAttack.h"
#include "Logic/Boss/BossRatScript.h"
#include "Logic/LogicSystem.h"
#include "Hierarchy/HierarchyManager.h"
#include "Logic/Cat/CatController_v2_0.h"
namespace PE
{
	BossRatSlamAttack::BossRatSlamAttack()
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, p_script->currentBoss);		

		if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->leftSideSlamAnimation))
		{
			EntityManager::GetInstance().Get<AnimationComponent>(p_data->leftSideSlamAnimation).StopAnimation();
			EntityManager::GetInstance().Get<AnimationComponent>(p_data->leftSideSlamAnimation).ResetAnimation();
		}
		if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->rightSideSlamAnimation))
		{
			EntityManager::GetInstance().Get<AnimationComponent>(p_data->rightSideSlamAnimation).StopAnimation();
			EntityManager::GetInstance().Get<AnimationComponent>(p_data->rightSideSlamAnimation).ResetAnimation();
		}
	}

	void BossRatSlamAttack::DrawTelegraphs(EntityID id)
	{
		DecideSide();

		if(EntityManager::GetInstance().Has<EntityDescriptor>(p_data->slamTelegraph))
			EntityManager::GetInstance().Get<EntityDescriptor>(p_data->slamTelegraph).isActive = true;

		if (p_script->currentSlamTurnCounter == 2)
		{
			DrawDamageTelegraph(p_script->currentBoss);
		}

		UpdateDamageTelegraph(p_script->currentBoss);
	}

	void BossRatSlamAttack::EnterAttack(EntityID)
	{
		HideTelegraph(p_script->currentBoss);

		if (p_script->currentSlamTurnCounter == 0)
			HideDamageTelegraph(p_script->currentBoss);
	}

	void BossRatSlamAttack::UpdateAttack(EntityID id, float dt)
	{	
		if (p_script->currentSlamTurnCounter == 2)
		{
			JumpUp(p_script->currentBoss,dt);
			if(m_ratSpawned)
			p_data->finishExecution = true;
		
		}
		else if (p_script->currentSlamTurnCounter == 1)
		{
			UpdateSlamTelegraph(p_script->currentBoss, dt);
			if (m_slamTelegraphAnimated)
			p_data->finishExecution = true;
		}
		else if (p_script->currentSlamTurnCounter == 0)
		{
			SlamDown(p_script->currentBoss, dt);

			if (m_attackIsLeft)
			{
				if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->leftSideSlamAnimation))
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(p_data->leftSideSlamAnimation).HasAnimationEnded())
					{
						DisableAnimation(p_script->currentBoss);
					}
				}
			}
			else
			{
				if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->rightSideSlamAnimation))
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(p_data->rightSideSlamAnimation).HasAnimationEnded())
					{
						DisableAnimation(p_script->currentBoss);
					}
				}
			}
		}
	}

	void BossRatSlamAttack::ExitAttack(EntityID)
	{

	}

	BossRatSlamAttack::~BossRatSlamAttack()
	{
	}

	void BossRatSlamAttack::DecideSide()
	{
		Transform bossTransform = EntityManager::GetInstance().Get<Transform>(p_script->currentBoss);

		if (bossTransform.position.x < 0)
		{
			m_attackIsLeft = false;
			if(EntityManager::GetInstance().Has<Transform>(p_data->leftSideSlam))
			m_slamLandLocation = EntityManager::GetInstance().Get<Transform>(p_data->rightSideSlam).position;
		}
		else
		{
			m_attackIsLeft = true;
			if (EntityManager::GetInstance().Has<Transform>(p_data->rightSideSlam))
			m_slamLandLocation = EntityManager::GetInstance().Get<Transform>(p_data->leftSideSlam).position;
		}
	}

	void BossRatSlamAttack::JumpUp(EntityID id,float dt)
	{
		if(EntityManager::GetInstance().Has<Collider>(id))
			EntityManager::GetInstance().Get<Collider>(id).isTrigger = true;


		Transform* bossTransform = &EntityManager::GetInstance().Get<Transform>(p_script->currentBoss);
		if (bossTransform->position.y < m_screenHeight/2 + bossTransform->width / 2 + 10)
			bossTransform->position.y += p_data->jumpSpeed * dt;
		else
		{
			SpawnRat(id);
		}
	}

	void BossRatSlamAttack::SlamDown(EntityID id,float dt)
	{
		Transform* bossTransform = &EntityManager::GetInstance().Get<Transform>(p_script->currentBoss);
		bossTransform->position.x = m_slamLandLocation.x;

		if (m_slamAttackDelay > 0)
		{
			m_slamAttackDelay -= dt;
			return;
		}
		if (bossTransform->position.y > m_slamLandLocation.y)
			bossTransform->position.y -= p_data->slamSpeed * dt;
		else
		{
			bossTransform->position.y = m_slamLandLocation.y;
			if (EntityManager::GetInstance().Has<Collider>(id))
				EntityManager::GetInstance().Get<Collider>(id).isTrigger = false;

			CheckDamage(id);
			EnableAnimation(id);
		}
	}

	void BossRatSlamAttack::SpawnRat(EntityID)
	{
		//need to determine if we are capable of spawning rats mid stage and have them work as per normal
		//need to determine if we want to use way point to spawn rats
		m_ratSpawned = true;
	}

	void BossRatSlamAttack::HideTelegraph(EntityID)
	{
		if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->slamTelegraph))
			EntityManager::GetInstance().Get<EntityDescriptor>(p_data->slamTelegraph).isActive = false;
	}

	void BossRatSlamAttack::DrawDamageTelegraph(EntityID)
	{
		if (m_attackIsLeft)
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->leftSideSlam))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->leftSideSlam).isActive = true;

			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->slamAreaTelegraph))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->slamAreaTelegraph).isActive = true;

			if(EntityManager::GetInstance().Has<Transform>(p_data->slamAreaTelegraph))
				EntityManager::GetInstance().Get<Transform>(p_data->slamAreaTelegraph).position = EntityManager::GetInstance().Get<Transform>(p_data->leftSideSlam).position;
		}
		else
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->rightSideSlam))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->rightSideSlam).isActive = true;

			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->slamAreaTelegraph))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->slamAreaTelegraph).isActive = true;

			if (EntityManager::GetInstance().Has<Transform>(p_data->slamAreaTelegraph))
			{
				EntityManager::GetInstance().Get<Transform>(p_data->slamAreaTelegraph).position = EntityManager::GetInstance().Get<Transform>(p_data->rightSideSlam).position;
				EntityManager::GetInstance().Get<Transform>(p_data->slamAreaTelegraph).width = EntityManager::GetInstance().Get<Transform>(p_data->slamAreaTelegraph).height = m_slamSize;
			}
		}
	}

	void BossRatSlamAttack::UpdateDamageTelegraph(EntityID)
	{
		if (EntityManager::GetInstance().Has<Graphics::Renderer>(p_data->leftSideSlam) && EntityManager::GetInstance().Has<Graphics::Renderer>(p_data->rightSideSlam) && EntityManager::GetInstance().Has<Graphics::Renderer>(p_data->slamAreaTelegraph))
		{
			if (p_script->currentSlamTurnCounter == 2)
			{
				EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->leftSideSlam).SetColor(229.f/255.f,198.f/255.f,88.f/255.f);
				EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->rightSideSlam).SetColor(229.f / 255.f, 198.f / 255.f, 88.f / 255.f);
				EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->slamAreaTelegraph).SetColor(104.f / 255.f, 82.f / 255.f, 84.f / 255.f);

				if (EntityManager::GetInstance().Has<Transform>(p_data->slamAreaTelegraph))
				{
					EntityManager::GetInstance().Get<Transform>(p_data->slamAreaTelegraph).width = EntityManager::GetInstance().Get<Transform>(p_data->slamAreaTelegraph).height  = m_slamSize;
				}
			}
			else if (p_script->currentSlamTurnCounter == 1)
			{
				EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->leftSideSlam).SetColor(229.f / 255.f, 88.f / 255.f, 88.f / 255.f);
				EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->rightSideSlam).SetColor(229.f / 255.f, 88.f / 255.f, 88.f / 255.f);

			}

		}
	}

	void BossRatSlamAttack::UpdateSlamTelegraph(EntityID,float dt)
	{
		if (EntityManager::GetInstance().Has<Transform>(p_data->slamAreaTelegraph))
		{
			Transform* slamTransform = &EntityManager::GetInstance().Get<Transform>(p_data->slamAreaTelegraph);

			if (slamTransform->height > m_slamMinSize)
			{
				slamTransform->height -= m_slamShrinkSpeed * dt;
				slamTransform->width = slamTransform->height;
			}
			else
			{
				slamTransform->width = slamTransform->height = m_slamMinSize;
				m_slamTelegraphAnimated = true;
			}
		
		}
	
	}

	void BossRatSlamAttack::HideDamageTelegraph(EntityID)
	{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->leftSideSlam))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->leftSideSlam).isActive = false;
			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->rightSideSlam))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->rightSideSlam).isActive = false;
			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->slamAreaTelegraph))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->slamAreaTelegraph).isActive = false;
	}

	void BossRatSlamAttack::CheckDamage(EntityID)
	{
		CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
		for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
		{
			Transform catTransform = EntityManager::GetInstance().Get<Transform>(CatID);
			if (m_attackIsLeft)
			{
				if (catTransform.position.x < (EntityManager::GetInstance().Get<Transform>(p_data->leftSideSlam).position.x + EntityManager::GetInstance().Get<Transform>(p_data->leftSideSlam).width/2))
				{
					GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(CatID);
				}
			}
			else
			{
				if (catTransform.position.x >= (EntityManager::GetInstance().Get<Transform>(p_data->rightSideSlam).position.x - EntityManager::GetInstance().Get<Transform>(p_data->rightSideSlam).width / 2))
				{
					GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(CatID);
				}
			}
		}
	}

	void BossRatSlamAttack::EnableAnimation(EntityID)
	{
		if (m_attackIsLeft)
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->leftSideSlamAnimation))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->leftSideSlamAnimation).isActive = true;

			if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->leftSideSlamAnimation))
			{
				EntityManager::GetInstance().Get<AnimationComponent>(p_data->leftSideSlamAnimation).PlayAnimation();
			}
		}
		else
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->rightSideSlamAnimation))
				EntityManager::GetInstance().Get<EntityDescriptor>(p_data->rightSideSlamAnimation).isActive = true;

			if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->rightSideSlamAnimation))
			{
				EntityManager::GetInstance().Get<AnimationComponent>(p_data->rightSideSlamAnimation).PlayAnimation();		
			}
		}
	}

	void BossRatSlamAttack::DisableAnimation(EntityID)
	{

		if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->leftSideSlamAnimation))
			EntityManager::GetInstance().Get<EntityDescriptor>(p_data->leftSideSlamAnimation).isActive = false;
		if (EntityManager::GetInstance().Has<EntityDescriptor>(p_data->rightSideSlamAnimation))
			EntityManager::GetInstance().Get<EntityDescriptor>(p_data->rightSideSlamAnimation).isActive = false;

		p_data->finishExecution = true;
	}

}