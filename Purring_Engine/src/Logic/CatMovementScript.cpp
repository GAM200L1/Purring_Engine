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
			EntityManager::GetInstance().Get<Collider>(id).isTrigger = true;
			m_clickEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, CatMovementPLAN::OnMouseClick, this);
			m_releaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, CatMovementPLAN::OnMouseRelease, this);
			m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnTriggerStay, CatMovementPLAN::OnPathCollision, this);

			ResetDrawnPath();
		}

		void CatMovementPLAN::StateUpdate(EntityID id, float deltaTime)
		{
				//std::cout << "CatMovementPLAN::StateUpdate( " << id << " )\n";
			// Check if pause state -------------------------------------------------------------------@TODO KRYSTAL uncomment this
			if (GameStateManager::GetInstance().GetGameState() == GameStates::PAUSE) 
			{
					EndPathDrawing(id);
					return;
			}

			// If the player has released their mouse and the path is invalid
			if (m_invalidPath && !m_mouseClick && m_mouseClickPrevious)
			{
				ResetDrawnPath();
				m_invalidPath = false;
			}

			// Check if the mouse has just been clicked
			if (m_mouseClick && !m_mouseClickPrevious && !m_pathBeingDrawn && CatScript::GetCurrentEnergyLevel())
			{
					// Get the position of the cat
					vec2 cursorPosition{ CatScript::GetCursorPositionInWorld() };

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
				if(CatScript::GetCurrentEnergyLevel() && !m_invalidPath) // Check if the player has sufficient energy
				{
					// Get the mouse position
					vec2 cursorPosition{ CatScript::GetCursorPositionInWorld() };

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

		void CatMovementPLAN::StateCleanUp()
		{
			REMOVE_MOUSE_EVENT_LISTENER(m_clickEventListener);
			REMOVE_MOUSE_EVENT_LISTENER(m_releaseEventListener);
			REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
		}

		void CatMovementPLAN::StateExit(EntityID id)
		{
			std::cout << "CatMovementPLAN::StateExit( " << id << " )\n";
			EntityManager::GetInstance().Get<Collider>(id).isTrigger = false;
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
						<< ", energy: " << CatScript::GetCurrentEnergyLevel() << "\n";

				// Check if the player has sufficient energy left
				if (p_data->pathPositions.size() == p_data->pathQuads.size()) {
						CatScript::SetCurrentEnergyLevel(0);
						return false;
				}

				// Check if the position is within the bounds of the window
				bool coordinatesInWindow{ true };
				std::optional<std::reference_wrapper<PE::Graphics::Camera>> r_mainCamera{ GameStateManager::GetInstance().p_cameraManager->GetMainCamera() };
				if (r_mainCamera.has_value())
				{
						coordinatesInWindow = r_mainCamera.value().get().GetPositionWithinViewport(r_nodePosition.x, r_nodePosition.y);
				}

				if (!coordinatesInWindow) {
						m_invalidPath = true;
						SetPathColor(1.f, 0.f, 0.f, 1.f);

						return false;
				}

				// Change the position of the node
				EntityID nodeId{ p_data->pathQuads[p_data->pathPositions.size()] };
				CatScript::PositionEntity(nodeId, r_nodePosition);
				CatScript::ToggleEntity(nodeId, true);
				
				// Add the position to the path positions list
				p_data->pathPositions.emplace_back(r_nodePosition);

				// Reduce the player's energy
				CatScript::SetCurrentEnergyLevel(CatScript::GetMaximumEnergyLevel() - static_cast<int>(p_data->pathPositions.size()));

				return true;
		}


		void CatMovementPLAN::EndPathDrawing(EntityID const id)
		{
				// Stop drawing a path
				m_pathBeingDrawn = false;

				// Position the cat at the end of the path
				CatScript::PositionEntity(id, p_data->pathPositions.back());
		}


		void CatMovementPLAN::SetPathColor(float const r, float const g, float const b, float const a)
		{
				for (EntityID& nodeID : p_data->pathQuads)
				{
						EntityManager::GetInstance().Get<Graphics::Renderer>(nodeID).SetColor(r, g, b, a);
				}
		}


		void CatMovementPLAN::ResetDrawnPath()
		{
			// reset to max energy
			CatScript::SetCurrentEnergyLevel(CatScript::GetMaximumEnergyLevel());

			// Clear all the path data
			if (!p_data->pathPositions.empty())
				CatScript::PositionEntity(p_data->catID, p_data->pathPositions.front());

			p_data->pathPositions.clear();

			// Disable all the path nodes
			for (EntityID& nodeId : p_data->pathQuads)
			{
				CatScript::ToggleEntity(nodeId, false);
				EntityManager::GetInstance().Get<Graphics::Renderer>(nodeId).SetColor(); // Reset to white
			}

			// Teleport the player to the start of the path
			try
			{
				Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(p_data->catID) };
				p_data->pathPositions.emplace_back(r_transform.position);
			}
			catch (...)
			{
				p_data->pathPositions.emplace_back(vec2{ 0.f, 0.f });
				return;
			}
		}


		void CatMovementPLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
		{
			if (r_ME.GetType() == MouseEvents::MouseButtonPressed)
			{
				MouseButtonPressedEvent MBPE = dynamic_cast<MouseButtonPressedEvent const&>(r_ME);
				if (MBPE.button == 1 && !p_data->pathPositions.empty())
					ResetDrawnPath();
				else
					m_mouseClick = true;
			}
		}


		void CatMovementPLAN::OnMouseRelease(const Event<MouseEvents>& r_ME)
		{
			if (r_ME.GetType() == MouseEvents::MouseButtonReleased)
				m_mouseClick = false;
		}


		void CatMovementPLAN::OnPathCollision(const Event<CollisionEvents>& r_CE)
		{
			if (r_CE.GetType() == CollisionEvents::OnTriggerStay)
			{
				OnTriggerStayEvent OCEE = dynamic_cast<const OnTriggerStayEvent&>(r_CE);
				// Check if the cat is colliding with an obstacle
				if ((OCEE.Entity1 == p_data->catID && CatScript::IsObstacle(OCEE.Entity2))
						|| (OCEE.Entity2 == p_data->catID && CatScript::IsObstacle(OCEE.Entity1)))
				{
					// The entity is colliding with is an obstacle
					SetPathColor(1.f, 0.f, 0.f, 1.f); // Set the color of the path nodes to red
					m_invalidPath = true;
				}
			}
		}


		// ----- Movement Execution Functions ----- //
		void CatMovementEXECUTE::StateEnter(EntityID id)  
		{
			std::cout << "CatMovementEXECUTE::StateEnter(" << id << ")\n";
		  p_data = GETSCRIPTDATA(CatScript, id);
			m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionEnter, CatMovementEXECUTE::OnCollisionEnter, this);
			CatScript::PositionEntity(id, p_data->pathPositions.front());
			p_data->currentPositionIndex = 0;
			m_doneMoving = p_data->pathPositions.size() <= 1; // Don't bother moving if there aren't enough paths
		}

		void CatMovementEXECUTE::StateUpdate(EntityID id, float deltaTime)  
		{ 
			// Check if pause state -------------------------------------------------------------------@TODO KRYSTAL uncomment this
			if (GameStateManager::GetInstance().GetGameState() == GameStates::PAUSE)
			{
				return;
			}

			if (!m_doneMoving)
			{
					if(p_data->currentPositionIndex >= p_data->pathPositions.size())
					{
							m_doneMoving = true;
					}

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
					GETSCRIPTINSTANCEPOINTER(CatScript)->TriggerStateChange(id, 0.5f);
			}
		}

		void CatMovementEXECUTE::StateCleanUp()
		{
			REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
		}

		void CatMovementEXECUTE::StateExit(EntityID id)  
		{
				std::cout << "CatMovementEXECUTE::StateExit(" << id << ")\n";
				
				StopMoving(id);
				p_data->pathPositions.clear();
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
				if ((CatScript::IsEnemy(OCEE.Entity1) && OCEE.Entity2 == p_data->catID)
						|| (CatScript::IsEnemy(OCEE.Entity2) && OCEE.Entity1 == p_data->catID))
				{
						m_collidedWithRat = true;
				}

		}
}