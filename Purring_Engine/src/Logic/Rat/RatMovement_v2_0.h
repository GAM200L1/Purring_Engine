/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatMovement_v2_0.h
 \date     17-01-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     50%
 \par      changes:    Majority rat movement AI logic.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     20%
 \par      changes:    06-02-2024
                       Integration of gamestates with the Rat States and Rat AI.

 \brief
    This file contains declarations for functions used for rats movement state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"

namespace PE
{
    // ----- RAT MOVEMENT STATE v2.0 ----- //
    class RatMovement_v2_0 : public State
    {
        // ----- Constructors ----- //
    public:
        RatMovement_v2_0();

        virtual ~RatMovement_v2_0() { p_data = nullptr; };

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
            \brief Cleans up any state-specific resources or subscriptions
        *************************************************************************************/
        virtual void StateCleanUp();

        /*!***********************************************************************************
            \brief Finalizes the state before transitioning out

            \param[in,out] id - ID of the rat instance
        *************************************************************************************/
        virtual void StateExit(EntityID id) override;

        /*!***********************************************************************************
            \brief Handles the event where the rat collides with a cat

            \param[in,out] r_TE - Event data related to the collision
        *************************************************************************************/
        void RatHitCat(const Event<CollisionEvents>& r_TE);

        /*!***********************************************************************************
            \brief Called when a trigger enter or stay event has occurred

            \param[in,out] r_TE - Trigger event data
        *************************************************************************************/
        void OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE);

        /*!***********************************************************************************
         \brief Called when a trigger exit event has occured. If an event has occurred
          between this script's rat's detection collider and a cat, the parent rat
          is notified.

         \param[in,out] r_TE - Trigger event data.
        *************************************************************************************/
        void OnTriggerExit(const Event<CollisionEvents>& r_TE);

        /*!***********************************************************************************
            \brief Returns the name of this state

            \return A string view representing the name of the state, useful for debugging and logging
        *************************************************************************************/
        virtual std::string_view GetName() { return "Movement_v2_0"; }

    private:

        /*!***********************************************************************************
            \brief Pointer to the game state controller, used to query and manage the overall game state
        *************************************************************************************/
        GameStateController_v2_0* gameStateController{ nullptr };
        GameStates_v2_0 m_previousGameState{GameStates_v2_0::PLANNING}; // The game state in the previous frame

        /*!***********************************************************************************
            \brief Pointer to data specific to the RatScript, containing information such as current position, health, etc.
        *************************************************************************************/
        RatScript_v2_0_Data* p_data{ nullptr };

        // ID of the event listener for collision events, used to register and unregister the rat for collision notifications
        int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};

        bool m_planningRunOnce{}; // True if the planning phase has been run once


        // ----- PRIVATE METHODS ----- //
    private:
        /*!***********************************************************************************
          \brief Returns true if the current game state is different from the game state
                  in the previous frame, false otherwise.
        *************************************************************************************/
        inline bool StateJustChanged() const
        {
            return m_previousGameState != gameStateController->currentState;
        }

        /*!***********************************************************************************
          \brief Sets the animation to be played based on whether the rat is moving or not.
        *************************************************************************************/
        void UpdateAnimation(bool const isRatMoving);

        /*!***********************************************************************************
          \brief Pick the position that the rat should move toward (in a straight line).

          \return Returns a next viable target for the rat.
        *************************************************************************************/
        vec2 PickTargetPosition();
    };
}
