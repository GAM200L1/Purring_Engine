/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatScript_v2_0.cpp
 \date     15-1-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu
 \par      code %:     50%
 \par      changes:    Defined the cat script class and added functions for the
											 player cat's attack states.

 \co-author            Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 \par      code %:     50%
 \par      changes:    21-11-2023
											 Added functions for the player cat's movement states.

 \brief
	This file contains declarations for functions used for a grey player cat's states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"

#include "CatScript_v2_0.h"
#include "CatMovementStates_v2_0.h"
#include "GreyCatAttackStates_v2_0.h"
#include "CatPlanningState_v2_0.h"
#include "CatHelperFunctions.h"
#include "CatController_v2_0.h"
#include "FollowScript_v2_0.h"

#include "ECS/EntityFactory.h"
#include "ResourceManager/ResourceManager.h"

namespace PE
{
	CatScript_v2_0::~CatScript_v2_0()
	{
		for (auto& [key, value] : m_scriptData)
		{
			delete value.p_stateManager;
		}
	}

	void CatScript_v2_0::Init(EntityID id)
	{
		//m_scriptData[id].catID = id;
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		m_scriptData[id].catID = id;
		
		try
		{
			m_scriptData[id].p_catAnimation = &EntityManager::GetInstance().Get<AnimationComponent>(id);
		}
		catch (...)
		{
			// error
		}

		switch (m_scriptData[id].catType)
		{
			/*case EnumCatType::FLUFFYCAT:
			{
				break;
			}*/
			case EnumCatType::ORANGECAT:
			{
				OrangeCatAttackVariables& vars = std::get<OrangeCatAttackVariables>(m_scriptData[id].attackVariables);
				// Creates an entity for the projectile
				vars.CreateSeismicAndTelegraph(id);
				break;
			}
			case EnumCatType::MAINCAT:
			{
				m_mainCatID = id;
			}
			default: // main cat or grey cat
			{
				GreyCatAttackVariables& vars = std::get<GreyCatAttackVariables>(m_scriptData[id].attackVariables);
				// Creates an entity for the projectile
				vars.CreateProjectileAndTelegraphs(id, (m_scriptData[id].catType == EnumCatType::MAINCAT)? true : false);
				break; 
			}
		}

		MakeStateManager(id);

		m_scriptData[id].pathPositions.reserve(m_scriptData[id].catMaxMovementEnergy);
		m_scriptData[id].pathQuads.reserve(m_scriptData[id].catMaxMovementEnergy);
		for (size_t i{ 0 }; i < m_scriptData[id].catMaxMovementEnergy; ++i)
		{
			CreatePathNode(id);
		}
	}

	void CatScript_v2_0::Update(EntityID id, float deltaTime)
	{
		if (m_scriptData[id].isCaged || // if cat is caged dont run
			p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }

		if (p_gsc->currentState == GameStates_v2_0::WIN || p_gsc->currentState == GameStates_v2_0::LOSE)
		{
			for (auto quad : m_scriptData[id].pathQuads)
			{
				CatHelperFunctions::ToggleEntity(quad, false);
			}
			return;
		}

		// cat dies
		if (m_scriptData[id].toggleDeathAnimation)
		{
			// plays death animation
			PlayAnimation(id, "Death");
			// TODO: play death audio
			if (m_scriptData[id].playDeathSound)
			{
				m_scriptData[id].playDeathSound = false;
				for (EntityID& nodeId : m_scriptData[id].pathQuads)
				{
					CatHelperFunctions::ToggleEntity(nodeId, false);
				}
				SerializationManager m_serializationManager;
				EntityID sound{};

				if ((GETSCRIPTDATA(CatScript_v2_0, id))->catType == EnumCatType::MAINCAT)
				{
					sound = m_serializationManager.LoadFromFile("AudioObject/Cat Death SFX_Meowsalot.prefab");
				}
				else
				{
					int randomInteger = std::rand() % 3 + 1;

					switch (randomInteger)
					{
					case 1:
						sound = m_serializationManager.LoadFromFile("AudioObject/Cat Death SFX1.prefab");
						break;
					case 2:
						sound = m_serializationManager.LoadFromFile("AudioObject/Cat Death SFX2.prefab");
						break;
					case 3:
						sound = m_serializationManager.LoadFromFile("AudioObject/Cat Death SFX3.prefab");
						break;
					}
				}

				if (EntityManager::GetInstance().Has<AudioComponent>(sound))
					EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
				EntityManager::GetInstance().RemoveEntity(sound);
			}


			if (m_scriptData[id].p_catAnimation->GetCurrentFrameIndex() == m_scriptData[id].p_catAnimation->GetAnimationMaxIndex())
			{
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->RemoveCatFromCurrent(id);
				m_scriptData[id].toggleDeathAnimation = false;
				if (m_scriptData[id].catType == EnumCatType::MAINCAT)
					p_gsc->LoseGame();
			}
			return;
		}

		// check that state manager is still working
		if (!m_scriptData[id].p_stateManager)
		{
			//m_scriptData[id].catID = id;

			MakeStateManager(id);
		}

		// updates state // @TODO: change this
		m_scriptData[id].p_stateManager->Update(id, deltaTime);
		
		// changes states depending on cat type
		switch (m_scriptData[id].catType)
		{
		//case EnumCatType::FLUFFYCAT:
		//{
		//	// change states here for fluffy cat
		//	break;
		//}
		case EnumCatType::ORANGECAT:
		{
			if (p_gsc->currentState == GameStates_v2_0::PLANNING)
				PlanningStatesHandler<OrangeCatAttack_v2_0PLAN>(id, deltaTime);
			else if (p_gsc->currentState == GameStates_v2_0::EXECUTE)
			{
				// disable showing telegraphs
				ExecuteStatesHandler<OrangeCatAttack_v2_0EXECUTE>(id, deltaTime);
			}
			break;
		}
		default: // main cat or grey cat
			if (p_gsc->currentState == GameStates_v2_0::PLANNING)
				PlanningStatesHandler<GreyCatAttack_v2_0PLAN>(id, deltaTime);
			else if (p_gsc->currentState == GameStates_v2_0::EXECUTE)
			{
				ExecuteStatesHandler<GreyCatAttack_v2_0EXECUTE>(id, deltaTime);
			}
			break;
		}

		m_mouseClick = false; // reset mouse
	}

	void CatScript_v2_0::OnAttach(EntityID id)
	{
		// check if the given entity has transform, rigidbody, and collider. if it does not, assign it one
		if (!EntityManager::GetInstance().Has<Transform>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Transform>() });
		}
		if (!EntityManager::GetInstance().Has<RigidBody>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<RigidBody>() });
			EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
		}
		if (!EntityManager::GetInstance().Has<Collider>(id))
		{
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<Collider>() });
			EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider(); // cat default colliders is circle
		}

		if (m_scriptData.find(id) == m_scriptData.end())
		{
			m_scriptData[id] = CatScript_v2_0Data{};
		}
		else
		{
			delete m_scriptData[id].p_stateManager;
			m_scriptData[id] = CatScript_v2_0Data{};
		}

		// Reset values
		m_scriptData[id].shouldChangeState = false;
		m_scriptData[id].timeBeforeChangingState = 0.f;

		// Set the cat max energy to the value set in the editor
		//m_catBaseMaxEnergy = m_catMaxEnergy = m_scriptData[id].catMaxEnergy;
	}

	void CatScript_v2_0::OnDetach(EntityID id)
	{
		if (m_scriptData.find(id) != m_scriptData.end())
		{
			delete m_scriptData[id].p_stateManager;
			m_scriptData.erase(id);
		}
	}

	void CatScript_v2_0::MakeStateManager(EntityID id)
	{
		if (m_scriptData.count(id)) {
			if (m_scriptData.at(id).p_stateManager)
				return;
		}

		m_scriptData[id].p_stateManager = new StateMachine{};
		ChangeToPlanningState(id);
	}


	void CatScript_v2_0::TriggerStateChange(EntityID id, float const stateChangeDelay)
	{
		if (m_scriptData[id].delaySet) { return; } // Prevent the state from constantly resetting the delay

		// Flags that the state should change in [timeBeforeChangingState] seconds
		m_scriptData[id].shouldChangeState = true;
		m_scriptData[id].timeBeforeChangingState = stateChangeDelay;
		m_scriptData[id].delaySet = true;
	}


	bool CatScript_v2_0::CheckShouldStateChange(EntityID id, float const deltaTime)
	{
		// Waits for [timeBeforeChangingState] to pass before changing the state
		if (m_scriptData[id].shouldChangeState)
		{
			if (m_scriptData[id].timeBeforeChangingState > 0.f)
			{
				m_scriptData[id].timeBeforeChangingState -= deltaTime;
				return false;
			}
			else
			{
				// Change the state immediately
				m_scriptData[id].shouldChangeState = false;
				m_scriptData[id].timeBeforeChangingState = 0.f;
				m_scriptData[id].delaySet = false;
				return true;
			}
		}

		return false;
	}

	void CatScript_v2_0::CreatePathNode(EntityID id)
	{
		// create the entity
		EntityID nodeId{ EntityFactory::GetInstance().CreateEntity<Transform, Graphics::Renderer>() };

		EntityManager::GetInstance().Get<Graphics::Renderer>(nodeId).SetColor(1.f, 1.f, 1.f, 1.f); // sets the color of the node to white

		EntityManager::GetInstance().Get<Transform>(nodeId).width = m_scriptData[id].nodeSize;
		EntityManager::GetInstance().Get<Transform>(nodeId).height = m_scriptData[id].nodeSize;

		EntityManager::GetInstance().Get<EntityDescriptor>(nodeId).isActive = false;
		EntityManager::GetInstance().Get<EntityDescriptor>(nodeId).toSave = false;

		m_scriptData[id].pathQuads.emplace_back(nodeId);
	}

	template<typename AttackPLAN>
	void CatScript_v2_0::PlanningStatesHandler(EntityID id, float deltaTime)
	{
		std::string_view const& r_stateName = m_scriptData[id].p_stateManager->GetStateName();

		// plays idle animation
		PlayAnimation(id, "Idle");

		// checks if the current state is any of the planning states, if not then set to movement planning
		if (r_stateName != "PLANNING")
		{
			TriggerStateChange(id);
			if (CheckShouldStateChange(id, deltaTime))
			{
				ChangeToPlanningState(id);
				m_scriptData[id].p_catAnimation->SetCurrentFrameIndex(0); // resets animation to 0
			}
		}
	}

	template<typename AttackEXECUTE>
	void CatScript_v2_0::ExecuteStatesHandler(EntityID id, float deltaTime)
	{
		std::string_view const& r_stateName = m_scriptData[id].p_stateManager->GetStateName();

		auto ChangeToAttack = 
		[&](EntityID animateID)
		{
			// change to attack execute state
			m_scriptData[animateID].p_stateManager->ChangeState(new AttackEXECUTE{}, animateID);

			// reset and play attack animation
			m_scriptData[animateID].p_catAnimation->SetCurrentFrameIndex(0);
			if (m_scriptData[animateID].attackSelected)
			{
				// if attack has been selected play attack execute animation
				PlayAnimation(animateID, "Attack");
				m_executionAnimationDuration = ResourceManager::GetInstance().GetAnimation(m_scriptData[animateID].p_catAnimation->GetAnimationID())->GetAnimationDuration();
				m_scriptData[animateID].executionAnimationFinished = false;
				m_scriptData[animateID].finishedExecution = false;
			}
			else
			{
				// if no attack has been selected play idle animation
				PlayAnimation(animateID, "Idle");
				m_scriptData[animateID].finishedExecution = true;
				m_scriptData[animateID].executionAnimationFinished = true;
			}
		};

		auto HasFinishedAnimation = 
		[&]()
		{
			m_executionAnimationDuration -= deltaTime;
			return (m_executionAnimationDuration <= 0.f);
		};

		// when execution phase is activated, sets the state to movement
		if (r_stateName != "MovementEXECUTE" && r_stateName != "AttackEXECUTE")
		{
			TriggerStateChange(id);
			if (CheckShouldStateChange(id, deltaTime))
			{
				// if in cat chain level and main cat is moving, animation change to walking
				if ((!(GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.empty() && m_scriptData[m_mainCatID].catCurrentEnergy < m_scriptData[m_mainCatID].catMaxMovementEnergy) ||
					// if deployment level, check cats own energy
					(m_scriptData[id].catCurrentEnergy < m_scriptData[id].catMaxMovementEnergy))
				{
					// change to movement execute state
					m_scriptData[id].p_stateManager->ChangeState(new CatMovement_v2_0EXECUTE{}, id);
					
					// reset and play movement animation
					m_scriptData[id].p_catAnimation->SetCurrentFrameIndex(0);
					PlayAnimation(id, "Walk");
				}
				else // if not moving, immediately set to attack state
				{
					ChangeToAttack(id);
				}
			} 
		}
		// executes movement and plays movement animation
		else if (r_stateName == "MovementEXECUTE")// && !m_scriptData[id].attackSelected
		{
			// plays movement animation
			PlayAnimation(id, "Walk");
			
			if (CheckShouldStateChange(id, deltaTime))
			{
				ChangeToAttack(id);
			}
		}
		// executes attack and plays attack animation, plays idle animation if attack is finished early
		else if (r_stateName == "AttackEXECUTE")
		{
			if (m_scriptData[id].executionAnimationFinished)
			{
				PlayAnimation(id, "Idle");
			}
			else if (m_scriptData[id].attackSelected && HasFinishedAnimation())
			{
				m_scriptData[id].executionAnimationFinished = true; // if attack animation finished set to true
				m_scriptData[id].p_catAnimation->SetCurrentFrameIndex(0);
			}
		}
	}

	void CatScript_v2_0::PlayAnimation(EntityID id, std::string const& r_animationState)
	{
		if (m_scriptData[id].p_catAnimation != nullptr && m_scriptData[id].animationStates.size())
		{
			try
			{
				if (m_scriptData[id].p_catAnimation->GetAnimationID() != m_scriptData[id].animationStates.at(r_animationState))
				{
					m_scriptData[id].p_catAnimation->SetCurrentAnimationID(m_scriptData[id].animationStates.at(r_animationState));
				}
			}
			catch (...)
			{
				// error
			}
		}
	}

	void CatScript_v2_0::ChangeToPlanningState(EntityID id)
	{
		switch (m_scriptData[id].catType)
		{
		case EnumCatType::ORANGECAT:
		{
			m_scriptData[id].p_stateManager->ChangeState(new Cat_v2_0PLAN{ new OrangeCatAttack_v2_0PLAN, new CatMovement_v2_0PLAN }, id);
			break;
		}
		/*case EnumCatType::FLUFFYCAT:
		{
			break;
		}*/
		default: // grey cat or main cat
			m_scriptData[id].p_stateManager->ChangeState(new Cat_v2_0PLAN{new GreyCatAttack_v2_0PLAN, new CatMovement_v2_0PLAN}, id);
			break;
		}
	}

	void CatScript_v2_0::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		m_mouseClickPrevious = m_mouseClick;
		m_mouseClick = true;
	}
}