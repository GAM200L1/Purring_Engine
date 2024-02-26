/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatMovementStates_v2_0.cpp
 \date     21-11-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \co-author:		   LIEW Yeni
 \par	   email:	   yeni.l@digipen.edu

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
#include "Events/CollisionEvent.h"
#include "Events/MouseEvent.h"
#include "Physics/CollisionManager.h"
#include "Logic/FollowScript.h"
#include "Graphics/CameraManager.h"
#include "Animation/Animation.h"
#include "Logic/Script.h"


namespace PE
{
	// ----- Movement Plan Functions ----- //
	void CatMovement_v2_0PLAN::Enter(EntityID id)
	{
		p_data = GETSCRIPTDATA(CatScript_v2_0, id);

		// Subscribe to events
		m_clickEventListener = ADD_MOUSE_EVENT_LISTENER(MouseEvents::MouseButtonPressed, CatMovement_v2_0PLAN::OnMouseClick, this);
		m_releaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, CatMovement_v2_0PLAN::OnMouseRelease, this);
		m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(PE::CollisionEvents::OnTriggerStay, CatMovement_v2_0PLAN::OnPathCollision, this);

		if (CatHelperFunctions::IsFirstLevel())
		{
			FollowScriptData_v2_0* follow_data = GETSCRIPTDATA(FollowScript_v2_0, p_data->catID);
			p_data->followCatPositions = follow_data->NextPosition;
		}
		
		ResetDrawnPath();
	}

	void CatMovement_v2_0PLAN::Update(EntityID id, float deltaTime)
	{
		GameStateController_v2_0* gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		
		// Check if pause state
		if (gsc->currentState == GameStates_v2_0::PAUSE)
		{
			EndPathDrawing(id);
			return;
		}

		// If the player has released their mouse and the path is invalid
		if (m_invalidPath && !m_mouseClick && m_mouseClickPrevious)
		{
			ResetDrawnPath();
			m_invalidPath = false;
			return;
		}

		// Check if the mouse has just been clicked
		if (m_mouseClick && !m_pathBeingDrawn && p_data->catCurrentEnergy)
		{
			// Get the position of the cat
			vec2 cursorPosition{ CatHelperFunctions::GetCursorPositionInWorld() };

			// Check if the cat has been clicked
			CircleCollider const& catCollider = std::get<CircleCollider>(EntityManager::GetInstance().Get<Collider>(id).colliderVariant);
			if (PointCollision(catCollider, cursorPosition))
			{
				// Start drawing a path
				m_pathBeingDrawn = true;
			}
		}
		
		// If the mouse is being pressed
		if (m_mouseClick && m_pathBeingDrawn)
		{
			if (p_data->catCurrentEnergy && !m_invalidPath) // Check if the player has sufficient energy
			{
				// Get the mouse position
				vec2 cursorPosition{ CatHelperFunctions::GetCursorPositionInWorld() };

				// Attempt to create a node at the position of the cursor
				// and position the cat where the node is
				CatHelperFunctions::PositionEntity(id, AttemptToDrawPath(cursorPosition));
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

	void CatMovement_v2_0PLAN::CleanUp()
	{
		// Return if this cat is not the main cat
		//if (!p_data->isMainCat) { return; }
		REMOVE_MOUSE_EVENT_LISTENER(m_clickEventListener);
		REMOVE_MOUSE_EVENT_LISTENER(m_releaseEventListener);
		REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
	}

	void CatMovement_v2_0PLAN::Exit(EntityID id)
	{
		EndPathDrawing(id);
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
			p_data->pathPositions.emplace_back(r_position);

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

				if (!AddPathNode(p_data->pathPositions.back() + directionOfProposed * distanceToMove)) {
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
		if (p_data->pathPositions.size() == p_data->pathQuads.size()) {
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
			SetPathColor(1.f, 0.f, 0.f, 1.f);
			return false;
		}

		// Change the position of the node
		EntityID nodeId{ p_data->pathQuads[p_data->pathPositions.size()] };
		CatHelperFunctions::PositionEntity(nodeId, r_nodePosition);
		CatHelperFunctions::ToggleEntity(nodeId, true);
		
		// Plays path placing audio
		SerializationManager m_serializationManager;
		EntityID sound = m_serializationManager.LoadFromFile("AudioObject/Movement Planning SFX_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(sound))
			EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(sound);

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
	}


	void CatMovement_v2_0PLAN::SetPathColor(float const r, float const g, float const b, float const a)
	{
		// Set the color of all the nodes
		for (EntityID& nodeID : p_data->pathQuads)
		{
			EntityManager::GetInstance().Get<Graphics::Renderer>(nodeID).SetColor(r, g, b, a);
		}
	}

	void CatMovement_v2_0PLAN::ResetDrawnPath()
	{
		FollowScriptData_v2_0* follow_data = GETSCRIPTDATA(FollowScript_v2_0, p_data->catID);

		for (auto s : follow_data->followers)
		{
			auto dat = GETSCRIPTDATA(CatScript_v2_0, s);

			if ((dat->catType != EnumCatType::MAINCAT && GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->GetCurrentLevel() == 0))
			{
				CatHelperFunctions::PositionEntity(dat->catID, dat->resetPosition);
			} // if cat is following cat in the chain )
		}

		CatHelperFunctions::PositionEntity(p_data->catID, p_data->resetPosition);
	
		// reset to max energy
		p_data->catCurrentEnergy = p_data->catMaxMovementEnergy;
		m_mouseClickPrevious = false;
		

		// Clear all the path data
		if (!p_data->pathPositions.empty())
		{
			CatHelperFunctions::PositionEntity(p_data->catID, p_data->pathPositions.front());
			follow_data->CurrentPosition = p_data->pathPositions.front();
		}
		p_data->pathPositions.clear();



		//set it to current so that it doesnt update followers
		//i only update followers if current position does not match the transform position
		/*follow_data->NextPosition = p_data->followCatPositions;
		for (int i = 1; i < follow_data->NumberOfFollower; i++)
		{
			EntityManager::GetInstance().Get<Transform>(follow_data->FollowingObject[i]).position = p_data->followCatPositions[i];
		}*/

		// Disable all the path nodes
		for (EntityID& nodeId : p_data->pathQuads)
		{
			CatHelperFunctions::ToggleEntity(nodeId, false);
			EntityManager::GetInstance().Get<Graphics::Renderer>(nodeId).SetColor(); // Reset to white
		}

		// Add the player's starting position as a node
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

	bool CatMovement_v2_0PLAN::CheckInvalid()
	{
		return m_invalidPath;
	}


	void CatMovement_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		if (r_ME.GetType() == MouseEvents::MouseButtonPressed)
		{
			// Reset the path on pressing right click
			MouseButtonPressedEvent MBPE = dynamic_cast<MouseButtonPressedEvent const&>(r_ME);
			if (MBPE.button != 1)
				m_mouseClick = true; // Flag that the mouse has been clicked
		

		}
	}


	void CatMovement_v2_0PLAN::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		if (r_ME.GetType() == MouseEvents::MouseButtonReleased)
			m_mouseClick = false;
	}


	void CatMovement_v2_0PLAN::OnPathCollision(const Event<CollisionEvents>& r_CE)
	{
		if (r_CE.GetType() == CollisionEvents::OnTriggerStay)
		{
			OnTriggerStayEvent OCEE = dynamic_cast<const OnTriggerStayEvent&>(r_CE);
			// Check if the cat is colliding with an obstacle
			if ((OCEE.Entity1 == p_data->catID && CatHelperFunctions::IsObstacle(OCEE.Entity2))
				|| (OCEE.Entity2 == p_data->catID && CatHelperFunctions::IsObstacle(OCEE.Entity1)))
			{
				// The entity is colliding with is an obstacle
				SetPathColor(1.f, 0.f, 0.f, 1.f); // Set the color of the path nodes to red
				m_invalidPath = true;

				SerializationManager m_serializationManager;
				EntityID sound = m_serializationManager.LoadFromFile("AudioObject/Path Denial SFX1_Prefab.json");
				if (EntityManager::GetInstance().Has<AudioComponent>(sound))
					EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
				EntityManager::GetInstance().RemoveEntity(sound);
			}
		}
	}


	// ----- Movement Execution Functions ----- //
	void CatMovement_v2_0EXECUTE::StateEnter(EntityID id)
	{
		p_data = GETSCRIPTDATA(CatScript_v2_0, id);
		m_triggerEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, CatMovement_v2_0EXECUTE::OnTriggerEnter, this);

		if (p_data->catType != EnumCatType::MAINCAT && CatHelperFunctions::IsFirstLevel())
		{
			m_doneMoving = true;
			return;
		}
		else
		{
			m_mainCatID = id;
		}

		CatHelperFunctions::PositionEntity(id, p_data->pathPositions.front());
		p_data->currentPositionIndex = 0;
		m_doneMoving = p_data->pathPositions.size() <= 1; // Don't bother moving if there aren't enough paths
	}

	void CatMovement_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{
		/*if (p_data->catType != EnumCatType::MAINCAT && CatHelperFunctions::IsFirstLevel())
		{ return; }*/// if cat is following cat in the chain )
		GameStateController_v2_0* p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

		// Check if pause state 
		if (p_gsc->currentState == GameStates_v2_0::PAUSE) { return; }

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

				if(footstepTimer <= 0)
				{
					PlayFootStep();
				}

				// Ensure the cat is facing the direction of their movement
				vec2 newScale{ CatHelperFunctions::GetEntityScale(id) };
				newScale.x = std::abs(newScale.x) * ((directionToMove.Dot(vec2{ 1.f, 0.f }) >= 0.f) ? 1.f : -1.f); // Set the scale to negative if the cat is facing left
				CatHelperFunctions::ScaleEntity(id, newScale.x, newScale.y);
			}
		}
		else
		{
			// for cat chain level
			if (CatHelperFunctions::IsFirstLevel() && id == m_mainCatID)
			{
				auto const& catVector = GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->GetCurrentCats(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->mainInstance);
				// triggers state change for cats in the chain to sync animations
				for (auto [catID, catType] : catVector)
				{
					GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCatCaged(catID);
					GETSCRIPTINSTANCEPOINTER(CatScript_v2_0)->TriggerStateChange(catID, 0.5f);
				}
				GETSCRIPTINSTANCEPOINTER(CatScript_v2_0)->TriggerStateChange(id, 0.5f);
			}
			// for other levels where cats move independently
			else if (!CatHelperFunctions::IsFirstLevel())
			{
				// Wait a second before changing state
				GETSCRIPTINSTANCEPOINTER(CatScript_v2_0)->TriggerStateChange(id, 0.5f);
			}
		}
	}

	void CatMovement_v2_0EXECUTE::StateCleanUp()
	{
		REMOVE_KEY_COLLISION_LISTENER(m_triggerEventListener);
	}

	void CatMovement_v2_0EXECUTE::StateExit(EntityID id)
	{
		StopMoving(id);
		p_data->pathPositions.clear();
		EntityManager::GetInstance().Get<Collider>(p_data->catID).isTrigger = false;
	}

	void CatMovement_v2_0EXECUTE::StopMoving(EntityID id)
	{
		m_doneMoving = true;

		// Position the player at the end of the path
		if (p_data->pathPositions.size())
			CatHelperFunctions::PositionEntity(id, p_data->pathPositions.back());
	}

	void CatMovement_v2_0EXECUTE::PlayFootStep()
	{
		int randNum = (std::rand() % 3) + 1;
		SerializationManager m_serializationManager;

		switch (randNum)
		{
		case 1:
		{
			EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Cat Movement SFX 1_Prefab.json");
			if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
				EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
			EntityManager::GetInstance().RemoveEntity(buttonpress);
			break;
		}
		case 2:
		{
			EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Cat Movement SFX 2_Prefab.json");
			if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
				EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
			EntityManager::GetInstance().RemoveEntity(buttonpress);
			break;
		}
		case 3:
		{
			EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Cat Movement SFX 3_Prefab.json");
			if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
				EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
			EntityManager::GetInstance().RemoveEntity(buttonpress);
			break;
		}
		}

		footstepTimer = footstepDelay;
	}


	void CatMovement_v2_0EXECUTE::OnTriggerEnter(const Event<CollisionEvents>& r_TriggerEvent)
	{
		GameStateController_v2_0* p_gsc = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
		OnTriggerEnterEvent OCEE{ dynamic_cast<const OnTriggerEnterEvent&>(r_TriggerEvent) };

		auto CheckExitPoint = [&](EntityID id) { return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Exit Point") != std::string::npos) ? true : false; };
		if ((CheckExitPoint(OCEE.Entity1) && OCEE.Entity2 == p_data->catID && (p_data->catType == EnumCatType::MAINCAT))
			|| (CheckExitPoint(OCEE.Entity2) && OCEE.Entity1 == p_data->catID && (p_data->catType == EnumCatType::MAINCAT)))
		{
			GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->NextStage((p_gsc->GetCurrentLevel()+1 == 3)? 0 : (p_gsc->GetCurrentLevel() + 1)); // goes to the next stage
		}
	}
}