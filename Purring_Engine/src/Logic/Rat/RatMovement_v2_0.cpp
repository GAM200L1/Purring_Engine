#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatMovement_v2_0.h"
#include "Logic/Rat/RatAttack_v2_0.h"

#include "Events/EventHandler.h"

#include "Math/MathCustom.h"
#include "Math/Transform.h"

namespace PE
{
    RatMovement_v2_0::RatMovement_v2_0() : gameStateController(nullptr), p_data(nullptr) {}

    void RatMovement_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

        vec2 playerPosition = RatScript_v2_0::GetEntityPosition(p_data->mainCatID);
        vec2 ratPosition = RatScript_v2_0::GetEntityPosition(id);

        // Calculate the distance and direction from the rat to the player cat
        p_data->ratPlayerDistance = (playerPosition - ratPosition).Length();
        p_data->directionFromRatToPlayerCat = (playerPosition - ratPosition).GetNormalized();

        std::cout << "Rat ID: " << id
            << " - Initial ratPlayerDistance: " << p_data->ratPlayerDistance
            << ", Initial directionFromRatToPlayerCat: "
            << p_data->directionFromRatToPlayerCat.x << ", "
            << p_data->directionFromRatToPlayerCat.y << std::endl;

        m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatMovement_v2_0::OnTriggerEnterAndStay, this);
        m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatMovement_v2_0::OnTriggerEnterAndStay, this);
        //m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatMovement_v2_0::OnTriggerExit, this);

    }

    void RatMovement_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        std::cout << "RatMovement_v2_0::StateUpdate - Rat ID: " << id << " is updating." << std::endl;

        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            std::cout << "RatMovement_v2_0::StateUpdate - Game is paused." << std::endl;
            return;
        }

        if (!p_data->catsInDetectionRadius.empty())
        {
            CalculateMovement(id, deltaTime);
        }
        p_data->catsExitedDetectionRadius.clear();

    }

    void RatMovement_v2_0::StateCleanUp()
    {
        REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
    }

    void RatMovement_v2_0::StateExit(EntityID id)
    {
        std::cout << "RatMovement_v2_0::StateExit - Rat ID: " << id << " is exiting the movement state." << std::endl;
        p_data->ratPlayerDistance = 0.f;
    }

    void RatMovement_v2_0::RatHitCat(const Event<CollisionEvents>& r_TE)
    {
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->RatHitCat(p_data->myID, r_TE);
    }

    void RatMovement_v2_0::CalculateMovement(EntityID id, float deltaTime)
    {
        // If there are no cats detected within the detection radius, exit early.
        if (p_data->catsInDetectionRadius.empty()) return;

        vec2 ratPosition = RatScript_v2_0::GetEntityPosition(id);

        // Find the closest cat within the detection radius.
        EntityID closestCat = 0;
        float minDistance = std::numeric_limits<float>::max();
        for (auto& catID : p_data->catsInDetectionRadius)
        {
            vec2 catPosition = RatScript_v2_0::GetEntityPosition(catID);
            float distance = (catPosition - ratPosition).Length();
            if (distance < minDistance)
            {
                minDistance = distance;
                closestCat = catID;
            }
        }

        // Move towards the closest cat if it's not within the attack radius.
        if (minDistance > p_data->attackRadius)
        {
            vec2 directionToCat = (RatScript_v2_0::GetEntityPosition(closestCat) - ratPosition).GetNormalized();
            vec2 newPosition = ratPosition + directionToCat * p_data->movementSpeed * deltaTime;
            RatScript_v2_0::PositionEntity(id, newPosition);

            std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id
                << " moved towards Cat ID: " << closestCat
                << " New Position: (" << newPosition.x << ", " << newPosition.y << ")\n";
        }
        else
        {
            // Transition to attack state if the closest cat is within attack radius.
            std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id
                << " is within attack radius of Cat ID: " << closestCat << ". Initiating attack.\n";
            //GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->TriggerStateChange(id, 0.f);
        }
    }


    bool RatMovement_v2_0::CheckDestinationReached(const vec2& newPosition, const vec2& targetPosition)
    {
        bool reached = (newPosition - targetPosition).Length() <= minDistanceToTarget;
        std::cout << "RatMovement_v2_0::CheckDestinationReached - Destination " << (reached ? "reached." : "not reached.") << std::endl;
        return reached;
    }

    void RatMovement_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }

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

    void RatMovement_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }

        OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
        // check if entity1 is the rat's detection collider and entity2 is cat
        if ((OTEE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
            EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity2);
        }
        // check if entity2 is the rat's detection collider and entity1 is cat
        else if ((OTEE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
            EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity1);
        }
    }

}
