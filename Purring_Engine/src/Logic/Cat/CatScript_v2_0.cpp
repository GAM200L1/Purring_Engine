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
#include "AudioManager/GlobalMusicManager.h"

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
				
				PlayDeathAudio(m_scriptData[id].catType);
			}

			if (m_scriptData[id].p_catAnimation->GetCurrentFrameIndex() == m_scriptData[id].p_catAnimation->GetAnimationMaxIndex())
			{
				switch (m_scriptData[id].catType)
				{
					case EnumCatType::ORANGECAT:
					{
						OrangeCatAttackVariables const& vars = std::get<OrangeCatAttackVariables>(m_scriptData[id].attackVariables);
						// clears entities
						CatHelperFunctions::ToggleEntity(vars.seismicID, false);
						CatHelperFunctions::ToggleEntity(vars.telegraphID, false);
						break;
					}
					default: // main cat or grey cat
					{
						GreyCatAttackVariables const& vars = std::get<GreyCatAttackVariables>(m_scriptData[id].attackVariables);
						// clears entities
						for (auto const& [direction, telegraphID] : vars.telegraphIDs)
						{
							CatHelperFunctions::ToggleEntity(telegraphID, false);
						}
						CatHelperFunctions::ToggleEntity(vars.projectileID, false);
						break;
					}
				}
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

		EntityManager::GetInstance().Get<Graphics::Renderer>(nodeId).SetColor(0.506f, 0.490f, 0.490f, 1.f); // sets the color of the node to white

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
		if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsFollowCat(id)) 
		{ 
			m_scriptData[id].finishedExecution = true;
			return; 
		}
		
		std::string_view const& r_stateName = m_scriptData[id].p_stateManager->GetStateName();

		auto ChangeToAttack = 
		[&](EntityID animateID)
		{
			// change to attack execute state if it is a cat that can attack
			m_scriptData[animateID].p_stateManager->ChangeState(new AttackEXECUTE{}, animateID);

			// reset and play attack animation
			m_scriptData[animateID].p_catAnimation->StopAnimation();
			if (animateID == m_mainCatID)
			{
				for (EntityID follower : (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers)
				{
					m_scriptData[follower].p_catAnimation->StopAnimation();
				}
			}
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

				// bool to inform controller that execution for this cat is finished
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
				// if there are follower cats and main cat is moving, animation change to walking
				if (m_scriptData[id].catCurrentEnergy < m_scriptData[id].catMaxMovementEnergy)
				{
					// change to movement execute state
					m_scriptData[id].p_stateManager->ChangeState(new CatMovement_v2_0EXECUTE{}, id);
					
					// reset and play movement animation
					m_scriptData[id].p_catAnimation->SetCurrentFrameIndex(0);
					if (id == m_mainCatID)
					{
						for (EntityID follower : (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers)
						{
							m_scriptData[follower].p_catAnimation->StopAnimation();
						}
					}
					m_scriptData[id].p_catAnimation->StopAnimation();
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
				if (id == m_mainCatID)
				{
					for (EntityID follower : (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers)
					{
						m_scriptData[follower].p_catAnimation->SetCurrentFrameIndex(0);
					}
				}
			}
		}

		if (id == m_mainCatID)
		{
			for (EntityID follower : (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers)
			{
				if (r_stateName == "MovementEXECUTE")
					PlayAnimation(follower, "Walk");
				else if (r_stateName == "AttackEXECUTE" || r_stateName == "PLANNING")
					PlayAnimation(follower, "Idle");
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
				m_scriptData[id].p_catAnimation->PlayAnimation();
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
	
	// ----- Audio Helper Functions ----- //
	void CatScript_v2_0::PlayDeathAudio(EnumCatType catType)
	{
		std::string soundPrefabPath = "AudioObject/";

		switch (catType)
		{
		case EnumCatType::MAINCAT:
			soundPrefabPath += "Cat Meowsalot Death SFX1.prefab"; // Meowsalot has only one death sound
			break;
		case EnumCatType::GREYCAT:
		{
			int randomInteger = std::rand() % 3 + 1; // Randomize between 1 and 3
			soundPrefabPath += "Cat Grey Death SFX" + std::to_string(randomInteger) + ".prefab";
		}
		break;
		case EnumCatType::ORANGECAT:
		{
			int randomInteger = std::rand() % 3 + 1; // Randomize between 1 and 3
			soundPrefabPath += "Cat Orange Death SFX" + std::to_string(randomInteger) + ".prefab";
		}
		break;
		default:
			return;
		}

		PE::GlobalMusicManager::GetInstance().PlaySFX(soundPrefabPath, false);

		// DEBUGHANS @PR-ER
		//std::cout << "[Debug] Playing cat death audio: " << soundPrefabPath << std::endl;
	}

	void CatScript_v2_0::PlayPathPlacementAudio()
	{
		std::string soundPrefabPath = "AudioObject/Movement Planning SFX.prefab";
		PE::GlobalMusicManager::GetInstance().PlaySFX(soundPrefabPath, false);

		// DEBUGHANS @PR-ER
		//std::cout << "[Debug] Playing path placement audio: " << soundPrefabPath << std::endl;
	}

	void CatScript_v2_0::PlayFootstepAudio()
	{
		int randNum = (std::rand() % 3) + 1;								//random number 1to3
		std::string soundPrefabPath = "AudioObject/Cat Movement SFX ";
		soundPrefabPath += std::to_string(randNum) + ".prefab";

		PE::GlobalMusicManager::GetInstance().PlaySFX(soundPrefabPath, false);

		// DEBUGHANS @PR-ER
		//std::cout << "[Debug] Playing footstep audio: " << soundPrefabPath << std::endl;
	}

	void CatScript_v2_0::PlayCatAttackAudio(EnumCatType catType)
	{
		std::string soundPrefabPath = "AudioObject/Cat ";
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		int randSound = std::rand() % 5 + 1; // Random number between 1 and 5

		switch (catType)
		{
		case EnumCatType::MAINCAT:
			soundPrefabPath += "Meowsalot Attack SFX" + std::to_string(randSound) + ".prefab";
			break;
		case EnumCatType::GREYCAT:
			soundPrefabPath += "Grey Attack SFX" + std::to_string(randSound) + ".prefab";
			break;
		case EnumCatType::ORANGECAT:
			soundPrefabPath += "Orange Attack SFX" + std::to_string(randSound) + ".prefab";
			break;
		default:
			return;
		}
		
		// DEBUGHANS @PR-ER
		//std::cout << "[Debug] Playing cat attack audio: " << soundPrefabPath << std::endl;

		PE::GlobalMusicManager::GetInstance().PlaySFX(soundPrefabPath, false);
	}

	void CatScript_v2_0::PlayRescueCatAudio(EnumCatType catType)
	{
		std::string soundPrefabPath = "AudioObject/Cat ";
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		int randSound = std::rand() % 2 + 1; // Random number between 1 and 2

		switch (catType)
		{
		case EnumCatType::GREYCAT:
			soundPrefabPath += "Grey Rescued SFX" + std::to_string(randSound) + ".prefab";
			break;
		case EnumCatType::ORANGECAT:
			soundPrefabPath += "Orange Rescued SFX" + std::to_string(randSound) + ".prefab";
			break;
		default:
			return;
		}

		// DEBUGHANS @PR-ER
		//std::cout << "[Debug] Playing cat rescue audio: " << soundPrefabPath << std::endl;

		PE::GlobalMusicManager::GetInstance().PlaySFX(soundPrefabPath, false);
	}
}