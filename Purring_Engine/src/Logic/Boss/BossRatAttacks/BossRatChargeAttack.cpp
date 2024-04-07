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
#include "ResourceManager/ResourceManager.h"
#include "Logic/CameraShakeScript.h"
#include "Graphics/CameraManager.h"
namespace PE
{
	BossRatChargeAttack::BossRatChargeAttack(EntityID furthestCat) : m_furthestCat{furthestCat}
	{
		p_script = GETSCRIPTINSTANCEPOINTER(BossRatScript);
		p_data = GETSCRIPTDATA(BossRatScript, p_script->currentBoss);

		m_activationTime = p_data->activationTime;
		m_distanceTravelled = p_data->distanceBetweenPools;
	}

	void BossRatChargeAttack::DrawTelegraphs(EntityID id)
	{
		Transform furthestCatTransform;

		if (EntityManager::GetInstance().Has<Transform>(m_furthestCat))
			furthestCatTransform = EntityManager::GetInstance().Get<Transform>(m_furthestCat);

		Transform BossTransform;

		if (EntityManager::GetInstance().Has<Transform>(id))
			BossTransform = EntityManager::GetInstance().Get<Transform>(id);

		vec2 StartPosition{};

		if (EntityManager::GetInstance().Has<Collider>(id))
		{
			CircleCollider cc = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
			StartPosition = cc.center;
		}

		//Direction of Boss to Furthest Cat
		vec2 direction = furthestCatTransform.position - StartPosition;
		vec2 unitDirection = m_chargeDirection = direction.GetNormalized();

		m_telegraph = ResourceManager::GetInstance().LoadPrefabFromFile(m_telegraphPrefab);

		if (EntityManager::GetInstance().Has<Transform>(m_telegraph))
		{
			Transform* TelegraphTransform = &EntityManager::GetInstance().Get<Transform>(m_telegraph);
			TelegraphTransform->position  = StartPosition + unitDirection * (furthestCatTransform.position.Distance(StartPosition) / 2) ;
			TelegraphTransform->orientation = atan2(unitDirection.y, unitDirection.x);
		}	
	}
	
	void BossRatChargeAttack::EnterAttack(EntityID)
	{
		if (!m_isCharging)
		{
			p_script->PlayChargeParticles(vec2(EntityManager::GetInstance().Get<Transform>(p_script->currentBoss).position.x + 15, EntityManager::GetInstance().Get<Transform>(p_script->currentBoss).position.y - 94));

			if (p_data->curr_Anim != BossRatAnimationsEnum::CHARGE)
			p_script->PlayAnimation(BossRatAnimationsEnum::CHARGE);
		}

		EntityManager::GetInstance().RemoveEntity(m_telegraph);
	}

	void BossRatChargeAttack::UpdateAttack(EntityID id, float dt)
	{	
		if (EntityManager::GetInstance().Get<AnimationComponent>(p_script->currentBoss).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(p_script->currentBoss).GetAnimationMaxIndex() && p_data->curr_Anim == BossRatAnimationsEnum::CHARGE)
		{
			m_animationPlayed = true;
			p_script->PlayAttackAudio();
			p_script->StopChargeParticles();
			p_script->PlayAnimation(BossRatAnimationsEnum::WALKFASTER);
			m_isCharging = true;
		}

		if (m_isCharging && m_activationTime > 0)
		{
			m_activationTime -= dt;
			return;
		}

		if (m_isCharging && m_animationPlayed)
		{
			if (EntityManager::GetInstance().Has<Transform>(id))
			{
				Transform* BossTransform = &EntityManager::GetInstance().Get<Transform>(id);
				BossTransform->position += m_chargeDirection * p_data->chargeSpeed * dt;

				m_distanceTravelled += p_data->chargeSpeed * dt;
				if (m_distanceTravelled >= p_data->distanceBetweenPools)
				{
					EntityID puddle = ResourceManager::GetInstance().LoadPrefabFromFile(m_poisonPuddlePrefab);

					vec2 StartPosition{};

					if (EntityManager::GetInstance().Has<Collider>(id))
					{
						CircleCollider cc = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
						StartPosition = cc.center;
					}

					if (EntityManager::GetInstance().Has<Transform>(puddle))
					{
						Transform* puddleTransform = &EntityManager::GetInstance().Get<Transform>(puddle);
						puddleTransform->position = StartPosition;
						p_script->PlayPoisonPuddleAudio();
					}
					p_script->poisonPuddles.insert(std::pair<EntityID,int>(puddle,3));
					m_distanceTravelled = 0;
				}
			}

			m_travelTime -= dt;
		}
		else if(m_animationPlayed)
		{
			m_chargeEndDelay -= dt;

			if (m_chargeEndDelay <= 0)
			{
				p_data->finishExecution = true;
			}
		}
	}
	
	void BossRatChargeAttack::ExitAttack(EntityID)
	{
	}

	void BossRatChargeAttack::StopAttack()
	{
		if (m_travelTime < 0)
		{
			if (p_data->curr_Anim != BossRatAnimationsEnum::IDLE && m_isCharging && p_data->curr_Anim != BossRatAnimationsEnum::DEATH)
			{
				p_script->PlayAnimation(BossRatAnimationsEnum::IDLE);
				GETSCRIPTINSTANCEPOINTER(CameraShakeScript)->Shake(GETCAMERAMANAGER()->GetMainCameraId());
				p_script->PlaySlamShockWaveAudio();
			}
			m_isCharging = false;

		}
	}

	BossRatChargeAttack::~BossRatChargeAttack()
	{
	}

}