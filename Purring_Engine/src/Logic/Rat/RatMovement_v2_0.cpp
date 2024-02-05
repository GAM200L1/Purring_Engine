#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatMovement_v2_0.h"
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
    }

    void RatMovement_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        std::cout << "RatMovement_v2_0::StateUpdate - Rat ID: " << id << " is updating." << std::endl;

        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            std::cout << "RatMovement_v2_0::StateUpdate - Game is paused." << std::endl;
            return;
        }

        CalculateMovement(id, deltaTime);
    }

    void RatMovement_v2_0::StateExit(EntityID id)
    {
        std::cout << "RatMovement_v2_0::StateExit - Rat ID: " << id << " is exiting the movement state." << std::endl;
        p_data->ratPlayerDistance = 0.f;
    }

    void RatMovement_v2_0::CalculateMovement(EntityID id, float deltaTime)
    {
        if (p_data->ratPlayerDistance > 0.f)
        {
            vec2 newPosition = RatScript_v2_0::GetEntityPosition(id) + (p_data->directionFromRatToPlayerCat * p_data->movementSpeed * deltaTime);
            RatScript_v2_0::PositionEntity(id, newPosition);
            p_data->ratPlayerDistance -= p_data->movementSpeed * deltaTime;

            std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id
                << " moved to new position: (" << newPosition.x << ", " << newPosition.y
                << "), Remaining distance: " << p_data->ratPlayerDistance << std::endl;

            if (CheckDestinationReached(newPosition, RatScript_v2_0::GetEntityPosition(p_data->mainCatID)))
            {
                std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id << " has reached the destination." << std::endl;
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->TriggerStateChange(id, 0.f);
            }
        }
        else
        {
            std::cout << "RatMovement_v2_0::CalculateMovement - Rat ID: " << id << " has no movement or already at destination." << std::endl;
        }
    }

    bool RatMovement_v2_0::CheckDestinationReached(const vec2& newPosition, const vec2& targetPosition)
    {
        bool reached = (newPosition - targetPosition).Length() <= minDistanceToTarget;
        std::cout << "RatMovement_v2_0::CheckDestinationReached - Destination " << (reached ? "reached." : "not reached.") << std::endl;
        return reached;
    }
}
