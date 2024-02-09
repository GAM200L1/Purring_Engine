/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatMovement_v2_0.h
 \date     17-01-2024

 \author               ONG You Yang
 \par      email:      youyang.o@digipen.edu

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
            \brief Called when a trigger exit event has occurred

            \param[in,out] r_TE - Trigger event data
        *************************************************************************************/
        void OnTriggerExit(const Event<CollisionEvents>& r_TE);

        /*!***********************************************************************************
            \brief Returns the name of this state

            \return A string view representing the name of the state, useful for debugging and logging
        *************************************************************************************/
        virtual std::string_view GetName() { return "Movement_v2_0"; }

    private:
        /*!***********************************************************************************
            \brief Calculates the next movement step for the rat based on its current position, target position, and other factors

            \param[in] id - The entity ID of the rat
            \param[in] deltaTime - Time elapsed since the last update call, used for frame rate independent movement
            \return A boolean indicating whether a new position has been successfully calculated
        *************************************************************************************/
        bool CalculateMovement(EntityID id, float deltaTime);

        /*!***********************************************************************************
            \brief Checks if the rat has reached its destination

            \param[in] newPosition - The new position of the rat after a movement calculation
            \param[in] targetPosition - The target position the rat is moving towards
            \return A boolean indicating whether the rat has reached its target position
        *************************************************************************************/
        bool CheckDestinationReached(const vec2& newPosition, const vec2& targetPosition);

        /*!***********************************************************************************
            \brief Pointer to the game state controller, used to query and manage the overall game state
        *************************************************************************************/
        GameStateController_v2_0* gameStateController{ nullptr };

        /*!***********************************************************************************
            \brief Pointer to data specific to the RatScript, containing information such as current position, health, etc.
        *************************************************************************************/
        RatScript_v2_0_Data* p_data{ nullptr };

        /*!***********************************************************************************
            \brief The minimum distance considered to be close enough to the target position for the movement to be considered complete
        *************************************************************************************/
        float minDistanceToTarget{ 1.0f };

        /*!***********************************************************************************
            \brief ID of the event listener for collision events, used to register and unregister the rat for collision notifications
        *************************************************************************************/
        int m_collisionEventListener{};

        /*!***********************************************************************************
            \brief ID of the event listener for collision stay events, used to continuously check for collisions while the rat remains in contact with another entity
        *************************************************************************************/
        int m_collisionStayEventListener{};

        /*!***********************************************************************************
            \brief ID of the event listener for collision exit events, used to determine when the rat has exited a collision with another entity
        *************************************************************************************/
        int m_collisionExitEventListener{};
    };
}
