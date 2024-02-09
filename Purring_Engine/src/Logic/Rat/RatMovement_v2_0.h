/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatMovement_v2_0.h
 \date     24-01-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief
    This file contains declarations for functions used for the rat movement state.

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
    public:
        // ----- Constructor ----- //
        RatMovement_v2_0();
        // ----- Destructor ----- //
        virtual ~RatMovement_v2_0() { p_data = nullptr; };

        // Override State class functions

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
         \brief Unsubscribes from the collision events
        *************************************************************************************/
        virtual void StateCleanUp();

        /*!***********************************************************************************
            \brief does nothing
        *************************************************************************************/
        virtual void StateExit(EntityID id) override;

        virtual std::string_view GetName() { return "Movement_v2_0"; }

        /*!***********************************************************************************
         \brief Checks if rat has collided with any cat entity.

         \param r_TE - Collision event.
        *************************************************************************************/
        void RatHitCat(const Event<CollisionEvents>& r_TE);

        // --- COLLISION DETECTION --- //

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

        /*!***********************************************************************************
        \brief Returns true if the rat's current position is different from its old position before moving.
        
        \param id - ID of the entity
        \param deltaTime - delta time to update movement with
        *************************************************************************************/
        bool CalculateMovement(EntityID id, float deltaTime);

        /*!***********************************************************************************
        \brief Returns true if the rat has reached its destination.

        \param newPosition - new position of the rat
        \param targetPosition - target position, the rat's destination
        *************************************************************************************/
        bool CheckDestinationReached(const vec2& newPosition, const vec2& targetPosition);

        GameStateController_v2_0* gameStateController{ nullptr };

        // Movement state specific variables and data
        RatScript_v2_0_Data* p_data{ nullptr };

        float minDistanceToTarget{ 1.0f };

        int m_collisionEventListener{};
        int m_collisionStayEventListener{};
    };
}
