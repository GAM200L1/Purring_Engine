/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatIdle_v2_0.h
 \date     17-01-2024
  
 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     99%
 \par      changes:    All function declarations.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     1%
 \par      changes:    06-02-2024
                       Addition of collision and trigger event listeners.

 \brief
    This file contains declarations for functions used for rats idle state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"

namespace PE
{
    // ----- RAT IDLE STATE v2.0 ----- //
    class RatIdle_v2_0 : public State
    {
        // ----- Constructors ----- //
    public:
        /*!***********************************************************************************
            \brief Constructor for RatIdle_v2_0

            \param[in,out] willPatrol - if cat will patrol
        *************************************************************************************/
        RatIdle_v2_0(bool willPatrol);

        /*!***********************************************************************************
            \brief Destructor for RatIdle_v2_0
        *************************************************************************************/
        virtual ~RatIdle_v2_0() { p_data = nullptr; }
        
        // ----- Public Functions ----- //
    public:
        /*!***********************************************************************************
            \brief Set up the state and subscribe to the collision events

            \param[in,out] id - The entity ID of the rat
        *************************************************************************************/
        virtual void StateEnter(EntityID id) override;

        /*!***********************************************************************************
            \brief Checks if its state should change

            \param[in,out] id - ID of instance of script
            \param[in,out] deltaTime - delta time to update the state with
        *************************************************************************************/
        virtual void StateUpdate(EntityID id, float deltaTime) override;

        /*!***********************************************************************************
         \brief Unsubscribes from the collision events
        *************************************************************************************/
        virtual void StateCleanUp();

        /*!***********************************************************************************
            \brief Finalizes the state before transitioning out

            \param[in,out] id - ID of the rat instance
        *************************************************************************************/
        virtual void StateExit(EntityID id) override;

        /*!***********************************************************************************
             \brief Logic for patrolling behavior, defines how the rat moves between patrol points.

             \param id - Entity ID of the rat.
             \param deltaTime - Time elapsed since the last frame, used for movement calculation.
        *************************************************************************************/
        void PatrolLogic(EntityID id, float deltaTime);

        /*!***********************************************************************************
             \brief Update the target the rat is moving toward while patrolling.
        *************************************************************************************/
        void UpdatePatrolTarget();

        /*!***********************************************************************************
             \brief Initializes default patrol points for the rat.
        *************************************************************************************/
        void InitializePatrolPoints();

        /*!***********************************************************************************
             \brief Sets custom patrol points for the rat.

             \param points - A vector of positions defining the patrol route.
        *************************************************************************************/
        void SetPatrolPoints(const std::vector<PE::vec2>& points);

        /*!***********************************************************************************
         \brief Returns the name of the state.

         \return A string view representing the state's name.
        *************************************************************************************/
        virtual std::string_view GetName() { return GETSCRIPTNAME(RatIdle_v2_0); }


        // --- COLLISION DETECTION --- // 

        /*!***********************************************************************************
         \brief Checks if any cats entered or executed the rat's detection radius during 
                the last execution phase and decides whether to swap to the attacking or 
                hunting states respectively.
        *************************************************************************************/
        void CheckIfShouldChangeStates();

        /*!***********************************************************************************
         \brief Called when a collision enter or stay event has occurred. If an event has
          occurred between this script's rat's collider and a cat or an obstacle, 
          the parent rat is notified.

         \param[in] r_event - Event data.
        *************************************************************************************/
        void OnCollisionEnterOrStay(const Event<CollisionEvents>& r_event);

        /*!***********************************************************************************
         \brief Called when a collision exit event has occurred. If an event has
          occurred between this script's rat's collider and an obstacle, the parent rat
          is notified.

         \param[in] r_event - Event data.
        *************************************************************************************/
        void OnCollisionExit(const Event<CollisionEvents>& r_event);

        /*!***********************************************************************************
         \brief Called when a trigger enter or stay event has occured. If an event has
          occurred between this script's rat's detection collider and a cat, the parent rat
          is notified.

         \param[in,out] r_TE - Trigger event data.
        *************************************************************************************/
        void OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE);

        /*!***********************************************************************************
         \brief Called when a trigger exit event has occured. If an event has occurred
          between this script's rat's detection collider and a cat, the parent rat
          is notified.

         \param[in,out] r_TE - Trigger event data.
        *************************************************************************************/
        void OnTriggerExit(const Event<CollisionEvents>& r_TE);

    private:
        // Idle Planning specific variables
        RatScript_v2_0_Data* p_data;
        bool m_willPatrol{ false };

        // Patrolling data
        int m_patrolIndex{ 0 }; // Index of the current patrol point being moved toward
        // Set to true to cycle in the positive order of the patrol points (i.e. 0 -> 1 -> 2), 
        // false to move in the opposite order (i.e. 2 -> 1 -> 0)
        bool m_returnToFirstPoint{ false };

        GameStateController_v2_0* m_gameStateController{ nullptr };
        bool m_planningRunOnce{ false }; // Set to true after target position has been set in the pause state, set to false one frame after the pause state has started.

        // Event listener IDs 
        int m_collisionEnterEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};
        int m_triggerEnterEventListener{}, m_triggerStayEventListener{}, m_triggerExitEventListener{};
    };


} // namespace PE
