/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatAttack_v2_0.h
 \date     17-01-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     50%
 \par      changes:    Majority rat attack AI logic.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     50%
 \par      changes:    06-02-2024
                       Integration of gamestates with the Rat States and Rat AI.

 \brief
    This file contains declarations for functions used for rats attack state.

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
        // ----- Constructors ----- //
    public:
        virtual ~RatAttack_v2_0() { p_data = nullptr; };

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
            \brief Handles the event when a rat successfully hits a cat during an attack.

            \param[in,out] r_TE - Event data for the collision.
        *************************************************************************************/
        void RatHitCat(const Event<CollisionEvents>& r_TE);


        // --- COLLISION DETECTION --- // 

        /*!***********************************************************************************
         \brief Called when a trigger enter or stay event has occured. If an event has
          occurred between this script's rat's detection collider or attack collider and a cat, 
          the parent rat is notified.

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

        /*!***********************************************************************************
            \brief Returns the name of the state, useful for debugging and logging.

            \return A string view representing the name of the state.
        *************************************************************************************/
        virtual std::string_view GetName() { return "Attack_v2_0"; }

    private:
        GameStateController_v2_0* gameStateController{ nullptr }; // pointer to the game state controller

        // Attack state specific variables and data
        RatScript_v2_0_Data* p_data;
        float m_delay{};
        float m_attackDuration{};

        bool attackFeedbackOnce{};
        bool telegraphEnabled{ false };

        // Event listener IDs 
        int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};

        // ----- PRIVATE METHODS ----- //
    private:
        /*!***********************************************************************************
         \brief Checks if any cats entered or executed the rat's detection radius during
                the last execution phase and decides whether to swap to the attacking or
                hunting states respectively.
        *************************************************************************************/
        void ChangeStates();
    };

} // namespace PE
