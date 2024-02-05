#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatMovement_v2_0.h"
#include "Math/MathCustom.h"
#include "Math/Transform.h"
namespace PE
{
    RatMovement_v2_0::RatMovement_v2_0()
        : gameStateController(nullptr), p_data(nullptr), canMove(false), isMoving(false), hasReachedDestination(false), moveSpeed(0.0f), minDistanceToTarget(1.0f)
    {
    }

    void RatMovement_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);                                   // Get GSM instance

        // Initialize movement variables
        canMove = true;                                                                 // Allow movement
        moveSpeed = p_data->movementSpeed;                                              // Set move speed from RatScript_v2_0_Data

        //p_data->targetPosition = RatScript_v2_0::GetEntityPosition(p_data->mainCatID); // Target position is the position of the main cat

    }

    void RatMovement_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;
        }
        gameStateController->currentState = GameStates_v2_0::EXECUTE;

        if (gameStateController->currentState == GameStates_v2_0::EXECUTE && canMove)
        {
            Move(id, deltaTime); // Execute the movement logic
        }

    }

    void RatMovement_v2_0::StateExit(EntityID id)
    {
        canMove = false; // Reset movement permission
        isMoving = false; // Reset movement state
    }

    void RatMovement_v2_0::Move(EntityID id, float deltaTime)
    {
        if (!canMove || hasReachedDestination) return;

        // get the current position
        vec2 currentPosition = RatScript_v2_0::GetEntityPosition(id);

        // Calculate the target position using the direction and the distance to the player
        vec2 targetPosition = currentPosition + (p_data->directionFromRatToPlayerCat * p_data->ratPlayerDistance);

        // Calculate the direction and move towards the target
        vec2 direction = (targetPosition - currentPosition).GetNormalized();
        vec2 newPosition = currentPosition + direction * moveSpeed * deltaTime;

        // Move the rat to the new position
        RatScript_v2_0::PositionEntity(id, newPosition);

        std::cout << "Moving Rat ID " << id << " from (" << currentPosition.x << ", " << currentPosition.y << ") to (" << newPosition.x << ", " << newPosition.y << ")" << std::endl;

        // Check if the destination has been reached
        if ((targetPosition - newPosition).Length() < minDistanceToTarget)
        {
            DestinationReached(id);
        }

        isMoving = true; // Rat is moving
    }

    void RatMovement_v2_0::DestinationReached(EntityID id)
    {
        isMoving = false;                                                               // Stop moving
        hasReachedDestination = true;                                                   // Mark destination as reached

        // Play any animations or sounds associated with reaching the destination
        // p_data->SetAnimatorBool("isMoving", false);

        // If an attack is to follow, you can transition to the attack state here
        // gameStateController->ChangeState(new RatAttack_v2_0(), id);
    }

}
