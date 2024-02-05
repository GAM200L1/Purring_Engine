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

        m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatMovement_v2_0::RatHitCat, this);
        m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatMovement_v2_0::RatHitCat, this);
        p_data->attacking = (p_data->ratPlayerDistance > 0.f) ? true : false;
    }

    void RatMovement_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        std::cout << "RatMovement_v2_0::StateUpdate - Rat ID: " << id << " is updating." << std::endl;

        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            std::cout << "RatMovement_v2_0::StateUpdate - Game is paused." << std::endl;
            return;
        }

        // Check if any cat is within detection range and move towards the nearest one
        if (!p_data->catsInDetectionRadius.empty())
        {
            CalculateMovement(id, deltaTime);

            // After moving, check if the rat is within attack range of any cat
            for (const auto& catID : p_data->catsInDetectionRadius)
            {
                vec2 catPosition = RatScript_v2_0::GetEntityPosition(catID);
                if (IsWithinAttackRange(id, catPosition))
                {
                    std::cout << "RatMovement_v2_0::StateUpdate - Rat ID: " << id << " is within attack range of Cat ID: " << catID << std::endl;
                    // Transition to attack state
                    GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->TriggerStateChange(id, 0.f, new RatAttack_v2_0());
                    return;
                }
            }
        }
    }

    bool RatMovement_v2_0::IsWithinAttackRange(EntityID ratID, const vec2& catPosition)
    {
        vec2 ratPosition = RatScript_v2_0::GetEntityPosition(ratID);
        float distance = (ratPosition - catPosition).Length();
        return distance <= p_data->attackRange;
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
        // Only move if not in attack mode and there are cats in the detection radius
        if (!p_data->attacking && !p_data->catsInDetectionRadius.empty())
        {
            vec2 newPosition = RatScript_v2_0::GetEntityPosition(id) + (p_data->directionFromRatToPlayerCat * p_data->movementSpeed * deltaTime);
            RatScript_v2_0::PositionEntity(id, newPosition);

            std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id
                << " moved to new position: (" << newPosition.x << ", " << newPosition.y << ")\n";

            // Check if any cat is still in range after moving
            if (p_data->catsInDetectionRadius.empty())
            {
                std::cout << "RatMovement_v2_0::CalculateMovement - No cats in range, switching to idle state.\n";
                //GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->TriggerStateChange(id, 0.f, new RatAttack_v2_0());
            }
        }
        else
        {
            std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id << " is in attack mode or no cats in range.\n";
        }
    }


    bool RatMovement_v2_0::CheckDestinationReached(const vec2& newPosition, const vec2& targetPosition)
    {
        bool reached = (newPosition - targetPosition).Length() <= minDistanceToTarget;
        std::cout << "RatMovement_v2_0::CheckDestinationReached - Destination " << (reached ? "reached." : "not reached.") << std::endl;
        return reached;
    }
}
