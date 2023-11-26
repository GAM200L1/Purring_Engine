/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatAttackScript.cpp
 \date     15-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains definitions for functions used for a grey cat's attack state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "Physics/CollisionManager.h"
#include "CatAttackScript.h"
#include "CatMovementScript.h"
#include "RatScript.h"

namespace PE
{
	// ----- CAT ATTACK PLAN STATE ----- //

	void CatAttackPLAN::StateEnter(EntityID id)
	{
		std::cout << "CatAttackPLAN::StateEnter(" << id << ")\n";
		p_data = GETSCRIPTDATA(CatScript, id);
		EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);
		m_checkedIgnored = false;
		m_ignoresTelegraphs.clear();

		// Don't bother if not the main cat and not following the main cat
		if (p_data->catID != CatScript::GetMainCat() && !p_data->isFollowing) { return; }

		for (auto const& [attackDirection, boxID] : p_data->telegraphIDs)
		{
			CatScript::ToggleEntity(boxID, true);
			EntityManager::GetInstance().Get<Graphics::Renderer>(boxID).SetColor(m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, 0.f);
		}
		
		m_triggerEnterEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnTriggerEnter, CatAttackPLAN::CatInTelegraph, this);
		m_triggerStayEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnTriggerStay, CatAttackPLAN::CatInTelegraph, this);
		m_mouseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, CatAttackPLAN::OnMouseClick, this);

	}
	
	void CatAttackPLAN::StateUpdate(EntityID id, float deltaTime)
	{
		// Skip this if it's the pause state
		if (GameStateManager::GetInstance().GetGameState() == GameStates::PAUSE) { return; }
		
	  // Don't bother if not the main cat and not following the main cat
		if (p_data->catID != CatScript::GetMainCat() && !p_data->isFollowing) { return; }
		
		// get the mouse cursor position
		vec2 cursorPosition{ CatScript::GetCursorPositionInWorld() };

		// if havent checked if the telegraphs are intersecting with the cats, go over this update (it should check after)
		if (!m_checkedIgnored) 
		{ 
			for (auto const& [attackDirection, boxID] : p_data->telegraphIDs)
			{
				CatScript::ToggleEntity(boxID, false);
				EntityManager::GetInstance().Get<Graphics::Renderer>(boxID).SetColor(m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, 1.f);
			}
			m_checkedIgnored = true;
			return; 
		}
		
		// if in attack planning phase, allow player to select a cat and plan that cats attacks
		if (GameStateManager::GetInstance().GetGameState() == GameStates::ATTACK)
		{
			CircleCollider const& catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);

			// Check if the telegraphs have already been enabled
			if (m_showBoxes)
			{
				// The telegraphs have been enabled 
				// Check if any of the telegraphs were clicked or are hovered over
				for (auto const& telegraph : p_data->telegraphIDs)
				{
					if (m_ignoresTelegraphs.count(telegraph.second)) { continue; }

					AABBCollider const& telegraphCollider = std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(telegraph.second).colliderVariant);
					// checks if the mouse hovers over any of the select boxes. if it does, the boxes should become red
					if (PointCollision(telegraphCollider, cursorPosition))
					{
						// sets hover color if telegraph is color
						EntityManager::GetInstance().Get<Graphics::Renderer>(telegraph.second).SetColor(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, 1.f);
						if (m_mouseClick)
						{
							std::cout << "CatAttackPLAN::StateUpdate() Clicked on telegraph " << telegraph.first << " on cat " << id << "\n";
							// if player selects either of the boxes, the attack direction is determined
							p_data->attackDirection = telegraph.first;
							// the other boxes should not show
							m_showBoxes = false;
							for (auto const& [direction, telegraphID] : p_data->telegraphIDs)
							{
								if (m_ignoresTelegraphs.count(telegraph.second)) { continue; }

								if (p_data->attackDirection == direction)
								{
									EntityManager::GetInstance().Get<Graphics::Renderer>(telegraph.second).SetColor(m_selectColor.x, m_selectColor.y, m_selectColor.z, 1.f);
								}
								else
								{
									EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).isActive = false;
								}
							}
							break;
						}
					}
					else
					{
						// if the mouse is not over any of the boxes, it should be green
						EntityManager::GetInstance().Get<Graphics::Renderer>(telegraph.second).SetColor(m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, 1.f); // sets the color of the box to be green if not hovering
					}
				}

				// if the mouse was clicked and no directions have been selected, disable everything
				if (m_mouseClick && p_data->attackDirection == EnumCatAttackDirection::NONE)
				{
					m_showBoxes = false;
					for (auto const& [atkDir, telegraphID] : p_data->telegraphIDs)
					{
						CatScript::ToggleEntity(telegraphID, false);
					}
				}

			}
			else // Telegraphs have not been enabled
			{
				// The telegraphs have not been enabled and the cat was clicked
				if (PointCollision(catCollider, cursorPosition) && m_mouseClick)
				{
					std::cout << "CatAttackPLAN::StateUpdate() Clicked on cat " << id << "\n";

					SerializationManager serializationManager;
					EntityID sound = serializationManager.LoadFromFile("../Assets/Prefabs/AudioObject/Cat Selection SFX_Prefab.json");
					if (EntityManager::GetInstance().Has<AudioComponent>(sound))
						EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
					EntityManager::GetInstance().RemoveEntity(sound);


					// if player selects cat with EntityID 'id', the cat will reset its attack choice and show its selectable attack boxes and become active
					m_showBoxes = true;
					p_data->attackDirection = EnumCatAttackDirection::NONE;

					// Disable telegraphs on all other cats
					auto catScriptDataMap{ GETSCRIPTINSTANCEPOINTER(CatScript)->m_scriptData };
					for (auto& scriptData : catScriptDataMap)
					{
						// Don't disable the telegraphs on this cat or cat that have selected directions
						if (scriptData.first == id || scriptData.second.attackDirection != EnumCatAttackDirection::NONE) { continue; }

						std::cout << "CatAttackPLAN::StateUpdate() Disabling telegraphs for " << scriptData.first << "\n";
						for (auto const& telegraph : scriptData.second.telegraphIDs)
						{
								CatScript::ToggleEntity(telegraph.second, false);
						}
					}

					// Activate telegraphs on this cat
					for (auto const& telegraph : p_data->telegraphIDs)
					{
						if (!m_ignoresTelegraphs.count(telegraph.second))
							CatScript::ToggleEntity(telegraph.second, true);
						else
							CatScript::ToggleEntity(telegraph.second, false);
					}
				}

				// If an attack directions has been selected
				if (p_data->attackDirection != EnumCatAttackDirection::NONE)
				{
					// Check if the player has clicked on the selected attack direction
					AABBCollider const& telegraphCollider = std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(p_data->telegraphIDs[p_data->attackDirection]).colliderVariant);
					if (PointCollision(telegraphCollider, cursorPosition) && m_mouseClick)
					{
						std::cout << "CatAttackPLAN::StateUpdate() Clicked on telegraph for cat " << id << " in direction " << p_data->attackDirection << "\n";
						m_showBoxes = true;
						p_data->attackDirection = EnumCatAttackDirection::NONE;
						for (auto const& telegraph : p_data->telegraphIDs)
						{
							if (!m_ignoresTelegraphs.count(telegraph.second))
								CatScript::ToggleEntity(telegraph.second, true);
							else
								CatScript::ToggleEntity(telegraph.second, false);
						}
					} // end of  if (PointCollision(telegraphCollider, cursorPosition) && m_mouseClick)
				}
			}	// end of 	if (m_showBoxes)
		}

		m_mouseClick = false;
	}
	
	void CatAttackPLAN::StateCleanUp()
	{
		std::cout << "CatAttackPLAN::StateCleanUp()\n";

	  // Don't bother if not the main cat and not following the main cat
		if (p_data->catID != CatScript::GetMainCat() && !p_data->isFollowing) { return; }
		
		REMOVE_KEY_COLLISION_LISTENER(m_triggerEnterEventListener);
		REMOVE_KEY_COLLISION_LISTENER(m_triggerStayEventListener);
		REMOVE_MOUSE_EVENT_LISTENER(m_mouseEventListener);
	}

	void CatAttackPLAN::StateExit(EntityID id)
	{
		std::cout << "CatAttackPLAN::StateExit(" << id << ")\n";
		for (auto const& telegraph : p_data->telegraphIDs)
		{
			// set the entity with p_attack direction to not active, the green box should disappear
			CatScript::ToggleEntity(telegraph.second, false);
			EntityManager::GetInstance().Get<Graphics::Renderer>(telegraph.second).SetColor(m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, 0.f);
		}
		
	}

	void CatAttackPLAN::ResetSelection()
	{
		m_showBoxes = false;
		p_data->attackDirection = EnumCatAttackDirection::NONE;
		for (auto const& [atkDir, telegraphID] : p_data->telegraphIDs)
		{
			EntityManager::GetInstance().Get<Graphics::Renderer>(telegraphID).SetColor(m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, 0.f);
			CatScript::ToggleEntity(telegraphID, false);
		}
	}

	void CatAttackPLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		m_mouseClickedPrevious = m_mouseClick;
		m_mouseClick = true;
	}

	void CatAttackPLAN::CatInTelegraph(const Event<CollisionEvents>& r_TE)
	{
		// only needs to check this once, and once it has been checked don't check again
		if (m_checkedIgnored) { return; }

		// Add any telegraphs overlapping with any cats to the m_ignoresTelegraphs map

		if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
		{
			OnTriggerEnterEvent OCEE = dynamic_cast<const OnTriggerEnterEvent&>(r_TE);
			for (auto const& [attackDir, telegraphID] : p_data->telegraphIDs)
			{
				if ((OCEE.Entity1 == telegraphID && CatScript::IsCat(OCEE.Entity2))
					|| (OCEE.Entity2 == telegraphID && CatScript::IsCat(OCEE.Entity1)))
				{
					m_ignoresTelegraphs.emplace(telegraphID);
				}
			}
		}
		else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
		{
			OnTriggerStayEvent OCSE = dynamic_cast<const OnTriggerStayEvent&>(r_TE);
			for (auto const& [attackDir, telegraphID] : p_data->telegraphIDs)
			{
				if ((OCSE.Entity1 == telegraphID && CatScript::IsCat(OCSE.Entity2))
						|| (OCSE.Entity2 == telegraphID && CatScript::IsCat(OCSE.Entity1)))
				{
					m_ignoresTelegraphs.emplace(telegraphID);
				}
			}
		}
		//m_checkedIgnored = true;
	}



	// ----- CAT ATTACK EXECUTION ----- //


	void CatAttackEXECUTE::StateEnter(EntityID id) 
	{
		std::cout << "CatAttackEXECUTE::StateEnter(" << id << ")\n";
		p_data = GETSCRIPTDATA(CatScript, id);
		EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);
		m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnCollisionEnter, CatAttackEXECUTE::ProjectileHitRat, this);
		m_bulletImpulse = vec2{ 0.f, 0.f };
		if (p_data->attackDirection != EnumCatAttackDirection::NONE)
		{
			m_attackDuration = p_data->bulletLifeTime;
			vec2 direction{ 0.f, 0.f };
			switch (p_data->attackDirection)
			{
			case EnumCatAttackDirection::EAST:
			{
				direction.x = 1.f;
				break;
			}
			case EnumCatAttackDirection::NORTH:
			{
				direction.y = 1.f;
				break;
			}
			case EnumCatAttackDirection::SOUTH:
			{
				direction.y = -1.f;
				break;
			}
			case EnumCatAttackDirection::WEST:
			{
				direction.x = -1.f;
				break;
			}
			}
			EntityManager::GetInstance().Get<Transform>(p_data->projectileID).position = EntityManager::GetInstance().Get<Transform>(id).position + (direction * ((0.5f * EntityManager::GetInstance().Get<Transform>(id).width) + 0.5f * EntityManager::GetInstance().Get<Transform>(p_data->projectileID).width));
			EntityManager::GetInstance().Get<RigidBody>(p_data->projectileID).velocity.Zero();
			m_bulletImpulse = direction * p_data->bulletForce;
			m_bulletDelay = p_data->bulletDelay;
		}
	}

	void CatAttackEXECUTE::StateUpdate(EntityID id, float deltaTime)
	{
		if (GameStateManager::GetInstance().GetGameState() == GameStates::PAUSE) { return; }
		if (p_data->attackDirection != EnumCatAttackDirection::NONE && !m_projectileFired)
		{
			// Ensure the rat is facing the direction of their attack
			vec2 newScale{ RatScript::GetEntityScale(id) };
			newScale.x = std::abs(newScale.x) * (((CatScript::GetEntityPosition(p_data->telegraphIDs[p_data->attackDirection]) - CatScript::GetEntityPosition(id)).Dot(vec2{1.f, 0.f}) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the rat is facing left
			RatScript::ScaleEntity(id, newScale.x, newScale.y);

			// show the projectile
			CatScript::ToggleEntity(p_data->projectileID, true);
			EntityManager::GetInstance().Get<RigidBody>(p_data->projectileID).ApplyLinearImpulse(m_bulletImpulse);

			if (!playShootOnce)
			{
				SerializationManager serializationManager;
				EntityID sound = serializationManager.LoadFromFile("../Assets/Prefabs/AudioObject/Cat Attack SFX_Prefab.json");
				if (EntityManager::GetInstance().Has<AudioComponent>(sound))
					EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
				EntityManager::GetInstance().RemoveEntity(sound);
				playShootOnce = true;
			}
			m_projectileFired = true;
		}
		if (m_attackDuration > 0.f && !m_bulletCollided)
		{
			m_attackDuration -= deltaTime;
		}
		else
		{
			if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationMaxIndex())
			{
				p_data->finishedExecution = true;
				m_bulletCollided = false;
				m_projectileFired = false;
				CatScript::ToggleEntity(p_data->projectileID, false);
			}
		}
	}

	void CatAttackEXECUTE::StateCleanUp()
	{
		std::cout << "CatAttackEXECUTE::StateCleanUp()\n";
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
	}

	void CatAttackEXECUTE::StateExit(EntityID id)
	{
		std::cout << "CatAttackEXECUTE::StateExit(" << id << ")\n";
		// resets attack direction selection
		p_data->attackDirection = EnumCatAttackDirection::NONE;
		CatScript::ToggleEntity(p_data->projectileID, false);
	}

	void CatAttackEXECUTE::ProjectileHitRat(const Event<CollisionEvents>& r_CE)
	{
		if (r_CE.GetType() == CollisionEvents::OnCollisionEnter)
		{
			OnCollisionEnterEvent OCEE = dynamic_cast<const OnCollisionEnterEvent&>(r_CE);

			std::pair<EntityID, EntityID> collidedEntities;
			// check which entity is the projectile. the projectile ID is stored in collidedEntities.first
			if (OCEE.Entity1 == p_data->projectileID)
			{
				collidedEntities.first = OCEE.Entity1;
				collidedEntities.second = OCEE.Entity2;
			}
			else if (OCEE.Entity2 == p_data->projectileID)
			{
				collidedEntities.first = OCEE.Entity2;
				collidedEntities.second = OCEE.Entity1;
			}

			if (EntityManager::GetInstance().Has<ScriptComponent>(collidedEntities.second))
			{
				if (CatScript::IsEnemy(collidedEntities.second))
				{
					try
					{
						GETSCRIPTINSTANCEPOINTER(RatScript)->LoseHP(collidedEntities.second, p_data->attackDamage);
					}catch(...){}
					EntityManager::GetInstance().Get<EntityDescriptor>(p_data->projectileID).isActive = false;
					m_bulletCollided = true;
					return;
				}
			}
		}

	}
}