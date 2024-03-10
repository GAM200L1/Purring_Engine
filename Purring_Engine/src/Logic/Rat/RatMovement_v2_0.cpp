/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatMovement_v2_0.cpp
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
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);
        m_planningRunOnce = false;

        // Subscribe to the collision trigger events for the 
        m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatMovement_v2_0::OnTriggerEnterAndStay, this);
        m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatMovement_v2_0::OnTriggerEnterAndStay, this);
        m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatMovement_v2_0::OnTriggerExit, this);
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
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DisableTelegraphs(id);

            if (!triggerWalkAnim) 
            {
                triggerWalkAnim = true;
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::WALK);
            }

            switch (p_data->ratType)
            {
            case EnumRatType::GUTTER:
            case EnumRatType::BRAWLER:
            {
                // Move towards the target position until we've reached 
                // The function returns true if the target has been reached
                if (GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CalculateMovement(p_data->myID, deltaTime))
                {
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::IDLE);

                    // Switch to the attack state since we're close enough to the targets
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToAttack(id);
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
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->EnableTelegraphs(id, targetPosition);

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
        p_data->ratPlayerDistance = 0.f;
        p_data = nullptr;
        gameStateController = nullptr;

        REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
    }

    void RatMovement_v2_0::StateExit(EntityID id)
    {
        // empty
#ifdef DEBUG_PRINT
        //std::cout << "RatMovement_v2_0::StateExit - Rat ID: " << id << " is exiting the movement state." << std::endl;
#endif // DEBUG_PRINT
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
            if (p_data->attackData)
            {
                return p_data->attackData->PickTargetPosition();
            }
            else
            {
                std::cout << "RatMovement_v2_0::PickTargetPosition() couldn't pick a target position\n";
                return RatScript_v2_0::GetEntityPosition(p_data->myID);
            }
        }        
        else
        {
            std::cout << "RatMovement_v2_0::PickTargetPosition() couldn't pick a target position\n";
            return vec2{};
        }
    }

}
