#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatMovement_v2_0.h"
#include "Logic/Rat/RatAttack_v2_0.h"

#include "Events/EventHandler.h"

#include "Math/MathCustom.h"
#include "Math/Transform.h"

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
    }

    void RatMovement_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            //std::cout << "RatMovement_v2_0::StateUpdate - Game is paused." << std::endl;
            return;
        }

        // Assume rat is idle by default
        bool isRatMoving = false;

        // If the game state is execute, keep track of any cats that pass through the rat's radius
        if (gameStateController->currentState == GameStates_v2_0::EXECUTE)
        {
            m_planningRunOnce = false;

            switch (p_data->ratType)
            {
            case EnumRatType::GUTTER:
            case EnumRatType::BRAWLER:
            {
                // Move towards the target position until we've reached 
                // The function returns true if the target has been reached
                if (GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CalculateMovement(p_data->myID, deltaTime))
                {
                    // The rat should stop moving, so set the animation to idle
                    if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->myID))
                    {
                        EntityManager::GetInstance().Get<AnimationComponent>(p_data->myID).SetCurrentAnimationID(p_data->animationStates.at("Idle"));
                    }

                    // Switch to the attack state since we're close enough to the targets
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToAttack(id);
                    std::cout << "Attack State";
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

                    // Pick a target position to move to
                    vec2 targetPosition{ PickTargetPosition() };

                    // ---- Update telegraph
                    // Rotate the telegraph to face the target
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->EnableTelegraphs(id, targetPosition);
                }

                break; // end of BRAWLER rat type
            }
            default: break;
            }
        }


        // Get the animation to play
        UpdateAnimation(isRatMoving); // @TODO to remove this boolean bc it should always be true

        // Store game state of frame
        m_previousGameState = gameStateController->currentState;
    }


    void RatMovement_v2_0::StateCleanUp()
    {
        REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
    }

    void RatMovement_v2_0::StateExit(EntityID id)
    {
        //std::cout << "RatMovement_v2_0::StateExit - Rat ID: " << id << " is exiting the movement state." << std::endl;
        p_data->ratPlayerDistance = 0.f;
    }

    void RatMovement_v2_0::RatHitCat(const Event<CollisionEvents>& r_TE)
    {
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->RatHitCat(p_data->myID, r_TE);
    }


    void RatMovement_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }
        else if (gameStateController->currentState != GameStates_v2_0::EXECUTE) { return; }

        if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
        {
            OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
            // check if entity1 is the rat's detection collider and entity2 is cat
            if ((OTEE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity2);
            }
            // check if entity2 is the rat's detection collider and entity1 is cat
            else if ((OTEE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity1);
            }
        }
        else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
        {
            OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
            // check if entity1 is the rat's detection collider and entity2 is cat
            if ((OTSE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity2) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity2).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity2);
            }
            // check if entity2 is the rat's detection collider and entity1 is cat
            else if ((OTSE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity1) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity1);
            }
        }
    }


    void RatMovement_v2_0::UpdateAnimation(bool const isRatMoving)
    {
        // Update the animation of the rat
        if (isRatMoving)
        {
            // Set to "Walk" animation only if movement occurred
            if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->myID))
            {
                EntityManager::GetInstance().Get<AnimationComponent>(p_data->myID).SetCurrentAnimationID(p_data->animationStates.at("Walk"));
            }
        }
        else
        {
            // Set to "Idle" animation if the rat didn't move
            if (EntityManager::GetInstance().Has<AnimationComponent>(p_data->myID))
            {
                EntityManager::GetInstance().Get<AnimationComponent>(p_data->myID).SetCurrentAnimationID(p_data->animationStates.at("Idle"));
            }
        }
    }



    vec2 RatMovement_v2_0::PickTargetPosition()
    {
        // Pick the closest cat to move 
        vec2 ratPosition = RatScript_v2_0::GetEntityPosition(p_data->myID);

        // Find the closest cat within the detection radius.
        EntityID closestCat = 0;
        vec2 closestPosition{ RatScript_v2_0::GetEntityPosition(p_data->myID) };
        float minDistance = std::numeric_limits<float>::max();
        for (auto& catID : p_data->catsInDetectionRadius)
        {
            vec2 catPosition = RatScript_v2_0::GetEntityPosition(catID);
            float distance = (catPosition - RatScript_v2_0::GetEntityPosition(p_data->myID)).Length();
            if (distance < minDistance)
            {
                minDistance = distance;
                closestCat = catID;
                closestPosition = RatScript_v2_0::GetEntityPosition(catID);
            }
        }

        // Set the position for the rat to move to
        m_targetId = closestCat;
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->SetTarget(p_data->myID, closestPosition, false);
        return closestPosition;
    }

}
