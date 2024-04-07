/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatMovementStates_v2_0.cpp
 \date     21-11-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 \par      code %:     80%
 \par	   changed:	   13-3-2024

 \co-author:		   LIEW Yeni
 \par	   email:	   yeni.l@digipen.edu
 \par      code %:     20%
 \par	   changed:	   13-3-2024

 \brief
	This file contains declarations for functions used for a grey cat's movement states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"

#include "CatMovementStates_v2_0.h"
#include "Logic/GameStateController_v2_0.h"
#include "CatController_v2_0.h"
#include "CatHelperFunctions.h"
#include "FollowScript_v2_0.h"

#include "ECS/Entity.h"
#include "Hierarchy/HierarchyManager.h"
#include "Events/CollisionEvent.h"
#include "Events/MouseEvent.h"
#include "Physics/CollisionManager.h"
#include "Logic/FollowScript.h"
#include "Graphics/CameraManager.h"
#include "Animation/Animation.h"
#include "Logic/Script.h"
#include "AudioManager/GlobalMusicManager.h"


namespace PE
{
	// ----- Movement Plan Functions ----- //
	void CatMovement_v2_0PLAN::Enter(EntityID id)
	{
		// retrieve cat data
		p_data = GETSCRIPTDATA(CatScript_v2_0, id);
		
		int currLvl = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetCurrentLevel();
		if (currLvl == 0 || currLvl == 1)
			m_defaultPathColor = vec4{ 1.f, 1.f, 1.f, 1.f };
		else
			m_defaultPathColor = vec4{ 0.506f, 0.490f, 0.490f, 1.f };
		
		p_data->pathPositions.clear();
		
		// subscribe to collision events
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnTriggerEnter, CatMovement_v2_0PLAN::OnPathCollision, this);

		// reset energy, toggle off all path nodes, reset path node colors and add a path position
		p_data->catCurrentEnergy = p_data->catMaxMovementEnergy;
		for (EntityID nodeID : p_data->pathQuads)
		{
			CatHelperFunctions::ToggleEntity(nodeID, false);
		}
		SetPathColor(m_defaultPathColor);
		p_data->pathPositions.emplace_back(CatHelperFunctions::GetEntityPosition(p_data->catID));
		m_heartPlayed = false;
	}

	void CatMovement_v2_0PLAN::Update(EntityID id, float deltaTime, vec2 const& r_cursorPosition, vec2 const& r_prevCursorPosition, bool mouseClicked, bool mouseClickedPrevious)
	{
		// If the player has released their mouse and the path is invalid
		if (m_invalidPath && !mouseClicked && mouseClickedPrevious)
		{
			ResetDrawnPath();
			m_invalidPath = false;
			return;
		}
		
		// Check if the mouse has just been clicked
		if (mouseClicked && !m_pathBeingDrawn && p_data->catCurrentEnergy)
		{
			// Check if the cat has been clicked
			CircleCollider const& catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
			if (PointCollision(catCollider, r_cursorPosition) && r_cursorPosition != r_prevCursorPosition)
			{
				if (p_data->pathPositions.empty())
					m_resetPositions.push(std::make_pair(-1, CatHelperFunctions::GetEntityPosition(id)));
				else
					m_resetPositions.push(std::make_pair(static_cast<int>(p_data->pathPositions.size() - 1), p_data->pathPositions.back()));
				
				// save the positions of the follow cats
				if (p_data->catType == EnumCatType::MAINCAT)
				{
					GETSCRIPTINSTANCEPOINTER(FollowScript_v2_0)->SavePositions(id);
				}
				// Start drawing a path
				m_pathBeingDrawn = true;
			}
		}
		
		// If the mouse is being pressed
		if (mouseClicked && m_pathBeingDrawn)
		{
			if (p_data->catCurrentEnergy) // Check if the player has sufficient energy
			{
				// Attempt to create a node at the position of the cursor
				// and position the cat where the node is
				CatHelperFunctions::PositionEntity(id, AttemptToDrawPath(CatHelperFunctions::GetCursorPositionInWorld()));
			}
			else // Path is being drawn but the player has run out of energy
			{
				// End the path drawing
				EndPathDrawing(id);
			}
		}
		else if (!mouseClicked && mouseClickedPrevious && m_pathBeingDrawn)
		{
			// The mouse has been released, so end the path
			p_data->startDoubleClickTimer = false;
			p_data->doubleClickTimer = 0.f;
			EndPathDrawing(id);
		}
	}

	void CatMovement_v2_0PLAN::CleanUp()
	{
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
	}

	void CatMovement_v2_0PLAN::Exit(EntityID id)
	{
		EndPathDrawing(id);
		
		if (p_data->catType == EnumCatType::MAINCAT && !(GETSCRIPTDATA(FollowScript_v2_0, id))->followers.empty())
		{
			// set follow cats to their previous positions
			GETSCRIPTINSTANCEPOINTER(FollowScript_v2_0)->ResetToSavePositions(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID());
		}

		p_data = nullptr;
	}


	vec2 CatMovement_v2_0PLAN::AttemptToDrawPath(vec2 const& r_position)
	{
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
		
		if (p_data->pathPositions.empty())
		{
			p_data->pathPositions.emplace_back(r_position);
		}

		// Get the distance of the proposed position from the last node in the path
		float distanceOfPosition = p_data->pathPositions.back().Distance(r_position);
		
		if (distanceOfPosition > p_data->maxDistance)
		{
			// Compute the direction of the proposed position from the last node in the path
			vec2 directionOfProposed{ (r_position - p_data->pathPositions.back()).GetNormalized() };

			// Too far, keep adding nodes in between the min and max positions
			for (; distanceOfPosition >= p_data->minDistance; distanceOfPosition -= p_data->maxDistance)
			{
				float distanceToMove{ distanceOfPosition > p_data->maxDistance ? p_data->maxDistance : distanceOfPosition };

				if (!AddPathNode(p_data->pathPositions.back() + directionOfProposed * distanceToMove)) 
				{
					CatScript_v2_0::PlayPathPlacementAudio();
					break;
				}
			}
		}
		else if (distanceOfPosition >= p_data->minDistance)
		{
			// Add a node at the position of the mouse
			AddPathNode(r_position);
		} // Otherwise too close, don't add any new positions

		return p_data->pathPositions.back();
	}


	bool CatMovement_v2_0PLAN::AddPathNode(vec2 const& r_nodePosition)
	{
		// Check if the player has sufficient energy left
		if (p_data->pathPositions.size() == p_data->pathQuads.size()) 
		{
			p_data->catCurrentEnergy = 0;
			return false;
		}

		// Check if the position is within the bounds of the window
		bool coordinatesInWindow{ true };
		std::optional<std::reference_wrapper<PE::Graphics::Camera>> r_mainCamera{ GETCAMERAMANAGER()->GetMainCamera() };
		if (r_mainCamera.has_value())
		{
			coordinatesInWindow = r_mainCamera.value().get().GetPositionWithinViewport(r_nodePosition.x, r_nodePosition.y);
		}

		// Invalidate the path if the position is out of bounds of the window
		if (!coordinatesInWindow) {
			m_invalidPath = true;
			SetPathColor(m_invalidPathColor);
			return false;
		}

		// Change the position of the node
		EntityID nodeId{ p_data->pathQuads[p_data->pathPositions.size()] };
		CatHelperFunctions::PositionEntity(nodeId, r_nodePosition);
		if (!CatHelperFunctions::IsActive(nodeId)) 
		{
				CatScript_v2_0::PlayPathPlacementAudio();
				CatHelperFunctions::ToggleEntity(nodeId, true);
		}

		// Add the position to the path positions list
		p_data->pathPositions.emplace_back(r_nodePosition);

		// Reduce the player's energy
		p_data->catCurrentEnergy = (p_data->catMaxMovementEnergy - static_cast<int>(p_data->pathPositions.size()));

		return true;
	}


	void CatMovement_v2_0PLAN::EndPathDrawing(EntityID const id)
	{
		// Stop drawing a path
		m_pathBeingDrawn = false;

		// Position the cat at the end of the path
		if (!p_data->pathPositions.empty())
			CatHelperFunctions::PositionEntity(id, p_data->pathPositions.back());

		// push into undo stack
		GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->AddToUndoStack(id, EnumUndoType::UNDO_MOVEMENT);
	}


	void CatMovement_v2_0PLAN::SetPathColor(vec4 const& r_color)
	{
		// Set the color of all the nodes
		for (EntityID& nodeID : p_data->pathQuads)
		{
			EntityManager::GetInstance().Get<Graphics::Renderer>(nodeID).SetColor(r_color.x, r_color.y, r_color.z, r_color.w);
		}
	}

	void CatMovement_v2_0PLAN::ResetDrawnPath()
	{
		if (m_resetPositions.empty()) { return; }
		
		EntityID const& r_mainID = GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID();

		if (p_data->catType == EnumCatType::MAINCAT)
		{
			// reset follower cats positions
			GETSCRIPTINSTANCEPOINTER(FollowScript_v2_0)->ResetToSavePositions(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID());
		}

		std::pair<int, vec2> const& r_resetPosition = m_resetPositions.top();

		// reset this cat's position
		CatHelperFunctions::PositionEntity(p_data->catID, r_resetPosition.second);
	
		// reset to previous energy
		if (r_resetPosition.first != -1)
		{
			p_data->catCurrentEnergy = p_data->catMaxMovementEnergy - r_resetPosition.first;
		}
		else
		{
			p_data->catCurrentEnergy = p_data->catMaxMovementEnergy;
		}
		
		if (r_resetPosition.first != -1)
		{
			std::vector<vec2> temp;
			for (int i{ 0 }; i < r_resetPosition.first; ++i)
				temp.emplace_back(p_data->pathPositions[i]);
			p_data->pathPositions.clear();
			p_data->pathPositions = temp;
		}

		// Disable all the path nodes
		for (int i{ static_cast<int>(p_data->pathQuads.size() - 1) }; i > r_resetPosition.first; --i)
		{
			// erases path node which entityid is in the path colliders on cage
			if (r_mainID == p_data->catID && !m_pathCollidersOnCage.empty())
				m_pathCollidersOnCage.erase(std::find(m_pathCollidersOnCage.begin(), m_pathCollidersOnCage.end(), i));
			
			// deactivates path node
			CatHelperFunctions::ToggleEntity(p_data->pathQuads[i], false);
		}
		
		// reset the last node's position to where the cat is
		CatHelperFunctions::PositionEntity(p_data->pathQuads[r_resetPosition.first], r_resetPosition.second);

		SetPathColor(m_defaultPathColor);

		m_pathBeingDrawn = false;

		m_resetPositions.pop();
		// Add the player's starting position as a node
		p_data->pathPositions.emplace_back(CatHelperFunctions::GetEntityPosition(p_data->catID));

		if (r_mainID == p_data->catID && m_pathHasCagedCat && m_pathCollidersOnCage.empty())
		{
			m_pathHasCagedCat = false;
			
			// stop heart animation
			EntityManager::GetInstance().Get<AnimationComponent>(m_heartIcon).StopAnimation();
			CatHelperFunctions::ToggleEntity(m_heartIcon, false);
			m_heartPlayed = false;

			// play sad cat sounds
			switch (*GETSCRIPTDATA(CatScript_v2_0, m_cagedCatID).catType)
			{
				case EnumCatType::ORANGECAT:
				{
					PE::GlobalMusicManager::GetInstance().PlaySFX("AudioObject/Cat Orange Stop Rescue SFX.prefab", false);
					break;
				}
				default:
				{
					PE::GlobalMusicManager::GetInstance().PlaySFX("AudioObject/Cat Grey Stop Rescue SFX.prefab", false);
				}
			}
		}
	}

	bool CatMovement_v2_0PLAN::CheckInvalid()
	{
		return m_invalidPath;
	}

	void CatMovement_v2_0PLAN::OnPathCollision(const Event<CollisionEvents>& r_CE)
	{
		if (r_CE.GetType() == CollisionEvents::OnTriggerEnter)
		{
			auto InvalidPath =
				[&]()
				{
					// The entity is colliding with is an obstacle
					SetPathColor(m_invalidPathColor); // Set the color of the path nodes to red
					m_invalidPath = true;

					// Play audio
					std::string soundPrefabPath = "AudioObject/Path Denial SFX1.prefab";
					PE::GlobalMusicManager::GetInstance().PlaySFX(soundPrefabPath, false);
				};

			auto IsCatAndCaged =
				[&](EntityID catCage)
				{
					if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(catCage) && GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(catCage))
						return true;
					else
						return false;
				};
			auto PlayHeart = 
				[&](EntityID cagedCatID)
				{
					if (!m_heartPlayed)
						m_heartPlayed = true;
					else
						return;

					for (EntityID findHeartID : Hierarchy::GetInstance().GetChildren(cagedCatID))
					{
						if (EntityManager::GetInstance().Get<EntityDescriptor>(findHeartID).name.find("Heart") != std::string::npos)
						{
							m_heartIcon = findHeartID;
							CatHelperFunctions::ToggleEntity(findHeartID, true);
							EntityManager::GetInstance().Get<AnimationComponent>(findHeartID).PlayAnimation();
							// play adopt audio
							CatScript_v2_0::PlayRescueCatAudio(*GETSCRIPTDATA(CatScript_v2_0, cagedCatID).catType);
						}
					}
					// @TODO Play audio for gonna rescue cat
					//std::string soundPrefabPath = "AudioObject/Path Denial SFX1.prefab";
					//PE::GlobalMusicManager::GetInstance().PlaySFX(soundPrefabPath, false);
				};

			OnTriggerEnterEvent OTEE = dynamic_cast<const OnTriggerEnterEvent&>(r_CE);
			// Check if the cat is colliding with an obstacle
			if (CatHelperFunctions::IsObstacle(OTEE.Entity2))
			{
				if (OTEE.Entity1 == p_data->catID)
				{
					InvalidPath();
				}
				else if (std::find(p_data->pathQuads.begin(), p_data->pathQuads.end(), OTEE.Entity1) != p_data->pathQuads.end())
				{
					InvalidPath();
				}
			}
			else if (CatHelperFunctions::IsObstacle(OTEE.Entity1))
			{
				if (OTEE.Entity2 == p_data->catID)
				{
					InvalidPath();
				}
				else if (std::find(p_data->pathQuads.begin(), p_data->pathQuads.end(), OTEE.Entity2) != p_data->pathQuads.end())
				{
					InvalidPath();
				}
			}
			else if (IsCatAndCaged(OTEE.Entity2))
			{
				if (OTEE.Entity1 == GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID())
				{
					// play animation and sound for the heart animation
					PlayHeart(OTEE.Entity2);
					m_pathHasCagedCat = true;
					m_cagedCatID = OTEE.Entity2;
				}
				else if (p_data->catID == GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID() && std::find(p_data->pathQuads.begin(), p_data->pathQuads.end(), OTEE.Entity1) != p_data->pathQuads.end())
				{
					m_pathCollidersOnCage.emplace_back(OTEE.Entity1);
				}
			}
			else if (IsCatAndCaged(OTEE.Entity1))
			{
				if (OTEE.Entity2 == GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID())
				{
					// play animation and sound for the heart animation
					PlayHeart(OTEE.Entity1);
					m_pathHasCagedCat = true;
					m_cagedCatID = OTEE.Entity1;
				}
				else if (p_data->catID == GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetMainCatID() && std::find(p_data->pathQuads.begin(), p_data->pathQuads.end(), OTEE.Entity2) != p_data->pathQuads.end())
				{
					m_pathCollidersOnCage.emplace_back(OTEE.Entity2);
				}
			}
		}
	}

	void CatMovement_v2_0PLAN::StopHeartAnimation(EntityID)
	{
		if (EntityManager::GetInstance().Has<AnimationComponent>(m_heartIcon) &&
			EntityManager::GetInstance().Get<AnimationComponent>(m_heartIcon).HasAnimationEnded())
		{
			EntityManager::GetInstance().Get<AnimationComponent>(m_heartIcon).StopAnimation();
			CatHelperFunctions::ToggleEntity(m_heartIcon, false);
		}
	}

	// ----- Movement Execution Functions ----- //
	void CatMovement_v2_0EXECUTE::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(CatScript_v2_0, id);
		m_triggerEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, CatMovement_v2_0EXECUTE::OnTriggerEnter, this);
		m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionStay, CatMovement_v2_0EXECUTE::OnCollisionStay, this);
		m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionExit, CatMovement_v2_0EXECUTE::OnCollisionStay, this);

		if (p_data->catType == EnumCatType::MAINCAT)
			m_mainCatID = id;
		
		if (!GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsFollowCat(id))
			CatHelperFunctions::PositionEntity(id, p_data->pathPositions.front());
		
		p_data->currentPositionIndex = 0;
		m_doneMoving = p_data->pathPositions.size() <= 1; // Don't bother moving if there aren't enough paths

		m_movementTimer = 0.5f;
		m_startMovementTimer = false;

		CatHelperFunctions::ToggleEntity(p_data->catWalkParticles, true); // set to active, it will only show during execute phase
		
		if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->godMode)
			EntityManager::GetInstance().Get<Collider>(id).isTrigger = true;
		else
			EntityManager::GetInstance().Get<Collider>(id).isTrigger = false;
	}

	void CatMovement_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{
		GameStateController_v2_0* p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		// Check if pause state 
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }
		if (GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsFollowCat(id)) { return; } // if cat is following cat in the chain

		// Check if the player is still moving
		if (!m_doneMoving)
		{
			if (p_data->currentPositionIndex >= p_data->pathPositions.size())
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
				CatHelperFunctions::ToggleEntity(p_data->pathQuads[p_data->currentPositionIndex], false);

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
				vec2 directionToMove{ (p_data->pathPositions[p_data->currentPositionIndex] - currentCatPosition).GetNormalized() };

				// Get the vector to move
				float amtToMove{ p_data->movementSpeed * deltaTime };
				directionToMove *= ((distanceFromNode > amtToMove) ? amtToMove : distanceFromNode);

				// Update the position of the cat
				CatHelperFunctions::PositionEntity(id, currentCatPosition + directionToMove);

				footstepTimer -= deltaTime;

				if (footstepTimer <= 0)
				{
					CatScript_v2_0::PlayFootstepAudio();
					footstepTimer = footstepDelay;
				}

				// Ensure the cat is facing the direction of their movement
				vec2 newScale{ CatHelperFunctions::GetEntityScale(id) };
				newScale.x = std::abs(newScale.x) * ((directionToMove.Dot(vec2{ 1.f, 0.f }) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the cat is facing left
				CatHelperFunctions::ScaleEntity(id, newScale.x, newScale.y);

				// ensure the cat does not stay stuck in the wall
				if (m_startMovementTimer)
				{
					m_movementTimer -= deltaTime;
					if (m_movementTimer <= 0.f)
					{
						// extra in case prevent going through does not work well
						CatHelperFunctions::PositionEntity(id, p_data->pathPositions[p_data->currentPositionIndex]);
						// Deactivate this node
						CatHelperFunctions::ToggleEntity(p_data->pathQuads[p_data->currentPositionIndex], false);

						if (p_data->currentPositionIndex >= (p_data->pathPositions.size() - 1))
						{
							//	This is the last node, so stop the movement of the cat
							StopMoving(id);
						}

						++(p_data->currentPositionIndex);
						//EntityManager::GetInstance().Get<RigidBody>(id).velocity = directionToMove * 2.f;
						m_startMovementTimer = false;
						m_movementTimer = 0.5f;
					}
				}
				else
				{
					m_movementTimer = 0.5f;
				}
			}
		}
		else
		{
			// Wait a second before changing state
			GETSCRIPTINSTANCEPOINTER(CatScript_v2_0)->TriggerStateChange(id, 0.5f);
		}
	}

	void CatMovement_v2_0EXECUTE::StateCleanUp()
	{
		REMOVE_KEY_COLLISION_LISTENER(m_triggerEventListener);
		REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
		REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
	}

	void CatMovement_v2_0EXECUTE::StateExit(EntityID id)
	{
		StopMoving(id);

		p_data = nullptr;
	}

	void CatMovement_v2_0EXECUTE::StopMoving(EntityID id)
	{
		m_doneMoving = true;

		// Position the player at the end of the path
		if (p_data->pathPositions.size())
			CatHelperFunctions::PositionEntity(id, p_data->pathPositions.back());
	}


	void CatMovement_v2_0EXECUTE::OnTriggerEnter(const Event<CollisionEvents>& r_TriggerEvent)
	{
		GameStateController_v2_0* p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		if (r_TriggerEvent.GetType() == CollisionEvents::OnTriggerEnter)
		{
			OnTriggerEnterEvent OTEE{ dynamic_cast<const OnTriggerEnterEvent&>(r_TriggerEvent) };

			auto CheckExitPoint = [&](EntityID id) { return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Exit Point") != std::string::npos) ? true : false; };
			if ((CheckExitPoint(OTEE.Entity1) && OTEE.Entity2 == p_data->catID && (p_data->catType == EnumCatType::MAINCAT))
				|| (CheckExitPoint(OTEE.Entity2) && OTEE.Entity1 == p_data->catID && (p_data->catType == EnumCatType::MAINCAT)))
			{
				CatController_v2_0* p_catManager = GETSCRIPTINSTANCEPOINTER(CatController_v2_0);
				p_catManager->UpdateCurrentCats(p_catManager->mainInstance);
				GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->NextStage(p_gsc->GetCurrentLevel() + 1); // goes to the next stage
			}
		}
	}

	void CatMovement_v2_0EXECUTE::OnCollisionStay(const Event<CollisionEvents>& r_collisionEvent)
	{
		if (r_collisionEvent.GetType() == CollisionEvents::OnCollisionStay)
		{
			// starts 
			OnCollisionStayEvent OCSE{ dynamic_cast<const OnCollisionStayEvent&>(r_collisionEvent) };
			if ((OCSE.Entity1 == p_data->catID && CatHelperFunctions::IsObstacle(OCSE.Entity2))
				|| (OCSE.Entity2 == p_data->catID && CatHelperFunctions::IsObstacle(OCSE.Entity1)))
			{
				m_startMovementTimer = true;
			}
			else
			{
				m_startMovementTimer = false;
			}
		}
		else
		{
			m_startMovementTimer = false;
		}
	}
}