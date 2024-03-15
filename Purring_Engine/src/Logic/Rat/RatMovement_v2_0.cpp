/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatMovement_v2_0.cpp
 \date     17-01-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     10%
 \par      changes:    Base structure of the class.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     90%
 \par      changes:    06-02-2024
                       Integration of gamestates with the Rat States and Rat AI.

 \brief
    This file contains functions for the rat movement state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatMovement_v2_0.h"
#include "Logic/Rat/RatAttack_v2_0.h"

#include "Events/EventHandler.h"

#include "Math/MathCustom.h"
#include "Math/Transform.h"

//#define DEBUG_PRINT

namespace PE
{
    RatMovement_v2_0::RatMovement_v2_0() : gameStateController{ nullptr }, p_data{ nullptr } {}

    void RatMovement_v2_0::StateEnter(EntityID id)
    {
#ifdef DEBUG_PRINT
        std::cout << "RatMovement_v2_0::StateEnter(" << id << ")" << std::endl;
#endif // DEBUG_PRINT

        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
        m_planningRunOnce = false;

        // Subscribe to the collision trigger events
        m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionEnter, RatMovement_v2_0::OnCollisionEnter, this);
        m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionExit, RatMovement_v2_0::OnCollisionExit, this);
        m_triggerEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatMovement_v2_0::OnTriggerEnterAndStay, this);
        m_triggerStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatMovement_v2_0::OnTriggerEnterAndStay, this);
        m_triggerExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatMovement_v2_0::OnTriggerExit, this);
    }

    void RatMovement_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
#ifdef DEBUG_PRINT
            //std::cout << "RatMovement_v2_0::StateUpdate - Game is paused." << std::endl;
#endif // DEBUG_PRINT
            return;
        }

        static bool triggerWalkAnim{ false };

        // If the game state is execute, keep track of any cats that pass through the rat's radius
        if (gameStateController->currentState == GameStates_v2_0::EXECUTE)
        {
            m_planningRunOnce = false;
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DisableMovementTelegraphs(id);

            if (!triggerWalkAnim) 
            {
                triggerWalkAnim = true;
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::WALK);
            }

            switch (p_data->ratType)
            {
            case EnumRatType::GUTTER:
            case EnumRatType::BRAWLER:
            case EnumRatType::SNIPER:
            {
                // Move towards the target position until we've reached 
                // The function returns true if the target has been reached
                if (GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CalculateMovement(p_data->myID, deltaTime))
                {
                    OnMovementDone();
                }
                break;
            }
            default: break;
            }
        }
        // If the game state is planning, choose the next position to move to
        else if (gameStateController->currentState == GameStates_v2_0::PLANNING)
        {
            switch (p_data->ratType)
            {
            case EnumRatType::GUTTER:
            case EnumRatType::BRAWLER:
            case EnumRatType::SNIPER:
            {
                // Choose where to move when the planning state has just started
                if (!m_planningRunOnce)
                {
                    m_planningRunOnce = true;
                    p_data->finishedExecution = false;

                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::IDLE);

                    // Pick a target position to move to
                    vec2 targetPosition{ PickTargetPosition() };

                    // ---- Update telegraph
                    // Rotate the telegraph to face the target
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->EnableMovementTelegraphs(id, targetPosition);

                    // Clear the collision containers
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ClearCollisionContainers(p_data->myID);
                }

                break; // end of BRAWLER rat type
            }
            default: break;
            }
        }


        // Store game state of frame
        m_previousGameState = gameStateController->currentState;
    }

    void RatMovement_v2_0::StateCleanUp()
    {
        p_data = nullptr;
        gameStateController = nullptr;

        // Unsubscribe events
        REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_triggerEnterEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_triggerStayEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_triggerExitEventListener);
    }

    void RatMovement_v2_0::StateExit(EntityID id)
    {
        // empty
#ifdef DEBUG_PRINT
        //std::cout << "RatMovement_v2_0::StateExit - Rat ID: " << id << " is exiting the movement state." << std::endl;
#endif // DEBUG_PRINT
    }

    void RatMovement_v2_0::OnCollisionEnter(const Event<CollisionEvents>& r_event)
    {
        if (!p_data) { return; }
        else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        if (r_event.GetType() == CollisionEvents::OnCollisionEnter)
        {
            OnCollisionEnterEvent OCEE = dynamic_cast<OnCollisionEnterEvent const&>(r_event);
            // check if rat and cat have collided
            bool ratCollidedWithCat{ GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatTouchingCat(p_data->myID, OCEE.Entity1, OCEE.Entity2) };
            
            // Check if the rat has been colliding with a wall
            if (!ratCollidedWithCat &&
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatTouchingWall(p_data->myID, OCEE.Entity1, OCEE.Entity2) &&
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetExecutionPhaseTimeout(p_data->myID))
            {
                // The rat has been touching the wall for too long
                OnMovementDone();
            }
        }
    }

    void RatMovement_v2_0::OnCollisionExit(const Event<CollisionEvents>& r_event)
    {
        if (!p_data) { return; }
        else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        if (r_event.GetType() == CollisionEvents::OnCollisionExit)
        {
            OnCollisionExitEvent OCEE = dynamic_cast<OnCollisionExitEvent const&>(r_event);
            // check if rat has stopped colliding with wall
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatStopTouchingWall(p_data->myID, OCEE.Entity1, OCEE.Entity2);
        }
    }

    void RatMovement_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
        {
            OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
            // check if a cat has entered the rat's detection collider
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerEntered(p_data->myID, OTEE.Entity1, OTEE.Entity2);
        }
        else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
        {
            OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
            // check if a cat has entered the rat's detection collider
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerEntered(p_data->myID, OTSE.Entity1, OTSE.Entity2);
        }
    }


    void RatMovement_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
        // check if a cat has exited the rat's detection collider
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerExited(p_data->myID, OTEE.Entity1, OTEE.Entity2);
    }


    vec2 RatMovement_v2_0::PickTargetPosition()
    {
        if (p_data)
        {
            if (p_data->p_attackData)
            {
                return p_data->p_attackData->PickTargetPosition();
            }
            else
            {
                return RatScript_v2_0::GetEntityPosition(p_data->myID);
            }
        }        
        else
        {
            return vec2{};
        }
    }


    void RatMovement_v2_0::OnMovementDone() 
    {
        if (!p_data) { return; }

        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(p_data->myID, EnumRatAnimations::IDLE);

        // Switch to the attack state since we're close enough to the targets
        p_data->ratPlayerDistance = 0.f;
        p_data->targetPosition = RatScript_v2_0::GetEntityPosition(p_data->myID);
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToAttack(p_data->myID);
    }

}
