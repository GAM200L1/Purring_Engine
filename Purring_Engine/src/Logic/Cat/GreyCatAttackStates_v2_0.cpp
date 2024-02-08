/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackStates_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Attack Base class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"

#include "GreyCatAttackStates_v2_0.h"
#include "CatScript_v2_0.h"
#include "CatHelperFunctions.h"
#include "CatController_v2_0.h"

#include "Hierarchy/HierarchyManager.h"
#include "Physics/CollisionManager.h"

namespace PE
{

	// ----- ATTACK PLAN ----- //

	void GreyCatAttack_v2_0PLAN::Enter(EntityID id)
	{
		// retrieve pointer to the game  controller
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		// retrieves the data for the grey cat's attack
		p_attackData = &std::get<GreyCatAttackVariables>((GETSCRIPTDATA(CatScript_v2_0, id))->attackVariables);

		// subscribe to mouse click event for selecting attack telegraphs
		m_mouseClickEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, GreyCatAttack_v2_0PLAN::OnMouseClick, this);
		m_mouseReleaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, GreyCatAttack_v2_0PLAN::OnMouseRelease, this);
	}

	void GreyCatAttack_v2_0PLAN::Update(EntityID id, float deltaTime)
	{
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }

		vec2 cursorPosition{ CatHelperFunctions::GetCursorPositionInWorld() };

		bool collidingWithAnyTelegraph{ false };

		try
		{
			for (auto const& r_telegraph : p_attackData->telegraphIDs) // for every telegraph
			{
				AABBCollider const& r_telegraphCollider = std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(r_telegraph.second).colliderVariant);

				bool collidedWithTelegraph = PointCollision(r_telegraphCollider, cursorPosition);

				// check if the mouse is hovering any of the boxes, if yes, boxes should change color
				if (collidedWithTelegraph)
				{
					collidingWithAnyTelegraph = true;
					if (r_telegraph.first == p_attackData->attackDirection) { continue; }
					CatHelperFunctions::SetColor(r_telegraph.second, m_hoverColor);
					if (m_mouseClick) // selects an attack direction
					{
						// @TODO: Add select direction sfx
						p_attackData->attackDirection = r_telegraph.first;
						(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = true;
						CatHelperFunctions::SetColor(r_telegraph.second, m_selectColor);
					}
				}
				else // if not hovering any telegraphs, set to default color
				{
					if (r_telegraph.first == p_attackData->attackDirection) { continue; }
					CatHelperFunctions::SetColor(r_telegraph.second, m_defaultColor);
				}
			}

			// disables telegraphs if anywhere but the telegraphs are clicked
			if (m_mouseClick && !collidingWithAnyTelegraph)
			{
				(GETSCRIPTDATA(CatScript_v2_0, id))->planningAttack = false;
				ToggleTelegraphs(false, true);
			}
		}
		catch (...) {} // colliders are not correct

		m_mouseClickedPrevious = m_mouseClick;
	}

	void GreyCatAttack_v2_0PLAN::CleanUp()
	{
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseClickEventListener);
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseReleaseEventListener);
	}

	void GreyCatAttack_v2_0PLAN::Exit(EntityID id)
	{
		// toggles all telegraphs except the selected one to false
		ToggleTelegraphs(false, false);
	}

	void GreyCatAttack_v2_0PLAN::ResetSelection(EntityID id)
	{
		m_showTelegraphs = false;
		for (auto const& r_telegraphs : p_attackData->telegraphIDs)
		{
			CatHelperFunctions::SetColor(r_telegraphs.second, m_defaultColor);
		}
		p_attackData->attackDirection = EnumCatAttackDirection_v2_0::NONE;
		(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = false;
	}

	void GreyCatAttack_v2_0PLAN::CreateProjectileTelegraphs(EntityID id, float bulletRange, std::map<EnumCatAttackDirection_v2_0, EntityID>& r_telegraphIDs)
	{
		auto CreateOneTelegraph =
			[&](bool isXAxis, bool isNegative)
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
				telegraphTransform.width = catTransform.width * bulletRange;
				EnumCatAttackDirection_v2_0 dir;
				AABBCollider telegraphCollider;

				// Set the dimensions of the telegraph based on the axis it's on
				if (isXAxis)
				{
					telegraphTransform.relPosition.x = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);
					dir = (isNegative) ? EnumCatAttackDirection_v2_0::WEST : EnumCatAttackDirection_v2_0::EAST;
				}
				else
				{
					telegraphTransform.relOrientation = PE_PI * 0.5f;
					telegraphTransform.relPosition.y = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);

					telegraphCollider.scaleOffset.x = telegraphTransform.height / telegraphTransform.width;
					telegraphCollider.scaleOffset.y = telegraphTransform.width / telegraphTransform.height;

					dir = (isNegative) ? EnumCatAttackDirection_v2_0::SOUTH : EnumCatAttackDirection_v2_0::NORTH;
				}

				// Configure the collider
				EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = telegraphCollider;
				EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;

				r_telegraphIDs.emplace(dir, telegraphID);
			};

		CreateOneTelegraph(true, false); // east
		CreateOneTelegraph(true, true); // west
		CreateOneTelegraph(false, false); // north
		CreateOneTelegraph(false, true); // south
	}

	void GreyCatAttack_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		if (MBPE.button != 1)
			m_mouseClick = true;
	}

	void GreyCatAttack_v2_0PLAN::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		MouseButtonReleaseEvent MBRE = dynamic_cast<const MouseButtonReleaseEvent&>(r_ME);
		m_mouseClick = false;
	}

	void GreyCatAttack_v2_0PLAN::ToggleTelegraphs(bool setToggle, bool ignoreSelected)
	{
		for (auto const& r_telegraph : p_attackData->telegraphIDs)
		{
			if (r_telegraph.first == p_attackData->attackDirection && ignoreSelected) { continue; } // don't disable attack that has been selected
			CatHelperFunctions::ToggleEntity(r_telegraph.second, setToggle);
		}
	}

	// ----- ATTACK EXECUTE ----- //

	void GreyCatAttack_v2_0EXECUTE::StateEnter(EntityID id)
	{
		// retrieves the data for the grey cat's attack
		m_catID = id;
		p_attackData = &std::get<GreyCatAttackVariables>((GETSCRIPTDATA(CatScript_v2_0, id))->attackVariables);

		// Subscribe to event
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnCollisionEnter, GreyCatAttack_v2_0EXECUTE::ProjectileCollided, this);

		// Set the start values of the attack projectile
		m_bulletImpulse = vec2{ 0.f, 0.f };
		if (GETSCRIPTDATA(CatScript_v2_0, id).attackSelected)
		{
			vec2 direction{ 0.f, 0.f };
			switch (p_attackData->attackDirection)
			{
			case EnumCatAttackDirection_v2_0::EAST:
			{
				direction.x = 1.f;
				break;
			}
			case EnumCatAttackDirection_v2_0::NORTH:
			{
				direction.y = 1.f;
				break;
			}
			case EnumCatAttackDirection_v2_0::SOUTH:
			{
				direction.y = -1.f;
				break;
			}
			case EnumCatAttackDirection_v2_0::WEST:
			{
				direction.x = -1.f;
				break;
			}
			}

			CatHelperFunctions::PositionEntity(p_attackData->projectileID, CatHelperFunctions::GetEntityPosition(id));
			EntityManager::GetInstance().Get<RigidBody>(p_attackData->projectileID).velocity.Zero();
			m_bulletImpulse = direction * p_attackData->bulletForce;
			m_bulletDelay = p_attackData->bulletDelay;
			m_bulletLifetime = p_attackData->bulletLifeTime;
		}
	}

	void GreyCatAttack_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{
		GameStateController_v2_0* p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }

		// if projectile has been fired, keep reducing lifetime and disable bullet

		if (m_projectileFired)
		{
			if (m_bulletLifetime <= 0.f)
			{
				(GETSCRIPTDATA(CatScript_v2_0, id))->finishedExecution = true;
				CatHelperFunctions::ToggleEntity(p_attackData->projectileID, false);
				EntityManager::GetInstance().Get<RigidBody>(p_attackData->projectileID).ZeroForce();
				EntityManager::GetInstance().Get<RigidBody>(p_attackData->projectileID).velocity.Zero();
			}
			m_bulletLifetime -= deltaTime;
		}

		// when the frame is attack frame, shoot the projectile after delay passes											//@TODO UNCOMMENT
		if (!(GETSCRIPTDATA(CatScript_v2_0, id))->finishedExecution && (GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected)// && EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == p_attackData->bulletFireAnimationIndex)
		{
			if (m_bulletDelay <= 0.f) // extra delay after the frame in case of slight inaccuracy
			{
				vec2 newScale{ CatHelperFunctions::GetEntityScale(id) };
				newScale.x = std::abs(newScale.x) * (((CatHelperFunctions::GetEntityPosition(p_attackData->telegraphIDs[p_attackData->attackDirection]) -
					CatHelperFunctions::GetEntityPosition(id)).Dot(vec2{ 1.f, 0.f }) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the rat is facing left
				CatHelperFunctions::ScaleEntity(id, newScale.x, newScale.y);
				CatHelperFunctions::ToggleEntity(p_attackData->projectileID, true);
				EntityManager::GetInstance().Get<RigidBody>(p_attackData->projectileID).ApplyLinearImpulse(m_bulletImpulse);
				m_projectileFired = true;
				// @TODO: play attack audio here

			}
			else
				m_bulletDelay -= deltaTime;
		}
		std::cout << (GETSCRIPTDATA(CatScript_v2_0, id))->finishedExecution << ' ';
	}

	void GreyCatAttack_v2_0EXECUTE::StateCleanUp()
	{
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
	}

	void GreyCatAttack_v2_0EXECUTE::StateExit(EntityID id)
	{
		p_attackData->attackDirection = EnumCatAttackDirection_v2_0::NONE;
		(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = false;
		CatHelperFunctions::ToggleEntity(p_attackData->projectileID, false);
	}

	void GreyCatAttack_v2_0EXECUTE::ProjectileCollided(const Event<CollisionEvents>& r_CE)
	{
		auto IsCatAndNotCaged = 
			[&](EntityID id)
			{
				if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(id))
				{
					if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(id))
						return false;
					else
						return true;
				}
				else
					return false;
			};

		if (r_CE.GetType() == CollisionEvents::OnCollisionEnter)
		{
			OnCollisionEnterEvent OCEE = dynamic_cast<const OnCollisionEnterEvent&>(r_CE);
			// @TODO: UNCOMMENT
			//if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetCurrentLevel() != 0) // check if hit cat for friendly fire
			{
				if (OCEE.Entity1 == p_attackData->projectileID && IsCatAndNotCaged(OCEE.Entity2))
					CatController_v2_0::KillCat(OCEE.Entity2);
				else if (OCEE.Entity2 == p_attackData->projectileID && IsCatAndNotCaged(OCEE.Entity1))
					CatController_v2_0::KillCat(OCEE.Entity1);
				if ((GETSCRIPTDATA(CatScript_v2_0, m_catID))->catType != EnumCatType::MAINCAT)
				{
					EntityManager::GetInstance().Get<RigidBody>(p_attackData->projectileID).ZeroForce();
					EntityManager::GetInstance().Get<RigidBody>(p_attackData->projectileID).velocity.Zero();
					CatHelperFunctions::ToggleEntity(p_attackData->projectileID, false);
					(GETSCRIPTDATA(CatScript_v2_0, m_catID))->finishedExecution = true;
				}
			}

			// kill rat

		}
	}
}