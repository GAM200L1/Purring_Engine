/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     OrangeCatAttackStates_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the definitions for the functions for the Orange Cat Attack class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"

#include "Hierarchy/HierarchyManager.h"
#include "Physics/CollisionManager.h"
#include "Animation/Animation.h"
#include "ResourceManager/ResourceManager.h"

#include "OrangeCatAttackStates_v2_0.h"
#include "CatController_v2_0.h"
#include "Logic/Rat/RatController_v2_0.h"
#include "CatScript_v2_0.h"
#include "CatHelperFunctions.h"
#include "Logic/Boss/BossRatScript.h"

namespace PE
{
	// ----- Create Orange Cat Stomp and Telegraphs ----- //
	void OrangeCatAttackVariables::CreateSeismicAndTelegraph(EntityID catID)
	{
		Transform const& catTransform = EntityManager::GetInstance().Get<Transform>(catID);

		// create seismic //
		SerializationManager serializationManager;
		seismicID = serializationManager.LoadFromFile("Seismic.prefab");
		
		/*EntityManager::GetInstance().Get<EntityDescriptor>(seismicID).layer = 0;
		EntityManager::GetInstance().Get<Collider>(seismicID).collisionLayerIndex = 0;*/
		CircleCollider& r_seismicCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(seismicID).colliderVariant);
		r_seismicCollider.scaleOffset = 0.25f;

		//Hierarchy::GetInstance().GetInstance().AttachChild(catID, seismicID);
		//EntityManager::GetInstance().Get<Transform>(seismicID).relPosition.Zero();
		
		CatHelperFunctions::ToggleEntity(seismicID, false);
		EntityManager::GetInstance().Get<EntityDescriptor>(seismicID).toSave = false;
		
		CatHelperFunctions::ScaleEntity(seismicID, catTransform.width * seismicRadius, catTransform.height * seismicRadius);
		
		// create telegraph //
		telegraphID = serializationManager.LoadFromFile("OrangeCatAttackTelegraph.prefab");

		Hierarchy::GetInstance().AttachChild(catID, telegraphID);
		EntityManager::GetInstance().Get<Transform>(telegraphID).relPosition.Zero();

		CatHelperFunctions::ToggleEntity(telegraphID, false); // telegraph to not show until attack planning
		EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).toSave = false;

		CatHelperFunctions::ScaleEntity(telegraphID, catTransform.width * seismicRadius, catTransform.height * seismicRadius);

		EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = CircleCollider();
		EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;
	}

	// ----- ATTACK PLAN ----- //

	void OrangeCatAttack_v2_0PLAN::Enter(EntityID id)
	{
		// retrieve pointer to game controller
		p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		// retrieves the data for the grey cat's attack
		p_attackData = &std::get<OrangeCatAttackVariables>((GETSCRIPTDATA(CatScript_v2_0, id))->attackVariables);

		// subscribe to mouse click event for selecting attack telegraphs
		m_mouseClickEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, OrangeCatAttack_v2_0PLAN::OnMouseClick, this);
		m_mouseReleaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, OrangeCatAttack_v2_0PLAN::OnMouseRelease, this);
	}

	void OrangeCatAttack_v2_0PLAN::Update(EntityID id, float deltaTime)
	{
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }

		CircleCollider const& r_telegraphCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(p_attackData->telegraphID).colliderVariant);
		CircleCollider const& r_catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
		
		vec2 cursorPosition{ CatHelperFunctions::GetCursorPositionInWorld() };

		bool collidingWithTelegraph = PointCollision(r_telegraphCollider, cursorPosition);
		bool collidingWithCat = PointCollision(r_catCollider, cursorPosition);

		if (collidingWithTelegraph && !collidingWithCat)
		{
			CatHelperFunctions::SetColor(p_attackData->telegraphID, m_hoverColor);
			if (m_mouseClick)
			{
				(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = true;
				CatHelperFunctions::SetColor(p_attackData->telegraphID, m_selectColor);
			}
		}
		else
		{
			if (!(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected)
				CatHelperFunctions::SetColor(p_attackData->telegraphID, m_defaultColor);
		}

		if (m_mouseClick && !m_mouseClickedPrevious && !collidingWithTelegraph)
		{
			(GETSCRIPTDATA(CatScript_v2_0, id))->planningAttack = false;

			if (!(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected)
				ToggleTelegraphs(false, false);
		}

		m_mouseClickedPrevious = m_mouseClick;
	}

	void OrangeCatAttack_v2_0PLAN::CleanUp()
	{
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseClickEventListener);
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseReleaseEventListener);
	}

	void OrangeCatAttack_v2_0PLAN::Exit(EntityID id)
	{
		ToggleTelegraphs(false, false);
	}

	void OrangeCatAttack_v2_0PLAN::ResetSelection(EntityID id)
	{
		CatHelperFunctions::SetColor(p_attackData->telegraphID, m_defaultColor);
		(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = false;
	}

	void OrangeCatAttack_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		if (MBPE.button != 1)
			m_mouseClick = true;
	}

	void OrangeCatAttack_v2_0PLAN::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		MouseButtonReleaseEvent MBRE = dynamic_cast<const MouseButtonReleaseEvent&>(r_ME);
		m_mouseClick = false;
	}

	void OrangeCatAttack_v2_0PLAN::ToggleTelegraphs(bool setToggle, bool ignoreSelected)
	{
		ignoreSelected;
		CatHelperFunctions::ToggleEntity(p_attackData->telegraphID, setToggle);
	}

	// ----- ATTACK EXECUTE ----- //

	void OrangeCatAttack_v2_0EXECUTE::StateEnter(EntityID id)
	{
		// stores ID of the cat
		m_catID = id;
		
		// @TODO: remove this temp solution
		EntityManager::GetInstance().Get<Collider>(id).isTrigger = true;

		// retrieves the data for the grey cat's attack
		p_attackData = &std::get<OrangeCatAttackVariables>((GETSCRIPTDATA(CatScript_v2_0, id))->attackVariables);

		// set seismic to where cat is
		CatHelperFunctions::PositionEntity(p_attackData->seismicID, CatHelperFunctions::GetEntityPosition(id));

		// subscribes to collision events
		m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnCollisionEnter, OrangeCatAttack_v2_0EXECUTE::SeismicCollided, this);

		m_seismicDelay = p_attackData->seismicDelay;

		// reset animation
		EntityManager::GetInstance().Get<AnimationComponent>(p_attackData->seismicID).SetCurrentFrameIndex(0);
		m_attackLifetime = ResourceManager::GetInstance().GetAnimation(EntityManager::GetInstance().Get<AnimationComponent>(p_attackData->seismicID).GetAnimationID())->GetAnimationDuration();
	}

	void OrangeCatAttack_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{
		GameStateController_v2_0* p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		CatScript_v2_0Data* p_catData = GETSCRIPTDATA(CatScript_v2_0, id);
		if (p_gsc->currentState == GameStates_v2_0::PAUSE || !p_catData->attackSelected) { return; }

		AnimationComponent& r_seismicAnimation = EntityManager::GetInstance().Get<AnimationComponent>(p_attackData->seismicID);

		if (m_seismicSlammed)
		{	
			if (m_attackLifetime <= 0.f)
			{
				p_catData->finishedExecution = true;
				CatHelperFunctions::ToggleEntity(p_attackData->seismicID, false);
			}
			else
			{
				CircleCollider& r_seismicCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(p_attackData->seismicID).colliderVariant);

				if (m_seismicPrevAnimationFrame != r_seismicAnimation.GetCurrentFrameIndex() && r_seismicCollider.scaleOffset < 1.f)
					r_seismicCollider.scaleOffset += 0.25f;

				m_seismicPrevAnimationFrame = r_seismicAnimation.GetCurrentFrameIndex();
				m_attackLifetime -= deltaTime;
			}
		}
		// if seismic is not yet done, attack is selected, and animation is at the point where seismic would play
		else if (!p_catData->finishedExecution && EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == p_attackData->seismicSlamAnimationIndex)
		{
			if (m_seismicDelay <= 0.f)
			{
				CatHelperFunctions::ToggleEntity(p_attackData->seismicID, true);
				m_seismicSlammed = true;
				r_seismicAnimation.SetCurrentFrameIndex(0);
				CatScript_v2_0::PlayCatAttackAudio(EnumCatType::ORANGECAT);
			}
			else 
			{
				m_seismicDelay -= deltaTime;
			}
		}
	}

	void OrangeCatAttack_v2_0EXECUTE::StateCleanUp()
	{
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
	}

	void OrangeCatAttack_v2_0EXECUTE::StateExit(EntityID id)
	{
		(GETSCRIPTDATA(CatScript_v2_0, id))->attackSelected = false;
		(GETSCRIPTDATA(CatScript_v2_0, id))->finishedExecution = false;
		m_seismicSlammed = false;
		CatHelperFunctions::ToggleEntity(p_attackData->seismicID, false);
		// @TODO remove this temp solution
		EntityManager::GetInstance().Get<Collider>(id).isTrigger = false;
	}

	void OrangeCatAttack_v2_0EXECUTE::SeismicCollided(const Event<CollisionEvents>& r_CE)
	{
		if (r_CE.GetType() == CollisionEvents::OnCollisionEnter)
		{
			OnCollisionEnterEvent OCEE = dynamic_cast<const OnCollisionEnterEvent&>(r_CE);

			if (OCEE.Entity1 != m_catID && OCEE.Entity2 != m_catID)
			{
				// if the seismic hits a cat or rat
				SeismicHitCatOrRat(OCEE.Entity1, OCEE.Entity2);
			}
		}
	}

	void OrangeCatAttack_v2_0EXECUTE::SeismicHitCatOrRat(EntityID id1, EntityID id2)
	{
		CatController_v2_0* p_catController = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
		// kill cat if it is not following and not in cage and projectile hits catif (id1 == p_attackData->seismicID && GETSCRIPTINSTANCEPOINTER(RatController_v2_0)->IsRatAndIsAlive(id2))
		if (id1 == p_attackData->seismicID)
		{
			if (GETSCRIPTINSTANCEPOINTER(RatController_v2_0)->IsRatAndIsAlive(id2))
			{
				GETSCRIPTINSTANCEPOINTER(RatController_v2_0)->ApplyDamageToRat(id2, id1, p_attackData->damage);
				return;
			}
			else if (p_catController->IsCatAndNotCaged(id2) && !p_catController->IsFollowCat(id2))
			{
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(id2);
				return;
			}
			else if (id2 == GETSCRIPTINSTANCEPOINTER(BossRatScript)->currentBoss)
			{
				GETSCRIPTINSTANCEPOINTER(BossRatScript)->TakeDamage(p_attackData->damage);
			}
		}
		else if (id2 == p_attackData->seismicID)
		{
			if (GETSCRIPTINSTANCEPOINTER(RatController_v2_0)->IsRatAndIsAlive(id1))
			{
				GETSCRIPTINSTANCEPOINTER(RatController_v2_0)->ApplyDamageToRat(id1, id2, p_attackData->damage);
				return;
			}
			else if (p_catController->IsCatAndNotCaged(id1) && !p_catController->IsFollowCat(id1))
			{
				GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(id1);
				return;
			}
			else if (id1 == GETSCRIPTINSTANCEPOINTER(BossRatScript)->currentBoss)
			{
				GETSCRIPTINSTANCEPOINTER(BossRatScript)->TakeDamage(p_attackData->damage);
			}
		}
	}
}