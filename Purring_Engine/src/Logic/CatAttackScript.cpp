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

namespace PE
{
	// ----- CAT ATTACK PLAN STATE ----- //

	void CatAttackPLAN::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(CatScript, id);
		ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, CatAttackPLAN::OnMouseClick, this);
	}
	
	void CatAttackPLAN::StateUpdate(EntityID id, float deltaTime)
	{
		// get the mouse cursor position
		float mouseX{}, mouseY{};
		InputSystem::GetCursorViewportPosition(GameStateManager::GetInstance().p_window, mouseX, mouseY); // I'll change this to take floats in next time...
		vec2 cursorPosition{ GameStateManager::GetInstance().p_cameraManager->GetWindowToWorldPosition(mouseX, mouseY) };

		// if in attack planning phase, allow player to select a cat and plan that cats attacks
		if (GameStateManager::GetInstance().GetGameState() == GameStates::ATTACK)
		{
			// resets selected attack and shows the boxes as green
			ShowAttackSelection(id, cursorPosition);

			if (m_showBoxes)
			{
				for (auto const& telegraph : p_data->telegraphIDs)
				{
					AABBCollider const& telegraphCollider = std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(telegraph.second).colliderVariant);
					// checks if the mouse hovers over any of the select boxes. if it does, the boxes should become red
					if (PointCollision(telegraphCollider, cursorPosition))
					{
						// sets hover color if telegraph is color
						EntityManager::GetInstance().Get<Graphics::Renderer>(telegraph.second).SetColor(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, 1.f);
						if (m_mouseClick)
						{
							// if player selects either of the boxes, the attack direction is determined
							p_data->attackDirection = telegraph.first;
							// the other boxes should not show
							m_showBoxes = false;
							for (auto const& [direction, telegraphID] : p_data->telegraphIDs)
							{
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
			}
			m_mouseClick = false;
		}
		else if (GameStateManager::GetInstance().GetGameState() == GameStates::EXECUTE)
		{
			p_data->p_stateManager->ChangeState(new CatAttackEXECUTE{}, id);
		}
	}
	
	void CatAttackPLAN::StateExit(EntityID id)
	{
		for (auto const& telegraph : p_data->telegraphIDs)
		{
			// set the entity with p_attack direction to not active, the green box should disappear
			EntityManager::GetInstance().Get<EntityDescriptor>(telegraph.second).isActive = false;
			EntityManager::GetInstance().Get<Graphics::Renderer>(telegraph.second).SetColor(m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, 1.f);
		}
	}

	void CatAttackPLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
		m_mouseClick = true;
	}

	void CatAttackPLAN::ShowAttackSelection(EntityID id, vec2 const& r_cursorPosition)
	{
		CircleCollider const& catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
		if (PointCollision(catCollider, r_cursorPosition) && m_mouseClick)
		{
			// if player selects cat with EntityID 'id', the cat will reset its attack choice and show its selectable attack boxes and become active
			m_showBoxes = true;
			p_data->attackDirection = 0;
			for (auto const& boxID : p_data->telegraphIDs)
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(boxID.second).isActive = true;
			}
		}
		else if (m_mouseClick && !m_showBoxes && p_data->attackDirection == 0)
		{
			m_showBoxes = false;
		}
	}



	// ----- CAT ATTACK EXECUTION ----- //
	void CatAttackEXECUTE::StateEnter(EntityID id) 
	{
		p_data = GETSCRIPTDATA(CatScript, id);
		EntityManager::GetInstance().Get<EntityDescriptor>(p_data->projectileID).isActive = true;
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
		EntityManager::GetInstance().Get<Transform>(p_data->projectileID).position = EntityManager::GetInstance().Get<Transform>(id).position + (direction * 0.5f * EntityManager::GetInstance().Get<Transform>(id).width);
		EntityManager::GetInstance().Get<RigidBody>(p_data->projectileID).ApplyLinearImpulse(direction * p_data->bulletForce * p_data->bulletRange);
	}

	void CatAttackEXECUTE::StateUpdate(EntityID id, float deltaTime)
	{
		if (GameStateManager::GetInstance().GetGameState() == GameStates::EXECUTE)
		{
			if (m_attackDuration > 0.f)
			{
				m_attackDuration -= deltaTime;
			}
			else
			{
				p_data->p_stateManager->ChangeState(new CatAttackPLAN{}, id);
				GameStateManager::GetInstance().SetPauseState();
			}
		}
	}

	void CatAttackEXECUTE::StateExit(EntityID id)
	{
		// resets attack direction selection
		p_data->attackDirection = 0;
		EntityManager::GetInstance().Get<EntityDescriptor>(p_data->projectileID).isActive = false;
		//EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
	}

}