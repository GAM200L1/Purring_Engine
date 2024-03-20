/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatAttack_v2_0.cpp
 \date     17-01-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     60%
 \par      changes:    Majority rat idle/patrol AI logic.

 \co-author            Krystal Yamin
 \par      email:      krystal.y\@digipen.edu
 \par      code %:     40%
 \par      changes:    06-02-2024
                       Integration of gamestates with the Rat States and Rat AI.
 \brief
    This file contains functions for the rat idle/patrol state.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "Logic/Rat/RatIdle_v2_0.h"

#include "Logic/Rat/RatScript_v2_0.h"
//
#include "Physics/CollisionManager.h"
#include "Animation/Animation.h"
#include "Logic/GameStateController_v2_0.h"
#include "Math/MathCustom.h"
#include "Math/Transform.h"

//#define DEBUG_PRINT

namespace PE
{
    // ---------- RAT IDLE STATE v2.0 ---------- //

    RatIdle_v2_0::RatIdle_v2_0(bool _willPatrol)
        : p_data(nullptr), m_willPatrol(_willPatrol), m_gameStateController(nullptr)
    { /* Empty by design */ }

    void RatIdle_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        m_gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);                   // Get GSM instance
        m_planningRunOnce = false;

        // Subscribe to collision events
        m_collisionEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionEnter, RatIdle_v2_0::OnCollisionEnterOrStay, this);
        m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionStay, RatIdle_v2_0::OnCollisionEnterOrStay, this);
        m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnCollisionExit, RatIdle_v2_0::OnCollisionExit, this);
        
        m_triggerEnterEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatIdle_v2_0::OnTriggerEnterAndStay, this);
        m_triggerStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatIdle_v2_0::OnTriggerEnterAndStay, this);
        m_triggerExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatIdle_v2_0::OnTriggerExit, this);


        // ----- Reset -----
        if (m_willPatrol)
        {
            if (p_data->patrolPoints.size() == 0)
                m_willPatrol = false; // There are no patrol points so don't try patrolling
            else
                InitializePatrolPoints(); // Initialize patrol points if the rat type is PATROL
        }

        p_data->finishedExecution = (!m_willPatrol);

        // Play animation
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::IDLE);
    }


    void RatIdle_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        if (m_gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;
        }

        if (m_gameStateController->currentState == GameStates_v2_0::EXECUTE)
        {
            if (m_planningRunOnce && m_willPatrol) 
            {
                // Play walk animation execution first starts
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::WALK);

                // Disable movement telegraphs
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->DisableMovementTelegraphs(id);
            }
            m_planningRunOnce = false;

            // Update its position if it should be patrolling
            if (m_willPatrol)
            {
                PatrolLogic(id, deltaTime);
            }
        }
        else if(m_gameStateController->currentState == GameStates_v2_0::PLANNING)
        {
            if (!m_planningRunOnce)
            {
                m_planningRunOnce = true;
                p_data->finishedExecution = (!m_willPatrol);

                // Play idle animation
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::IDLE);

                // Rotate the telegraph to face the target
                if (m_willPatrol)
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->EnableMovementTelegraphs(id, p_data->patrolPoints[m_patrolIndex]);

                // Check if any cats have entered or exited
                // the rat's detection range and change state accordingly
                if (!(p_data->hasRatStateChanged))
                    CheckIfShouldChangeStates();
            }
        }
    }


    void RatIdle_v2_0::PatrolLogic(EntityID id, float deltaTime)
    {
        if (p_data->patrolPoints.empty()) 
        {
            m_willPatrol = false;
            p_data->finishedExecution = true;
            return;
        }
        else if (p_data->finishedExecution) { return; } // Don't update movement further if we're done 

        bool setNewTarget{ GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CalculateMovement(id, deltaTime) };

        // Stop moving this turn if we have reached our destination
        p_data->finishedExecution = setNewTarget;

        // Update to next target if we have reached
        if (setNewTarget)
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->PlayAnimation(id, EnumRatAnimations::IDLE);
            UpdatePatrolTarget();
        }
    }


    void RatIdle_v2_0::UpdatePatrolTarget() 
    {
        // Check if the rat should move in the positive order of the patrol points (i.e. 0 -> 1 -> 2)
        if (!m_returnToFirstPoint)
        {
            // Positive order
            ++m_patrolIndex;
            if (m_patrolIndex >= p_data->patrolPoints.size())
            {
                m_patrolIndex = std::min(0, static_cast<int>(p_data->patrolPoints.size()) - 2);
                m_returnToFirstPoint = true;
            }
        }
        else
        {
            // Negative order
            --m_patrolIndex;
            if (m_patrolIndex < 0)
            {
                m_patrolIndex = 1;
                m_returnToFirstPoint = false;
            }
        }

        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, p_data->patrolPoints[m_patrolIndex], false);
    }


    void RatIdle_v2_0::StateCleanUp()
    {
        p_data = nullptr;
        m_gameStateController = nullptr;

        // Unsubscribe from events
        REMOVE_KEY_COLLISION_LISTENER(m_collisionEnterEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);

        REMOVE_KEY_COLLISION_LISTENER(m_triggerEnterEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_triggerStayEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_triggerExitEventListener);
    }

    void RatIdle_v2_0::StateExit(EntityID id)
    {
        // empty
    }


    void RatIdle_v2_0::CheckIfShouldChangeStates()
    {
        // Clear dead cats from the collision sets
        RatScript_v2_0::ClearDeadCats(p_data->catsInDetectionRadius);
        RatScript_v2_0::ClearDeadCats(p_data->catsExitedDetectionRadius);

        // Check if there are any cats in the detection range
        if (!(p_data->catsInDetectionRadius.empty()))
        {
            // Change to aggressive state
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToMovement(p_data->myID);
        }
        // Check if any cats exited the radius during execution
        else if (!(p_data->catsExitedDetectionRadius.empty()))
        {
            // Check for the closest cat
            EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) };

            // Change to hunting state
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToHunt(p_data->myID, closestCat);
        }
        else
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ClearCollisionContainers(p_data->myID);
        }
    }


    void RatIdle_v2_0::OnCollisionEnterOrStay(const Event<CollisionEvents>& r_event)
    {
        if (!p_data) { return; }
        else if (m_gameStateController && m_gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

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
                p_data->finishedExecution = true;
                p_data->ratPlayerDistance = 0.f;
            }
        }
        else if (r_event.GetType() == CollisionEvents::OnCollisionStay)
        {
            OnCollisionStayEvent OCSE = dynamic_cast<OnCollisionStayEvent const&>(r_event);

            // Check if the rat has been colliding with a wall
            if (GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatTouchingWall(p_data->myID, OCSE.Entity1, OCSE.Entity2) &&
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetExecutionPhaseTimeout(p_data->myID))
            {
                // The rat has been touching the wall for too long
                p_data->finishedExecution = true;
                p_data->ratPlayerDistance = 0.f;
            }
        }
    }


    void RatIdle_v2_0::OnCollisionExit(const Event<CollisionEvents>& r_event)
    {
        if (!p_data) { return; }
        else if (m_gameStateController && m_gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        if (r_event.GetType() == CollisionEvents::OnCollisionExit)
        {
            OnCollisionExitEvent OCEE = dynamic_cast<OnCollisionExitEvent const&>(r_event);
            // check if rat has stopped colliding with wall
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckRatStopTouchingWall(p_data->myID, OCEE.Entity1, OCEE.Entity2);
        }
    }


    void RatIdle_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (m_gameStateController && m_gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

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


    void RatIdle_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (m_gameStateController && m_gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
        // check if a cat has exited the rat's detection collider
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckDetectionTriggerExited(p_data->myID, OTEE.Entity1, OTEE.Entity2);
    }


    void RatIdle_v2_0::InitializePatrolPoints()
    {        
        // Add the rat's current position to the start of the patrol points
        p_data->patrolPoints.emplace(p_data->patrolPoints.begin(), 
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->GetEntityPosition(p_data->myID));

        m_patrolIndex = (p_data->patrolPoints.size() > 1 ? 1 : 0);
        m_returnToFirstPoint = false;

        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, p_data->patrolPoints[m_patrolIndex], false);
    }


    void RatIdle_v2_0::SetPatrolPoints(const std::vector<vec2>& points)
    {
        p_data->patrolPoints.clear();
        for (const auto& point : points)
        {
            p_data->patrolPoints.push_back(point);
        }
    }

} // namespace PE
