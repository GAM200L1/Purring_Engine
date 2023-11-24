/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CatMovementScript.cpp
 \date     21-11-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains definitions for functions used for a grey cat's movement state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "CatMovementScript.h"
#include "CatAttackScript.h"
#include "Physics/CollisionManager.h"

namespace PE
{
		// ----- Movement Plan Functions ----- //
		void CatMovementPLAN::StateEnter(EntityID id)
		{
			std::cout << "CatMovementPLAN::StateEnter( " << id << " )\n";
			p_data = GETSCRIPTDATA(CatScript, id);
			m_clickEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, CatMovementPLAN::OnMouseClick, this);
			m_releaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, CatMovementPLAN::OnMouseRelease, this);
		
			// reset to max energy
			p_data->catEnergy = p_data->catMaxEnergy; 

			// Clear all the path data
			p_data->pathPositions.clear();

			// Disable all the path nodes
			for (EntityID const& nodeId : p_data->pathQuads)
			{
					CatScript::ToggleEntity(nodeId, false);
			}

			// Teleport the player to the start of the path
			try
			{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };
					p_data->pathPositions.emplace_back(r_transform.position);
			}
			catch (...) 
			{
					p_data->pathPositions.emplace_back(vec2{ 0.f, 0.f });
					return; 
			}
		}

		void CatMovementPLAN::StateUpdate(EntityID id, float deltaTime)
		{
				//std::cout << "CatMovementPLAN::StateUpdate( " << id << " )\n";
			// Check if pause state -------------------------------------------------------------------@TODO KRYSTAL uncomment this
		 // if (GameStateManager::GetInstance().GetGameState() == GameStates::PAUSE) 
		 // {
			//		EndPathDrawing(id);
			//		return;
			//}
			//if (m_mouseClick) 
			//{
			//		std::cout << "Mouse clicked\n";
			//}

			// Check if the mouse has just been clicked
			if (m_mouseClick && !m_mouseClickPrevious && !m_pathBeingDrawn && p_data->catEnergy)
			{
					// Get the position of the cat
					float mouseX{}, mouseY{};
					InputSystem::GetCursorViewportPosition(GameStateManager::GetInstance().p_window, mouseX, mouseY);
					vec2 cursorPosition{ GameStateManager::GetInstance().p_cameraManager->GetWindowToWorldPosition(mouseX, mouseY) };

					// Check if the cat has been clicked
					CircleCollider const& catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
					if (PointCollision(catCollider, cursorPosition))
					{
							std::cout << "Start drawing path\n";
							// Start drawing a path
							m_pathBeingDrawn = true;
					}
			}

			// If the mouse is being pressed
			if (m_mouseClick && m_pathBeingDrawn)
			{
					if(p_data->catEnergy) // Check if the player has sufficient energy
					{
							// Get the mouse position
							float mouseX{}, mouseY{};
							InputSystem::GetCursorViewportPosition(GameStateManager::GetInstance().p_window, mouseX, mouseY);
							vec2 cursorPosition{ GameStateManager::GetInstance().p_cameraManager->GetWindowToWorldPosition(mouseX, mouseY) };

							// Attempt to create a node at the position of the cursor
							// and position the cat where the node is
							CatScript::PositionEntity(id, AttemptToDrawPath(cursorPosition));
					}
					else // Path is being drawn but the player has run out of energy
					{
							// End the path drawing
							EndPathDrawing(id);
					}
			} 
			else if (!m_mouseClick && m_mouseClickPrevious && m_pathBeingDrawn)
			{
					// The mouse has been released, so end the path
					EndPathDrawing(id);
			}
			if (GameStateManager::GetInstance().GetGameState() == GameStates::ATTACK)
			{
				p_data->p_stateManager->ChangeState(new CatAttackPLAN{}, id);
			}
			// Store the current frame's mouse click status
			m_mouseClickPrevious = m_mouseClick;
		}

		void CatMovementPLAN::StateExit(EntityID id)
		{
			std::cout << "CatMovementPLAN::StateExit( " << id << " )\n";
			REMOVE_MOUSE_EVENT_LISTENER(m_clickEventListener);
			REMOVE_MOUSE_EVENT_LISTENER(m_releaseEventListener);

			EndPathDrawing(id);
		}


		vec2 CatMovementPLAN::AttemptToDrawPath(vec2 const& r_position)
		{
				std::cout << "Attempt to draw path\n";

				// Check if there's more than one node in the list
				if (p_data->pathPositions.size() > 1) 
				{
						// Get the distance of the last two nodes
						if (p_data->pathPositions.back().DistanceSquared(p_data->pathPositions[p_data->pathPositions.size() - 2]) < p_data->maxDistance * p_data->maxDistance)
						{
								// IF  the last two nodes are not the max dist from each other, delete the last node
								p_data->pathPositions.erase(std::prev(p_data->pathPositions.end()));
						}
				}

				// Get the distance of the proposed position from the last node in the path
				float distanceOfPosition{ p_data->pathPositions.back().Distance(r_position) };
				if (distanceOfPosition > p_data->maxDistance)
				{
						// Compute the direction of the proposed position from the last node in the path
						vec2 directionOfProposed{ (r_position - p_data->pathPositions.back()).GetNormalized() };

						// Too far, keep adding nodes in between the min and max positions
						for (; distanceOfPosition >= p_data->minDistance; distanceOfPosition -= p_data->maxDistance)
						{
								float distanceToMove{ distanceOfPosition > p_data->maxDistance ? p_data->maxDistance : distanceOfPosition};

								if (!AddPathNode(p_data->pathPositions.back() + directionOfProposed * distanceToMove)) {
										break;
								}
						}
				}
				else if(distanceOfPosition >= p_data->minDistance)
				{
						// Add a node at the position of the mouse
						AddPathNode(r_position);
				} // Otherwise too close, don't add any new positions

				return p_data->pathPositions.back();
		}


		bool CatMovementPLAN::AddPathNode(vec2 const& r_nodePosition)
		{
				std::cout << "AddPathNode at " << r_nodePosition.x << ", " << r_nodePosition.y << "\n";

				// Check if the player has sufficient energy left
				if (p_data->pathPositions.size() == p_data->pathQuads.size()) {
						p_data->catEnergy = 0;
						return false;
				}

				// Change the position of the node
				EntityID nodeId{ p_data->pathQuads[p_data->pathPositions.size()] };
				CatScript::PositionEntity(nodeId, r_nodePosition);
				CatScript::ToggleEntity(nodeId, true);
				
				// Add the position to the path positions list
				p_data->pathPositions.emplace_back(r_nodePosition);

				// Reduce the player's energy
				--p_data->catEnergy;

				return true;
		}


		void CatMovementPLAN::EndPathDrawing(EntityID const id)
		{
				// Stop drawing a path
				m_pathBeingDrawn = false;

				// Position the cat at the end of the path
				CatScript::PositionEntity(id, p_data->pathPositions.back());
		}


		void CatMovementPLAN::OnMouseClick(const Event<MouseEvents>&)
		{
				m_mouseClick = true;
		}


		void CatMovementPLAN::OnMouseRelease(const Event<MouseEvents>&)
		{
				m_mouseClick = false;
		}




		// ----- Movement Execution Functions ----- //
		void CatMovementEXECUTE::StateEnter(EntityID id)  
		{
		  p_data = GETSCRIPTDATA(CatScript, id);
			CatScript::PositionEntity(id, p_data->pathPositions.front());
		}

		void CatMovementEXECUTE::StateUpdate(EntityID id, float deltaTime)  
		{ 
			id; deltaTime;
			// Check if pause state
			if (GameStateManager::GetInstance().GetGameState() == GameStates::PAUSE)
			{
					return;
			}

			// IF  the cat is within a certain threshold from the current node
				// IF  this is the last node, stop the movement of the cat
				// ELSE  change the node to move towards to the next node
						// Deactivate the next node

			// Check where the cat is in relation to the end of the last node
			// Start moving the cat

		}

		void CatMovementEXECUTE::StateExit(EntityID id)  
		{
				// Position the player at the end of the path
				CatScript::PositionEntity(id, p_data->pathPositions.back());
		}
}