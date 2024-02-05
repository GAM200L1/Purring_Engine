/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatScript.cpp
 \date     15-11-2023

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
#include "CatScript.h"
#include "CatAttackScript.h"
#include "CatMovementScript.h"
#include "Data/SerializationManager.h"
#include "ResourceManager/ResourceManager.h"
#include "Graphics/CameraManager.h"
#include "Hierarchy/HierarchyManager.h"


namespace PE
{
	int CatScript::m_catEnergy{};  // Current energy level of the cat
	int CatScript::m_catMaxEnergy{ 21 }, CatScript::m_catBaseMaxEnergy{ 21 }; // Maximum energy of the cat
	EntityID CatScript::m_mainCatId{ 0 }; // ID of the main cat


	// ----- Destructor ----- //
	CatScript::~CatScript()
	{
			for (auto& [key, value] : m_scriptData)
			{
					delete value.p_stateManager;
			}
	}

	// ----- Public Functions ----- //
	void CatScript::Init(EntityID id)
	{
		m_scriptData[id].catID = id;
		if (m_scriptData[id].isMainCat) { // Set this as the main cat. Note that only one cat can be the main cat at a time
				CatScript::SetMainCat(id);
		}

		// Make a statemanager and set the starting state
		MakeStateManager(id);

		// Creates entities for the telegraph boxes
		CreateAttackTelegraphs(id, true, false); // east box
		CreateAttackTelegraphs(id, true, true); // west box
		CreateAttackTelegraphs(id, false, false); // north box
		CreateAttackTelegraphs(id, false, true); // south box

		// Create a projectile
		CreateProjectile(id);
		
		// Create as many entities to visualise the player path nodes  
		// such that there is one node per energy level
		if (m_scriptData[id].isMainCat)
		{
			m_scriptData[id].pathPositions.reserve(CatScript::GetMaximumEnergyLevel());
			m_scriptData[id].pathQuads.reserve(CatScript::GetMaximumEnergyLevel());
			for (std::size_t i{}; i < CatScript::GetMaximumEnergyLevel(); ++i)
			{
				CreatePathNode(id);
			}
		}
	}

	void CatScript::Update(EntityID id, float deltaTime)
	{
		GameStateController_v2_0* gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		//if (GameStateManager::GetInstance().GetGameState() == GameStates::SPLASHSCREEN) { return; } // don't allow cat script to update during splashscreen gamestate
		if (gsc->currentState == GameStates_v2_0::WIN || gsc->currentState == GameStates_v2_0::LOSE)
		{
			for (auto [direction, telegraph] : m_scriptData[id].telegraphIDs)
			{
				ToggleEntity(telegraph, false);
			}
			for (auto quad : m_scriptData[id].pathQuads)
			{
				ToggleEntity(quad, false);
			}
			ToggleEntity(m_scriptData[id].projectileID, false);
			return;
		}

		// Check if the player has died
		if (m_scriptData[id].catHealth <= 0 && m_scriptData[id].isMainCat)
		{
			//GameStateManager::GetInstance().noPause = true;

			for (auto quad : m_scriptData[id].pathQuads)
			{
				CatScript::ToggleEntity(quad, false);
			}

			// Set game state to lose when player HP is 0
			// probably some DT stuff to let the animation run
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				try
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != m_scriptData[id].animationStates.at("Death"))
						EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Death"));
				}
				catch (...)
				{
					// error
				}

				// death animation example
				if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationMaxIndex())
				{
					ToggleEntity(id, false);
				}
			}

			gsc->LoseGame();
			ToggleEntity(id, false);
			return;
		}

		// Ensure that the state manager exists
		if (!m_scriptData[id].p_stateManager) 
		{
			m_scriptData[id].catID = id;

			// Make a statemanager and set the starting state
			MakeStateManager(id);
		}

		m_scriptData[id].p_stateManager->Update(id, deltaTime);


		// --- ANIMATE THE CAT BASED ON ITS STATE --- // 

		if (m_scriptData[id].p_stateManager->GetStateName() == "MovementPLAN")
		{ 
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				try
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != m_scriptData[id].animationStates.at("Idle"))
						EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Idle"));
				}
				catch (...)
				{
					// error
				}
			}
			// If current gamestate is set to attack planning, change state to CatAttackPLAN
			if (gsc->currentState == GameStates_v2_0::ATTACK)
			{
				TriggerStateChange(id); // immediate state change
				if (CheckShouldStateChange(id, deltaTime))
				{
					m_scriptData[id].finishedExecution = false;
					m_scriptData[id].p_stateManager->ChangeState(new CatAttackPLAN{}, id);
				}
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "AttackPLAN")
		{
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				try
				{
					if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != m_scriptData[id].animationStates.at("Idle"))
						EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Idle"));
				}
				catch (...)
				{
					// error
				}
			}
			// Check if the state should be changed
			if (gsc->currentState == GameStates_v2_0::PAUSE)
			{
				TriggerStateChange(id); // immediate state change
				if (CheckShouldStateChange(id, deltaTime)) 
				{
					m_scriptData[id].finishedExecution = false;
					m_scriptData[id].p_stateManager->ChangeState(new CatMovementEXECUTE{}, id);
				}
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "MovementEXECUTE")
		{
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				try
				{ 
					EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Walk"));
				}
				catch (...)
				{
					// error
				}
			}			

			// If this is not the main cat, check if the state should be changed
			if(id != CatScript::GetMainCat() && m_scriptData[CatScript::GetMainCat()].p_stateManager->GetStateName() != "MovementEXECUTE")
			{
					m_scriptData[id].p_stateManager->ChangeState(new CatAttackEXECUTE{}, id);
			}

			if (id == CatScript::GetMainCat() && CheckShouldStateChange(id, deltaTime))
			{
				// trigger state change called in MovementEXECUTE state update
				m_scriptData[id].finishedExecution = false;
				m_scriptData[id].p_stateManager->ChangeState(new CatAttackEXECUTE{}, id);
			}
		}
		else if (m_scriptData[id].p_stateManager->GetStateName() == "AttackEXECUTE")
		{
			// cycle the cat animations
			if (EntityManager::GetInstance().Has<AnimationComponent>(id))
			{
				if (m_scriptData[id].finishedExecution)
				{
					try
					{
						if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationID() != m_scriptData[id].animationStates.at("Idle"))
							EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Idle"));
					}
					catch (...)
					{
						// error
					}
				}
				else
				{
					try
					{
						if(m_scriptData[id].attackDirection != EnumCatAttackDirection::NONE)
							EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Attack"));
						else
							EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentAnimationID(m_scriptData[id].animationStates.at("Idle"));
					}
					catch (...)
					{
						// error
					}
				}
			}
			if (gsc->currentState == GameStates_v2_0::PLANNING)
			{
				TriggerStateChange(id); // immediate state change
				if (CheckShouldStateChange(id, deltaTime))
				{
					m_scriptData[id].p_stateManager->ChangeState(new CatMovementPLAN{}, id);
				}
			}
		}
	}

	void CatScript::OnAttach(EntityID id)
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
			m_scriptData[id] = CatScriptData{};
		}
		else
		{
			delete m_scriptData[id].p_stateManager;
			m_scriptData[id] = CatScriptData{};
		}

		// Reset values
		m_scriptData[id].shouldChangeState = false;
		m_scriptData[id].timeBeforeChangingState = 0.f;

		// Set the cat max energy to the value set in the editor
		m_catBaseMaxEnergy = m_catMaxEnergy = m_scriptData[id].catMaxEnergy;
	}

	void CatScript::OnDetach(EntityID id)
	{

		if (m_scriptData.find(id) != m_scriptData.end())
		{
			delete m_scriptData[id].p_stateManager;
			m_scriptData.erase(id);
		}
	}		
		
	void CatScript::LoseHP(EntityID id, int damageTaken)
	{
		//if (!GameStateManager::GetInstance().godMode)
			m_scriptData[id].catHealth -= damageTaken;
		std::cout << "Cat HP: " << m_scriptData[id].catHealth << '\n';
	}
		
	void CatScript::MakeStateManager(EntityID id)
	{
		if (m_scriptData.count(id)) { 
			if(m_scriptData.at(id).p_stateManager)
			return; }

		m_scriptData[id].p_stateManager = new StateMachine{};
		m_scriptData[id].p_stateManager->ChangeState(new CatMovementPLAN{}, id);
	}


	void CatScript::TriggerStateChange(EntityID id, float const stateChangeDelay)
	{
		if (m_scriptData[id].delaySet) { return; } // Prevent the state from constantly resetting the delay
		
		// Flags that the state should change in [timeBeforeChangingState] seconds
		m_scriptData[id].shouldChangeState = true;
		m_scriptData[id].timeBeforeChangingState = stateChangeDelay;
		m_scriptData[id].delaySet = true;
	}	


	bool CatScript::CheckShouldStateChange(EntityID id, float const deltaTime)
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


	void CatScript::ToggleEntity(EntityID id, bool setToActive)
	{
			// Exit if the entity is not valid
			if (!EntityManager::GetInstance().IsEntityValid(id)) { return; }

		// Toggle the entity
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
	}


	void CatScript::PositionEntity(EntityID const transformId, vec2 const& r_position)
	{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.position = r_position;
			}
			catch (...) { return; }
	}	


	void CatScript::ScaleEntity(EntityID const transformId, float const width, float const height)
	{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				r_transform.width = width;
				r_transform.height = height;
			}
			catch (...) { return; }
	}	


	vec2 CatScript::GetEntityPosition(EntityID const transformId)
	{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				return r_transform.position;
			}
			catch (...) { return vec2{}; }
	}


	vec2 CatScript::GetEntityScale(EntityID const transformId)
	{
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(transformId) }; // Get the transform of the player
				return vec2{ r_transform.width, r_transform.height };
			}
			catch (...) { return vec2{}; }
	}


	vec2 CatScript::GetCursorPositionInWorld()
	{
			float mouseX{}, mouseY{};
			InputSystem::GetCursorViewportPosition(WindowManager::GetInstance().GetWindow(), mouseX, mouseY);
			return GETCAMERAMANAGER()->GetWindowToWorldPosition(mouseX, mouseY);
	}


	bool CatScript::IsCat(EntityID const id)
	{
			return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Cat") != std::string::npos);
	}


	bool CatScript::IsEnemy(EntityID const id)
	{
			return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Rat") != std::string::npos);
	}	


	bool CatScript::IsObstacle(EntityID const id)
	{
			return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Obstacle") != std::string::npos);
	}


	bool CatScript::DoesGameStateMatchCatState(std::string const& catStateName, GameStates_v2_0 gameState)
	{ 
			// Cat states: "MovementPLAN" "AttackPLAN" "MovementEXECUTE" "AttackEXECUTE"
			switch (gameState) {
			case GameStates_v2_0::PLANNING:
			{
					return catStateName == "MovementPLAN";
			}
			case GameStates_v2_0::ATTACK:
			{
					return catStateName == "AttackPLAN";
			}
			case GameStates_v2_0::EXECUTE:
			{
					return catStateName == "MovementEXECUTE" || catStateName == "AttackEXECUTE";
			}
			default: // PAUSE, WIN, LOSE, INACTIVE or SPLASHSCREEN states
			{ 
					return false;
			}
			}
	}


	void CatScript::CreateAttackTelegraphs(EntityID id, bool isXAxis, bool isNegative)
	{
		Transform const& catTransform = EntityManager::GetInstance().Get<Transform>(id);

		SerializationManager serializationManager;

		EntityID telegraphID = serializationManager.LoadFromFile("PlayerAttackTelegraph_Prefab.json");
		Transform& telegraphTransform = EntityManager::GetInstance().Get<Transform>(telegraphID);

		//EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).parent = id; // telegraph follows the cat entity
		Hierarchy::GetInstance().AttachChild(id, telegraphID); // new way of attatching parent child
		telegraphTransform.relPosition.Zero();
		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).isActive = false; // telegraph to not show until attack planning
		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).toSave = false; // telegraph to not show until attack planning
		

		// set size of telegraph
		telegraphTransform.height = catTransform.height * 0.75f;
		telegraphTransform.width = catTransform.width * m_scriptData[id].bulletRange;
		EnumCatAttackDirection dir;
		AABBCollider telegraphCollider;

		// Set the dimensions of the telegraph based on the axis it's on
		if (isXAxis)
		{
			telegraphTransform.relPosition.x = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);
			dir = (isNegative) ? EnumCatAttackDirection::WEST : EnumCatAttackDirection::EAST;
		}
		else
		{
			telegraphTransform.relOrientation = PE_PI * 0.5f;
			telegraphTransform.relPosition.y = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);

			telegraphCollider.scaleOffset.x = telegraphTransform.height / telegraphTransform.width;
			telegraphCollider.scaleOffset.y = telegraphTransform.width / telegraphTransform.height;

			dir = (isNegative) ? EnumCatAttackDirection::SOUTH : EnumCatAttackDirection::NORTH;
		}

		// Configure the collider
		EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = telegraphCollider;
		EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;

		m_scriptData[id].telegraphIDs.emplace(dir, telegraphID);
	}


	void CatScript::CreateProjectile(EntityID id)
	{
			// Creates an entity for the projectile
			SerializationManager serializationManager;
			m_scriptData[id].projectileID = serializationManager.LoadFromFile("Projectile_Prefab.json");
			EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].projectileID).isActive = false;
	}


	void CatScript::CreatePathNode(EntityID id)
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

}