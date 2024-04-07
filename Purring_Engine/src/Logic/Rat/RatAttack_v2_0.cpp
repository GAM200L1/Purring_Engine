/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatAttack_v2_0.cpp
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
    This file contains functions for the rat attack state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatAttack_v2_0.h"
#include "Events/EventHandler.h"
#include "Logic/CatScript.h"
#include "Logic/FollowScript.h"
#include "../Cat/CatController_v2_0.h"

//#define DEBUG_PRINT

namespace PE
{
    void RatAttack_v2_0::StateEnter(EntityID id)
    {
#ifdef DEBUG_PRINT
        std::cout << "RatAttack_v2_0::StateEnter(" << id << ")" << std::endl;
#endif // DEBUG_PRINT

        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);                                                   // Get GSM instance

        // Subscribe to events
        m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionEnter, RatAttack_v2_0::OnCollisionEnter, this);
        m_triggerEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatAttack_v2_0::OnTriggerEnterAndStay, this);
        m_triggerStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatAttack_v2_0::OnTriggerEnterAndStay, this);
        m_triggerExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatAttack_v2_0::OnTriggerExit, this);

        // Reset variables
        m_delay = 0.1f;
        p_data->attacking = true;  // Set the attacking flag to true
        m_attackFeedbackOnce = false;
        m_attackDuration = 0; // will be set later when the attack animation plays
        p_data->attackedCats.clear();

        if (p_data->p_attackData) { p_data->p_attackData->InitAttack(); }
    }

    void RatAttack_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;
        }

        if (gameStateController->currentState == GameStates_v2_0::EXECUTE) 
        { 
            if (m_delay > 0.f && p_data->attacking)
            {
                m_delay -= deltaTime;
#ifdef DEBUG_PRINT
                //std::cout << "[DEBUG] RatAttack_v2_0::StateUpdate(" << id << ") - Delay countdown: " << m_delay << std::endl;
#endif // DEBUG_PRINT
            }
            else if (p_data->attacking)
            {
                if (p_data->p_attackData && p_data->p_attackData->ExecuteAttack(deltaTime))
                {
                    p_data->p_attackData->DisableAttackObjects();
                    p_data->attacking = false;
                    p_data->finishedExecution = true;
                }
            } // end of if (p_data->attacking)
        } // end of if (gameStateController->currentState == GameStates_v2_0::EXECUTE) 
        else if (gameStateController->currentState == GameStates_v2_0::PLANNING)
        {
            // Change states during the next planning phase
            ChangeStates();
        } // end of if (gameStateController->currentState == GameStates_v2_0::PLANNING) 
        
    }

    void RatAttack_v2_0::StateCleanUp()
    {
        p_data = nullptr;
        gameStateController = nullptr;

        // Unsubscribe from collision events
        REMOVE_KEY_COLLISION_LISTENER(m_triggerEnterEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_triggerStayEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_triggerExitEventListener);
    }

    void RatAttack_v2_0::StateExit(EntityID id)
    {
        // empty
#ifdef DEBUG_PRINT
        //std::cout << "[DEBUG] RatAttack_v2_0::StateExit - Rat ID: " << id << " exiting Attack state." << std::endl;
#endif // DEBUG_PRINT
    }


    void RatScript_v2_0::DealDamageToCat(EntityID collidedCat, EntityID rat)
    {
        // Ensure that we do not attack the same cat twice in the same frame
        auto ratIt = m_scriptData.find(rat);
        if (ratIt != m_scriptData.end())
        {
            auto catIt{ ratIt->second.attackedCats.find(collidedCat) };
            if (catIt != ratIt->second.attackedCats.end()) {
                // cat has already been damaged
                return;
            }

            // Add the cat to the map
            ratIt->second.attackedCats.emplace(collidedCat);
            ratIt->second.hitCat = true;
        }

        GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->KillCat(collidedCat);
    }


    void RatAttack_v2_0::ChangeStates()
    {
        // Clear dead cats from the collision sets
        RatScript_v2_0::ClearDeadCats(p_data->catsInDetectionRadius);
        RatScript_v2_0::ClearDeadCats(p_data->catsExitedDetectionRadius);

        // Check if there are any cats in the detection range
        if (!(p_data->catsInDetectionRadius.empty()))
        {
            // there's a cat in the detection range, move to attack it again
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToMovement(p_data->myID);
        }
        // Check if any cats exited the detection range
        else if (!(p_data->catsExitedDetectionRadius.empty()))
        {
            // Check for the closest cat
            EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) };

            // a cat just passed by us, hunt it down
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToHunt(p_data->myID, closestCat);
        }
        // No cats in detection range
        else
        {
            // the cat we're chasing is dead, return to the original position
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(p_data->myID);
        }
    }


    void RatAttack_v2_0::OnCollisionEnter(const Event<CollisionEvents>& r_event)
    {
        if (!p_data) { return; }
        else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        if (r_event.GetType() == CollisionEvents::OnCollisionEnter)
        {
            OnCollisionEnterEvent OCEE = dynamic_cast<OnCollisionEnterEvent const&>(r_event);
            // check if rat and cat have collided
            bool touchingCat{ GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatTouchingCat(p_data->myID, OCEE.Entity1, OCEE.Entity2) };

            if (!touchingCat && p_data->attacking && p_data->p_attackData) // Currently attacking 
            {
                p_data->p_attackData->OnCollisionEnter(OCEE.Entity1, OCEE.Entity2);
            } // end of if (p_data->attacking)
        }
    }

    void RatAttack_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }
        if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
        {
            OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
            // check if a cat has entered the rat's detection collider
            bool inDetectionRadius{ GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerEntered(p_data->myID, OTEE.Entity1, OTEE.Entity2) };
            
            if (!inDetectionRadius && p_data->attacking && p_data->p_attackData) // Currently attacking 
            {
                p_data->p_attackData->OnCollisionEnter(OTEE.Entity1, OTEE.Entity2);
            } // end of if (p_data->attacking)
        }
        else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
        {
            OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
            // check if a cat has entered the rat's detection collider
            bool inDetectionRadius{ GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerEntered(p_data->myID, OTSE.Entity1, OTSE.Entity2) };

            if (!inDetectionRadius && p_data->attacking && p_data->p_attackData) // Currently attacking 
            {
                p_data->p_attackData->OnCollisionEnter(OTSE.Entity1, OTSE.Entity2);
            } // end of if (p_data->attacking)
        } // end of if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
    }


    void RatAttack_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (gameStateController && gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
        // check if a cat has exited the rat's detection collider
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerExited(p_data->myID, OTEE.Entity1, OTEE.Entity2);
    }
} // namespace PE
