/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatScript.cpp
\date     24 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Definition for BossRatScript class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "prpch.h"
#include "BossRatScript.h"
#include "Logic/LogicSystem.h"
#include "BossRatExecuteState.h"
#include "BossRatPlanningState.h"
#include "Logic/Cat/CatController_v2_0.h"
#include "Layers/LayerManager.h"
#include "AudioManager/GlobalMusicManager.h"
#include "ResourceManager/ResourceManager.h"
#include "VisualEffects/ParticleSystem.h"
namespace PE
{
	// ---------- FUNCTION DEFINITIONS ---------- //

	BossRatScript::~BossRatScript()
	{
	}

	void BossRatScript::Init(EntityID id)
	{
		m_hasInitiatedAudioFadeOut = false; // Reset the flag for audio fade out

		FindAllObstacles();

		m_scriptData[id].collisionEnterEventKey = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnCollisionStay, BossRatScript::OnCollisionStay, this)
		m_scriptData[id].collisionStayEventKey = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnCollisionStay, BossRatScript::OnCollisionStay, this)

		m_scriptData[id].currentAttackInSet = 3;
		m_scriptData[id].currenthealth = m_scriptData[id].maxHealth;
		
		m_deathDelayTimeBeforeOutro = m_deathDelayTimerBeforeOutro;

		GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->SetCurrentLevel(3);
		
		m_chargeParticlePrefabID = ResourceManager::GetInstance().LoadPrefabFromFile(m_chargeParticlePrefab);
		Hierarchy::GetInstance().AttachChild(id, m_chargeParticlePrefabID);
		EntityManager::GetInstance().Get<Transform>(m_chargeParticlePrefabID).relPosition = vec2(0.f, -100.f);
		DeactiveObject(m_chargeParticlePrefabID);
	}


	void BossRatScript::Update(EntityID id, float deltaTime)
	{
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		if (p_gsc->currentState == GameStates_v2_0::PAUSE || p_gsc->currentState == GameStates_v2_0::DEPLOYMENT) 
		{
			return;
		}

		if (p_gsc->currentState == GameStates_v2_0::WIN || p_gsc->currentState == GameStates_v2_0::LOSE)
		{
			return;
		}		

		if (m_scriptData[id].currenthealth <= 0)
		{
			//do something, call end cutscene most likely and end bgm here
			if (m_scriptData[currentBoss].curr_Anim != BossRatAnimationsEnum::DEATH)
			{
				PlayAnimation(BossRatAnimationsEnum::DEATH);
				PlayDeathAudio();

				if (!m_hasInitiatedAudioFadeOut)
				{
					GlobalMusicManager::GetInstance().StartFadeOutAllAudio(3.0f);
					m_hasInitiatedAudioFadeOut = true;									// Ensure all audio fade-out only once
				}
			}

			//keep in execution phase
			//m_scriptData[id].finishExecution = false;
			if (EntityManager::GetInstance().Get<AnimationComponent>(currentBoss).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(currentBoss).GetAnimationMaxIndex())
			{
				if (m_deathDelayTimeBeforeOutro > 0)
				{
					m_deathDelayTimeBeforeOutro -= deltaTime;
				}
				else
				{
					m_deathDelayTimeBeforeOutro = m_deathDelayTimeBeforeOutro;
					p_gsc->GoToOutroCutscene();
					EntityManager::GetInstance().Get<EntityDescriptor>(currentBoss).isActive = false;
				}

			}
		}

		CreateCheckStateManager(id);

		if(p_gsc->currentState == GameStates_v2_0::PLANNING || p_gsc->currentState == GameStates_v2_0::EXECUTE)
		m_scriptData[id].p_stateManager->Update(id, deltaTime);

		if (EntityManager::GetInstance().Has<AnimationComponent>(currentBoss))
		{
			if(EntityManager::GetInstance().Get<AnimationComponent>(currentBoss).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(currentBoss).GetAnimationMaxIndex())
			{
				if (m_scriptData[currentBoss].curr_Anim != BossRatAnimationsEnum::IDLE && m_scriptData[currentBoss].curr_Anim != BossRatAnimationsEnum::CHARGE && m_scriptData[currentBoss].curr_Anim != BossRatAnimationsEnum::WALKFASTER && m_scriptData[currentBoss].curr_Anim != BossRatAnimationsEnum::DEATH)
					PlayAnimation(BossRatAnimationsEnum::IDLE);
			}
		}



		//to ensure execution ends
		if(executionTimeOutTimer > 0 && p_gsc->currentState == GameStates_v2_0::EXECUTE)
		{
			executionTimeOutTimer -= deltaTime;
			if (executionTimeOutTimer <= 0)
			{
				m_scriptData[id].finishExecution = true;
				executionTimeOutTimer = executionTimeOutTime;
			}
		 }
	}


	void BossRatScript::OnAttach(EntityID id)
	{
		m_scriptData[id] = BossRatScriptData();
		currentBoss = id;
		currentSlamTurnCounter = 0;
	}


	void BossRatScript::OnDetach(EntityID id)
	{
		// Delete this instance's script data
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			m_scriptData.erase(id);
			REMOVE_KEY_COLLISION_LISTENER(m_scriptData[id].collisionEnterEventKey)
			REMOVE_KEY_COLLISION_LISTENER(m_scriptData[id].collisionStayEventKey)
		}

		poisonPuddles.clear();
	}


	std::map<EntityID, BossRatScriptData>& BossRatScript::GetScriptData()
	{
		return m_scriptData;
	}


	rttr::instance BossRatScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

	void BossRatScript::CreateCheckStateManager(EntityID id)
	{
		if (m_scriptData.count(id))
		{
			if (m_scriptData.at(id).p_stateManager)
				return;

			m_scriptData[id].p_stateManager = new StateMachine{};
			m_scriptData[id].p_stateManager->ChangeState(new BossRatPlanningState{}, id);
		}
	}

	void BossRatScript::FindAllObstacles()
	{
		m_obstacles.clear();
		for (const auto& layer : LayerView<EntityDescriptor, Collider>(true))
		{
			for (const EntityID& id : InternalView(layer))
			{
				std::string nameTest = EntityManager::GetInstance().Get<EntityDescriptor>(id).name;
				if(EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Obstacle") != std::string::npos)
				m_obstacles.push_back(id);
			}
		}
	}

	void BossRatScript::TakeDamage(int damage)
	{
		if(m_scriptData[currentBoss].currenthealth > 0)
		{ 
			m_scriptData[currentBoss].currenthealth -= damage;

			if (m_scriptData[currentBoss].currenthealth < 0)
			{
				m_scriptData[currentBoss].currenthealth = 0;
			}
		}
		if(m_scriptData[currentBoss].curr_Anim != BossRatAnimationsEnum::WALKFASTER)
		PlayAnimation(BossRatAnimationsEnum::HURT);
		PlayHurtAudio();
	}

	EntityID BossRatScript::FindFurthestCat()
	{
		CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
		EntityID FurthestCat{};
		float FurthestDistance{};
		
		for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
		{
			Transform cattransform = EntityManager::GetInstance().Get<Transform>(CatID);

			if (cattransform.position.Distance(EntityManager::GetInstance().Get<Transform>(currentBoss).position) > FurthestDistance)
			{
				FurthestDistance = cattransform.position.Distance(EntityManager::GetInstance().Get<Transform>(currentBoss).position);
				FurthestCat = CatID;
			}

		}

		return FurthestCat;
	}

	EntityID BossRatScript::FindClosestCat()
	{
		CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
		EntityID ClosestCat{};
		float ClosestDistance{1000000000};

		for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
		{
			Transform cattransform = EntityManager::GetInstance().Get<Transform>(CatID);

			if (cattransform.position.Distance(EntityManager::GetInstance().Get<Transform>(currentBoss).position) < ClosestDistance)
			{
				ClosestDistance = cattransform.position.Distance(EntityManager::GetInstance().Get<Transform>(currentBoss).position);
				ClosestCat = CatID;
			}

		}
		return ClosestCat;
	}

	std::vector<EntityID> BossRatScript::GetAllObstacles()
	{
		return m_obstacles;
	}

	void BossRatScript::OnCollisionStay(const Event<CollisionEvents>& r_collisionStay)
	{
		OnCollisionStayEvent OCEE{ dynamic_cast<const OnCollisionStayEvent&>(r_collisionStay) };

		if (IsObstacle(OCEE.Entity1))
		{
			if (OCEE.Entity2 == currentBoss)
			{
				if (m_scriptData[currentBoss].p_currentAttack)
					m_scriptData[currentBoss].p_currentAttack->StopAttack();
			}
		}
		else if (IsObstacle(OCEE.Entity2))
		{
			if (OCEE.Entity1 == currentBoss)
			{
				if (m_scriptData[currentBoss].p_currentAttack)
					m_scriptData[currentBoss].p_currentAttack->StopAttack();
			}
		}

		CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);

		for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
		{
			if (OCEE.Entity1 == currentBoss  && OCEE.Entity2 == CatID
				|| OCEE.Entity2 == currentBoss && OCEE.Entity1 == CatID)
			{
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(CatID);
			}
		}
	}

	void BossRatScript::OnCollisionEnter(const Event<CollisionEvents>& r_collisionEnter)
	{
		OnCollisionEnterEvent OCEE{ dynamic_cast<const OnCollisionEnterEvent&>(r_collisionEnter) };

		CatController_v2_0* CatManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);

		for (auto [CatID, CatType] : CatManager->GetCurrentCats(CatManager->mainInstance))
		{
			if (OCEE.Entity1 == currentBoss && OCEE.Entity2 == CatID
				|| OCEE.Entity2 == currentBoss && OCEE.Entity1 == CatID)
			{
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(CatID);
			}
		}
	}

	bool BossRatScript::IsObstacle(EntityID id)
	{
	 return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Obstacle") != std::string::npos);;
	}

	void BossRatScript::PlayAnimation(BossRatAnimationsEnum AnimationState)
	{
		m_scriptData[currentBoss].curr_Anim = AnimationState;
		std::string animationState;
		
		switch (AnimationState)
		{
			case BossRatAnimationsEnum::WALK:
				animationState = "Walk";
				break;
			case BossRatAnimationsEnum::WALKFASTER:
				animationState = "WalkFaster";
				break;
			case BossRatAnimationsEnum::ATTACK1:
				animationState = "Attack1";
				break;
			case BossRatAnimationsEnum::ATTACK2:
				animationState = "Attack2";
				break;
			case BossRatAnimationsEnum::HURT:
				animationState = "Hurt";
				break;
			case BossRatAnimationsEnum::DEATH:
				animationState = "Death";
				break;
			case BossRatAnimationsEnum::CHARGE:
				animationState = "Charge";
				break;
			default: animationState = "Idle";
		}


			if (EntityManager::GetInstance().Has<AnimationComponent>(currentBoss) && m_scriptData[currentBoss].animationStates.size())
			{
				try
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(currentBoss).GetAnimationID() != m_scriptData[currentBoss].animationStates.at(animationState))
					{
						EntityManager::GetInstance().Get<AnimationComponent>(currentBoss).SetCurrentAnimationID(m_scriptData[currentBoss].animationStates[animationState]);
						EntityManager::GetInstance().Get<AnimationComponent>(currentBoss).PlayAnimation();
					}
				}
				catch (...) { /* error */ }
			}
		

	}

	void BossRatScript::PlayAttackAudio()
	{
		int random = rand() % 5 + 1;
		std::string attackAudio = "AudioObject/BossAudioPrefab/BossRatAttack" + std::to_string(random) + "SFX.prefab";
		GlobalMusicManager::GetInstance().PlaySFX(attackAudio, false);

	}

	void BossRatScript::PlayHurtAudio()
	{
		int random = rand() % 8 + 1;
		std::string hurtAudio = "AudioObject/BossAudioPrefab/BossRatInjured" + std::to_string(random) + "SFX.prefab";
		GlobalMusicManager::GetInstance().PlaySFX(hurtAudio, false);
	}

	void BossRatScript::PlayDeathAudio()
	{
		//GlobalMusicManager::GetInstance().PlaySFX("AudioObject/BossAudioPrefab/BossRatDeath1SFX.prefab", false);
		GlobalMusicManager::GetInstance().StartFadeOut("AudioObject/BossAudioPrefab/BossRatDeath1SFX.prefab", 3.0f);
	}

	void BossRatScript::PlayPoisonPuddleAudio()
	{
		GlobalMusicManager::GetInstance().PlaySFX("AudioObject/BossAudioPrefab/BossRatChargePuddlesSFX.prefab", false);
	}

	void BossRatScript::PlaySlamShockWaveAudio()
	{
		GlobalMusicManager::GetInstance().PlaySFX("AudioObject/BossAudioPrefab/BossRatSlamShockwaveSFX.prefab",false);
	}

	void BossRatScript::PlayBashSpikeAudio()
	{
		GlobalMusicManager::GetInstance().PlaySFX("AudioObject/BossAudioPrefab/BossRatBashSpikeSFX.prefab", false);
	}

	void BossRatScript::PlayChargeParticles(vec2 pos)
	{	
		/*if (EntityManager::GetInstance().Has<Transform>(m_chargeParticlePrefabID))
		{
			EntityManager::GetInstance().Get<Transform>(m_chargeParticlePrefabID).position = pos;
		}*/
		ResetChargeParticles(m_chargeParticlePrefabID);
		ActiveObject(m_chargeParticlePrefabID);
	}

	void BossRatScript::StopChargeParticles()
	{
		DeactiveObject(m_chargeParticlePrefabID);
	}

	void BossRatScript::ResetChargeParticles(EntityID id)
	{
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (EntityManager::GetInstance().Has<ParticleEmitter>(id2))
			{
				EntityManager::GetInstance().Get<ParticleEmitter>(id2).ResetAllParticles();
			}
		}
	}

	void BossRatScript::ActiveObject(EntityID id)
	{
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		//set active the current object
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = true;

		//set active the childrens if there are any
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (!EntityManager::GetInstance().Has<EntityDescriptor>(id2))
				break;


			if (EntityManager::GetInstance().Has<ParticleEmitter>(id2))
			{
				EntityManager::GetInstance().Get<ParticleEmitter>(id2).ResetAllParticles();
			}
			EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = true;
		}
	}
	void BossRatScript::DeactiveObject(EntityID id)
	{
		//deactive all the children objects first
		if (EntityManager::GetInstance().Has<EntityDescriptor>(id))
			for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
			{
				if (EntityManager::GetInstance().Has<ParticleEmitter>(id2))
				{
					EntityManager::GetInstance().Get<ParticleEmitter>(id2).ResetAllParticles(false);
				}

				if (!EntityManager::GetInstance().Has<EntityDescriptor>(id2))
					break;

				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}

		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		//deactive current object
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = false;
	}
} // End of namespace PE