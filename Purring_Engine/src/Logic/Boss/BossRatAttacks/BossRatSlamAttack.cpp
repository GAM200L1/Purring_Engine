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
	BossRatSlamAttack::BossRatSlamAttack(EntityID furthestCat) : m_FurthestCat{furthestCat}
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, p_script->currentBoss);		
	}

	void BossRatSlamAttack::DrawTelegraphs(EntityID id)
	{
	}

	void BossRatSlamAttack::EnterAttack(EntityID)
	{
		
	}

	void BossRatSlamAttack::UpdateAttack(EntityID id, float dt)
	{	
		if (p_script->m_currentSlamTurnCounter == 2)
		{
			JumpUp(p_script->currentBoss,dt);
		}
		else if (p_script->m_currentSlamTurnCounter == 1)
		{
			p_data->finishExecution = true;
		}
		else if (p_script->m_currentSlamTurnCounter == 0)
			SlamDown(p_script->currentBoss,dt);

		//std::cout << "Boss Position: " << EntityManager::GetInstance().Get<Transform>(p_script->currentBoss).position.x << " " << EntityManager::GetInstance().Get<Transform>(p_script->currentBoss).position.y << std::endl;
	}

	void BossRatSlamAttack::ExitAttack(EntityID)
	{

	}

	BossRatSlamAttack::~BossRatSlamAttack()
	{
	}

	void BossRatSlamAttack::JumpUp(EntityID id,float dt)
	{
		if(EntityManager::GetInstance().Has<Collider>(id))
			EntityManager::GetInstance().Get<Collider>(id).isTrigger = true;


		Transform* bossTransform = &EntityManager::GetInstance().Get<Transform>(p_script->currentBoss);
		if (bossTransform->position.y < ScreenHeight/2 + bossTransform->width / 2 + 10)
			bossTransform->position.y += p_data->jumpSpeed * dt;
		else
		{
			SpawnRat(id);
		}
	}

	void BossRatSlamAttack::SlamDown(EntityID id,float dt)
	{
		Transform* bossTransform = &EntityManager::GetInstance().Get<Transform>(p_script->currentBoss);

		if (bossTransform->position.y > 0)
			bossTransform->position.y -= p_data->jumpSpeed * dt;
		else
		{
			if (EntityManager::GetInstance().Has<Collider>(id))
				EntityManager::GetInstance().Get<Collider>(id).isTrigger = false;
			p_data->finishExecution = true;
		}
	}

	void BossRatSlamAttack::SpawnRat(EntityID)
	{
		//need to determine if we are capable of spawning rats mid stage and have them work as per normal
		//need to determine if we want to use way point to spawn rats
		p_data->finishExecution = true;
	}

}