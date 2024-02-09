/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatAttack_v2_0.h
 \date     24-01-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief
    This file contains declarations for functions used for the rat attack state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"
#include "Events/EventHandler.h"

namespace PE
{
    // ----- RAT ATTACK STATE v2.0 ----- //
    class RatAttack_v2_0 : public State
    {
    public:
        // ----- Constructor ----- //
        RatAttack_v2_0();

        // ----- Destructor ----- //
        virtual ~RatAttack_v2_0() { p_data = nullptr; };

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

        /*!***********************************************************************************
         \brief Checks if rat has collided with any cat entity.

         \param r_TE - Collision event.
        *************************************************************************************/
        void RatHitCat(const Event<CollisionEvents>& r_TE);

        // --- COLLISION DETECTION --- //

        /*!***********************************************************************************
         \brief Called when a trigger enter event has occured. If an event has
            occurred between this script's rat's detection collider and a cat, the rat cannot continue attacking.

         \param[in,out] r_TE - Trigger event data.
        *************************************************************************************/
        void OnTriggerEnterForAttack(const Event<CollisionEvents>& r_TE);

        /*!***********************************************************************************
        \brief Called when a trigger stay event has occured. If an event has
           occurred between this script's rat's detection collider and a cat, the rat cannot continue attacking.

        \param[in,out] r_TE - Trigger event data.
       *************************************************************************************/
        void OnTriggerStayForAttack(const Event<CollisionEvents>& r_TE);

        /*!***********************************************************************************
         \brief Plays attack audio whenever the rat attacks.
        *************************************************************************************/
        void PlayAttackAudio();

        // Getter for the state name with version
        virtual std::string_view GetName() { return "Attack_v2_0"; }

    private:
        GameStateController_v2_0* gameStateController{ nullptr }; //pointer to game state controller

        // Attack state specific variables and data
        RatScript_v2_0_Data* p_data;
        int m_attackEventListener{};
        int m_attackStayEventListener{};
        float m_delay{};

        bool attacksoundonce{};
        bool telegraphEnabled{ false };
    };

} // namespace PE
