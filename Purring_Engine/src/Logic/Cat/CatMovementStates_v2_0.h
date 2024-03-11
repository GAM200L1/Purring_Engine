/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatMovementStates_v2_0.h
 \date     21-11-2023

 \author:              Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 \par      code %:     80%

 \co-author:		   LIEW Yeni
 \par	   email:	   yeni.l@digipen.edu
 \par      code %:     20%

 \brief
	This file contains declarations for functions used for a grey cat's movement states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "CatScript_v2_0.h"

namespace PE
{
	// ----- CAT MOVEMENT PLAN STATE ----- //
	class CatMovement_v2_0PLAN
	{
	public:
		// ----- Destructor ----- //
		/*!**********************************************************************************
		 \brief Destructor for CatMovement_v2_0PLAN
		*************************************************************************************/
		~CatMovement_v2_0PLAN() 
		{ 
			p_data = nullptr;
		}

		// ----- Public Functions ----- //
		/*!***********************************************************************************
		 \brief Subscribes to input and collision events and resets the variables of the state.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		void Enter(EntityID id);

		/*!***********************************************************************************
		 \brief Attempts to draw paths following the player's cursor position.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		 \param[in] deltaTime - Time in seconds since the last frame.
		*************************************************************************************/
		void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Unsubscribe from all the event listeners.
		*************************************************************************************/
		void CleanUp();

		/*!***********************************************************************************
		 \brief Forces the end of drawing and moves the player to the end of their path.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		void Exit(EntityID id);

		/*!***********************************************************************************
		 \brief Sets variable path being drawn

		 \param[in] toggle - true if path being drawn, false if otherwise
		*************************************************************************************/
		inline void SetPathBeingDrawn(bool toggle) { m_pathBeingDrawn = toggle; }

		/*!***********************************************************************************
		 \brief Gets variable path being drawn

		 \param[out] bool - true if path being drawn, false if otherwise
		*************************************************************************************/
		inline bool GetPathBeingDrawn() { return m_pathBeingDrawn; }

		/*!***********************************************************************************
		 \brief Checks if the position is far away enough from the previous node to add a new
						node. If the proposed position is too far away, points are added in a straight
						line until the proposed position is reached or the cat runs out of energy.

		 \param[in] r_position - Proposed position to create the node at.
		 \return vec2 - Position that the node was created at. If no node was created, the
						position of the last node in the container is returned.
		*************************************************************************************/
		vec2 AttemptToDrawPath(vec2 const& r_position);


		/*!***********************************************************************************
		 \brief Adds a node to the container of path nodes and positions one of the path quads
						where the path node should be.

		 \param[in] r_nodePosition - Position to create the path node at.
		*************************************************************************************/
		bool AddPathNode(vec2 const& r_nodePosition);


		/*!***********************************************************************************
		 \brief Sets the status of path drawing to false and move the cat to the last node
						in the path node list.

		 \param[in] id - ID of the cat entity.
		*************************************************************************************/
		void EndPathDrawing(EntityID const id);


		/*!***********************************************************************************
		 \brief Sets the color of all the nodes in the path.

		 \param[in] r_color - new color to set the path
		*************************************************************************************/
		void SetPathColor(vec4 const& r_color = {1.f, 1.f, 1.f, 1.f});


		// ----- Events ----- // 

		/*!***********************************************************************************
		 \brief Callback function for the mouse click event.

		 \param[in] r_mouseEvent - Mouse event data.
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_mouseEvent);


		/*!***********************************************************************************
		 \brief Callback function for the mouse release event.

		 \param[in] r_mouseEvent - Mouse event data.
		*************************************************************************************/
		void OnMouseRelease(const Event<MouseEvents>& r_mouseEvent);


		/*!***********************************************************************************
		 \brief Callback function for collision events.

		 \param[in] r_mouseEvent - Collision event data.
		*************************************************************************************/
		void OnPathCollision(const Event<CollisionEvents>& r_TE);

		/*!**********************************************************************************
		 \brief Resets the position of the player to the beginning of their drawn path,
						clears the drawn path, and resets the energy level of the player.
		*************************************************************************************/
		void ResetDrawnPath();

		/*!**********************************************************************************
		 \brief Checks if the path drawn is invalid
		*************************************************************************************/
		bool CheckInvalid();


	private:
		// ----- Private Variables ----- //
		CatScript_v2_0Data* p_data = nullptr;
		vec4 m_invalidPathColor{ 1.f, 0.f, 0.f, 1.f };
		vec4 m_defaultPathColor{ 0.506f, 0.490f, 0.490f, 1.f };

		int m_clickEventListener{}, m_releaseEventListener{}, m_collisionEventListener{}; // Stores the handler for the mouse click and release events
		bool m_pathBeingDrawn{ false }; // Set to true when the player path is being drawn
		bool m_mouseClick{ false }; // Set to true when the mouse is pressed, false otherwise
		bool m_mouseClickPrevious{ false }; // Set to true if the mouse was pressed in the previous frame, false otherwise
		bool m_invalidPath{ false };
	};

	// ----- CAT MOVEMENT EXECUTE STATE ----- //
	class CatMovement_v2_0EXECUTE : public State
	{
	public:
		
		// ----- Destructor ----- //
		/*!**********************************************************************************
		 \brief Destructor for CatMovement_v2_0EXECUTE
		*************************************************************************************/
		virtual ~CatMovement_v2_0EXECUTE() override 
		{ 
			p_data = nullptr;
		}

		// ----- Public Functions ----- //

		/*!***********************************************************************************
		 \brief Subscribes to the collision events and resets the variables of the state.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;


		/*!***********************************************************************************
		 \brief Called every frame. Moves the player along the path they have drawn.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		 \param[in] deltaTime - Time since the last frame.
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;


		/*!***********************************************************************************
		 \brief Unsubscribe from all the event listeners.
		*************************************************************************************/
		virtual void StateCleanUp();


		/*!***********************************************************************************
		 \brief Moves the player to the end of their path.

		 \param[in] id - EntityID of the entity this instance of the script is attached to.
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;


		/*!***********************************************************************************
		 \brief Stop moving towards the end of the path.
		*************************************************************************************/
		void StopMoving(EntityID id);

		// ----- Public Getters ----- //
		/*!**********************************************************************************
		 \brief Returns name of this state
		*************************************************************************************/
		virtual std::string_view GetName() override { return "MovementEXECUTE"; }


		// ----- Events ----- // 

		/*!***********************************************************************************
		 \brief Callback function for the collision enter event. Checks if the player has
						collided with the rat.

		 \param[in] r_collisionEvent - Trigger event data.
		*************************************************************************************/
		void OnTriggerEnter(const Event<CollisionEvents>& r_collisionEvent);

	private:
		// ----- Private Variables ----- //
		CatScript_v2_0Data* p_data;
		EntityID m_mainCatID;
		int m_triggerEventListener{}; // Stores the handler for the mouse click and release events
		bool m_doneMoving{ false }; // Set to true when the cat has reached the end of their path
	
		float footstepDelay{ 0.45f };
		float footstepTimer{ 0 };
	};
}