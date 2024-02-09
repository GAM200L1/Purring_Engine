/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatIdle_v2_0.h
 \date     24-01-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief
    This file contains declarations for functions used for the rat idle state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"

namespace PE
{
    enum class RatType
    {
        IDLE,
        PATROL
    };

    // ----- RAT IDLE STATE v2.0 ----- //
    class RatIdle_v2_0 : public State
    {
    public:
        // ----- Constructor ----- //
        RatIdle_v2_0(RatType type);

        // ----- Destructor ----- //
        virtual ~RatIdle_v2_0() { p_data = nullptr; }

        /*!***********************************************************************************
            \brief Set up the state and subscribe to the collision events

            \param[in,out] id - ID of instance of script
        *************************************************************************************/
        virtual void StateEnter(EntityID id) override;

        /*!***********************************************************************************
            \brief Checks if its state should change

            \param[in,out] id - ID of instance of script
            \param[in,out] deltaTime - delta time to update the state with
        *************************************************************************************/
        virtual void StateUpdate(EntityID id, float deltaTime) override;

        /*!***********************************************************************************
            \brief does nothing
        *************************************************************************************/
        virtual void StateExit(EntityID id) override;

        /*!***********************************************************************************
         \brief Unsubscribes from the collision events
        *************************************************************************************/
        virtual void StateCleanUp();

        // Logic for Rat idle state

        /*!***********************************************************************************
         \brief Logic for rat patrol points and patrolling.

         \param id - ID of instance of script
         \param deltaTime - time taken to move towards patrol point
        *************************************************************************************/
        void PatrolLogic(EntityID id, float deltaTime);

        /*!***********************************************************************************
         \brief Moves the rat towards the target point.

         \param id - ID of instance of script
         \param target - target point to move towards
         \param deltaTime - time taken to move towards target
        *************************************************************************************/
        void MoveTowards(EntityID id, const vec2& target, float deltaTime);

        /*!***********************************************************************************
         \brief Checks if rat has reached its destination.

         \param id - ID of instance of script
         \param target - target point to move towards
        *************************************************************************************/
        bool HasReachedDestination(EntityID id, const vec2& target);

        /*!***********************************************************************************
         \brief Initalizes points to patrol to.
        *************************************************************************************/
        void InitializePatrolPoints();

        /*!***********************************************************************************
         \brief Sets points to patrol to.

         \param points - Array of points
        *************************************************************************************/
        void SetPatrolPoints(const std::vector<PE::vec2>& points);

        // Getter for the state name with version
        virtual std::string_view GetName() { return "Idle_v2_0"; }


        // --- COLLISION DETECTION --- // 

        /*!***********************************************************************************
         \brief Checks if any cats entered or executed the rat's detection radius during 
                the last execution phase and decides whether to swap to the attacking or 
                hunting states respectively.
        *************************************************************************************/
        void CheckIfShouldChangeStates();

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
        RatType m_type;

        GameStateController_v2_0* gameStateController{ nullptr };
        GameStates_v2_0 m_previousGameState; // The game state in the previous frame
        bool m_planningRunOnce{ false }; // Set to true after target position has been set in the pause state, set to false one frame after the pause state has started.

        // Event listener IDs 
        int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};

        // ----- PRIVATE METHODS ---- //
    private:
        /*!***********************************************************************************
          \brief Returns true if the current game state is different from the game state
                  in the previous frame, false otherwise.
        *************************************************************************************/
        inline bool StateJustChanged() const
        {
            return m_previousGameState != gameStateController->currentState;
        }
    };


} // namespace PE
