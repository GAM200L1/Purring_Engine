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
				std::cout << "AddPathNode at " << r_nodePosition.x << ", " << r_nodePosition.y 
						<< ", energy: " << p_data->catEnergy << "\n";

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
			  p_data->catEnergy = p_data->catMaxEnergy - p_data->pathPositions.size();

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
			std::cout << "CatMovementEXECUTE::StateEnter(" << id << ")\n";
		  p_data = GETSCRIPTDATA(CatScript, id);
			m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionEnter, CatMovementEXECUTE::OnCollisionEnter, this);
			CatScript::PositionEntity(id, p_data->pathPositions.front());
			m_doneMoving = p_data->pathPositions.size() <= 1; // Don't bother moving if there aren't enough paths
		}

		void CatMovementEXECUTE::StateUpdate(EntityID id, float deltaTime)  
		{ 
			// Check if pause state -------------------------------------------------------------------@TODO KRYSTAL uncomment this
			//if (GameStateManager::GetInstance().GetGameState() == GameStates::PAUSE)
			//{
			//		return;
			//}

			if (!m_doneMoving)
			{
					vec2 currentCatPosition{};
					try
					{
							Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) }; // Get the transform of the player
							currentCatPosition = r_transform.position;
					}
					catch (...) { return; }

					// IF the cat is close enough to the node they're moving towards
					float distanceFromNode{ currentCatPosition.Distance(p_data->pathPositions[p_data->currentPositionIndex]) };
					if (distanceFromNode <= p_data->forgivenessOffset)
					{
							// Deactivate this node
							CatScript::ToggleEntity(p_data->pathQuads[p_data->currentPositionIndex], false);

							if (p_data->currentPositionIndex >= (p_data->pathPositions.size() - 1)) 
							{
									//	This is the last node, so stop the movement of the cat
									StopMoving(id);
							}
							else
							{
									++(p_data->currentPositionIndex);

									// Recalculate distance
									distanceFromNode = currentCatPosition.Distance(p_data->pathPositions[p_data->currentPositionIndex]);
							}
					}

					if (!m_doneMoving)
					{
							// Get the direction of the cat from the point they're moving towards
							vec2 directionToMove{ (p_data->pathPositions[p_data->currentPositionIndex] - currentCatPosition).GetNormalized()};
							 
							// Get the vector to move
							float amtToMove{p_data->movementSpeed * deltaTime};
							directionToMove *= ((distanceFromNode > amtToMove) ? amtToMove : distanceFromNode);

							// Update the position of the cat
							CatScript::PositionEntity(id, currentCatPosition + directionToMove);

							// Ensure the cat is facing the direction of their movement
							vec2 newScale{ CatScript::GetEntityScale(id) };
							newScale.x = std::abs(newScale.x) * ((directionToMove.Dot(vec2{ 1.f, 0.f }) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the cat is facing left
							CatScript::ScaleEntity(id, newScale.x, newScale.y);
					}
			}
			else
			{
					// Wait a second before changing state
					GETSCRIPTINSTANCEPOINTER(CatScript)->TriggerStateChange(id, 1.f);
			}
		}

		void CatMovementEXECUTE::StateExit(EntityID id)  
		{
				std::cout << "CatMovementEXECUTE::StateExit(" << id << ")\n";
				REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
				StopMoving(id);
		}


		void CatMovementEXECUTE::StopMoving(EntityID id)
		{
				m_doneMoving = true;

				// Position the player at the end of the path
				CatScript::PositionEntity(id, p_data->pathPositions.back());
		}


		void CatMovementEXECUTE::OnCollisionEnter(const Event<CollisionEvents>& r_collisionEvent)
		{
				OnCollisionEnterEvent OCEE{ dynamic_cast<const OnCollisionEnterEvent&>(r_collisionEvent)};

				// Check if the rat is colliding with the cat
				if ((EntityManager::GetInstance().Get<EntityDescriptor>(OCEE.Entity1).name.find("Rat") != std::string::npos && OCEE.Entity2 == p_data->catID)
						|| (EntityManager::GetInstance().Get<EntityDescriptor>(OCEE.Entity2).name.find("Rat") != std::string::npos && OCEE.Entity1 == p_data->catID))
				{
						m_collidedWithRat = true;
				}

		}
}