/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatAttack_v2_0.cpp
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

namespace PE
{
    void RatAttack_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);                                                   // Get GSM instance

        m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatAttack_v2_0::OnTriggerEnterAndStay, this);
        m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatAttack_v2_0::OnTriggerEnterAndStay, this);
        m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatAttack_v2_0::OnTriggerExit, this);

        m_delay = 0.1f;
        p_data->attacking = true;  // Set the attacking flag to true
        attackFeedbackOnce = false;
        m_attackDuration = 0; // will be set later when the attack animation plays

        // Position the attack telegraph
        RatScript_v2_0::PositionEntity(p_data->attackTelegraphEntityID, RatScript_v2_0::GetEntityPosition(p_data->myID));

        p_data->attackedCats.clear();
        //std::cout << "[DEBUG] RatAttack_v2_0::StateEnter - Rat ID: " << id << " transitioning to Attack state." << std::endl;
    }

    void RatAttack_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        //std::cout << "[DEBUG] RatAttack_v2_0::StateUpdate - Rat ID: " << id << " is updating with attacking status: " << p_data->attacking << std::endl;

        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            //std::cout << "[DEBUG] RatMovement_v2_0::StateUpdate - Game is paused." << std::endl;
            return;
        }

        if (gameStateController->currentState == GameStates_v2_0::EXECUTE) { 
            if (m_delay > 0.f && p_data->attacking)
            {
                m_delay -= deltaTime;
                //std::cout << "[DEBUG] RatAttack_v2_0::StateUpdate(" << id << ") - Delay countdown: " << m_delay << std::endl;
            }
            else if (p_data->attacking)
            {
                //std::cout << "RatAttack_v2_0::StateUpdate(" << id << "): p_data->attacking true" << std::endl;

                //std::cout << "[DEBUG] RatAttack_v2_0::StateUpdate - Attack initiated, enabling telegraph." << std::endl;
                RatScript_v2_0::ToggleEntity(p_data->attackTelegraphEntityID, true);
                m_attackDuration -= deltaTime;

                if (!attackFeedbackOnce)
                {
                    attackFeedbackOnce = true;
                    RatScript_v2_0::PlayAttackAudio();
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(p_data->myID, EnumRatAnimations::ATTACK);
                    m_attackDuration = GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetAnimationDuration(p_data->myID);
                }
                else if (m_attackDuration <= 0.f)
                {
                    std::cout << "RatAttack_v2_0::StateUpdate(" << id << "): animation is done" << std::endl;
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(p_data->myID, EnumRatAnimations::IDLE);

                    RatScript_v2_0::ToggleEntity(p_data->attackTelegraphEntityID, false);
                    p_data->attacking = false;
                    p_data->finishedExecution = true;

                    ChangeStates();
                }
            } // if (p_data->attacking)
        } // if (gameStateController->currentState == GameStates_v2_0::EXECUTE) 
        
    }

    void RatAttack_v2_0::StateCleanUp()
    {
        REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
    }

    void RatAttack_v2_0::StateExit(EntityID id)
    {
        // Cleanup attack-specific data here
        gameStateController = nullptr;
        //std::cout << "[DEBUG] RatAttack_v2_0::StateExit - Rat ID: " << id << " exiting Attack state." << std::endl;
        p_data->hitCat = false;
    }


    void RatScript_v2_0::DealDamageToCat(EntityID collidedCat, EntityID rat)
    {
        // A
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
        ClearDeadCats(p_data->catsInDetectionRadius);
        ClearDeadCats(p_data->catsExitedDetectionRadius);

        // Check if there are any cats in the detection range
        if (!(p_data->catsInDetectionRadius.empty()))
        {
            std::cout << "RatAttack_v2_0::ChangeStates(" << p_data->myID << "): cats in detection radius\n";

            // there's a cat in the detection range, move to attack it again
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToMovement(p_data->myID);
        }
        // Check if any cats exited the detection range
        else if (!(p_data->catsExitedDetectionRadius.empty()))
        {
            std::cout << "RatAttack_v2_0::ChangeStates(" << p_data->myID << "): cats exited radius\n";
            // Check for the closest cat
            EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) };

            // a cat just passed by us, hunt it down
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToHunt(p_data->myID, closestCat);
        }
        // No cats in detection range
        else
        {
            std::cout << "RatAttack_v2_0::ChangeStates(" << p_data->myID << "): no cats in detection radius\n";

            // the cat we're chasing is dead, return to the original position
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToReturn(p_data->myID);
        }
    }

    void RatAttack_v2_0::ClearDeadCats(std::set<EntityID>& catSet)
    {
        // Store the indices of the dead cats 
        std::vector<EntityID> deadCats{};
        for (EntityID const& id : catSet) 
        {
            if (!(GETSCRIPTINSTANCEPOINTER(CatController_v2_0)->IsCat(id))) 
            {
                deadCats.emplace_back(id);
            }
        }

        // Remove the cats from the set
        for (EntityID const& id : deadCats) 
        {
            catSet.erase(id);
        }
    }

    void RatAttack_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
        {
            OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
            // check if entity1 is the rat's detection collider and entity2 is cat
            if ((OTEE.Entity1 == p_data->detectionRadiusId) && RatScript_v2_0::GetIsCat(OTEE.Entity2))
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity2);
            }
            // check if entity2 is the rat's detection collider and entity1 is cat
            else if ((OTEE.Entity2 == p_data->detectionRadiusId) && RatScript_v2_0::GetIsCat(OTEE.Entity1))
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity1);
            }
            else if (p_data->attacking) // Currently attacking 
            {
                // check if entity1 is rat or rat's attack and entity2 is cat
                if ((OTEE.Entity1 == p_data->myID || OTEE.Entity1 == p_data->attackTelegraphEntityID) &&
                    RatScript_v2_0::GetIsCat(OTEE.Entity2))
                {
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(OTEE.Entity2, p_data->myID);
                }
                // check if entity2 is rat or rat's attack and entity1 is cat
                else if ((OTEE.Entity2 == p_data->myID || OTEE.Entity2 == p_data->attackTelegraphEntityID) &&
                    RatScript_v2_0::GetIsCat(OTEE.Entity1))
                {
                    // save the id of the cat that has been checked so that it wont be checked again
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(OTEE.Entity1, p_data->myID);
                }
            } // end of if (p_data->attacking)
        }
        else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
        {
            OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
            // check if entity1 is the rat's detection collider and entity2 is cat
            if ((OTSE.Entity1 == p_data->detectionRadiusId) && RatScript_v2_0::GetIsCat(OTSE.Entity2))
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity2);
            }
            // check if entity2 is the rat's detection collider and entity1 is cat
            else if ((OTSE.Entity2 == p_data->detectionRadiusId) && RatScript_v2_0::GetIsCat(OTSE.Entity1))
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity1);
            }
            else if (p_data->attacking) // Currently attacking 
            {
                // check if entity1 is rat or rat's attack and entity2 is cat
                if ((OTSE.Entity1 == p_data->myID || OTSE.Entity1 == p_data->attackTelegraphEntityID) &&
                    RatScript_v2_0::GetIsCat(OTSE.Entity2))
                {
                    // save the id of the cat that has been checked so that it wont be checked again
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(OTSE.Entity2, p_data->myID);
                }
                // check if entity2 is rat or rat's attack and entity1 is cat
                else if ((OTSE.Entity2 == p_data->myID || OTSE.Entity2 == p_data->attackTelegraphEntityID) &&
                    RatScript_v2_0::GetIsCat(OTSE.Entity1))
                {
                    // save the id of the cat that has been checked so that it wont be checked again
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DealDamageToCat(OTSE.Entity1, p_data->myID);
                }
            } // end of if (p_data->attacking)
                
        } // end of if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
    }


    void RatAttack_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
        // check if entity1 is the rat's detection collider and entity2 is cat
        if ((OTEE.Entity1 == p_data->detectionRadiusId) && RatScript_v2_0::GetIsCat(OTEE.Entity2))
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity2);
        }
        // check if entity2 is the rat's detection collider and entity1 is cat
        else if ((OTEE.Entity2 == p_data->detectionRadiusId) && RatScript_v2_0::GetIsCat(OTEE.Entity1))
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity1);
        }
    }
} // namespace PE
