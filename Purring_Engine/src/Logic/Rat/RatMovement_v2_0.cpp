#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatMovement_v2_0.h"

namespace PE
{
    RatMovement_v2_0::RatMovement_v2_0()
        : gameStateController(nullptr), p_data(nullptr), canMove(false), isMoving(false), hasReachedDestination(false), moveSpeed(0.0f)
    {
    }
    void RatMovement_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);                                   // Get GSM instance

        // Initialize movement variables
        canMove = true;                                                                 // Allow movement
        moveSpeed = p_data->movementSpeed;                                              // Set move speed from RatScript_v2_0_Data
    }

    void RatMovement_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;
        }

        if (gameStateController->currentState == GameStates_v2_0::EXECUTE && canMove)
        {
            Move(id, deltaTime); // Execute the movement logic
        }
    }

    void RatMovement_v2_0::StateExit(EntityID id)
    {
        canMove = false; // Reset movement permission
        isMoving = false; // Reset movement state
        // Perform any cleanup required
    }

    void RatMovement_v2_0::Move(EntityID id, float deltaTime)
    {
        if (!isMoving || hasReachedDestination) return;

        // Implement actual movement logic here
        // For example: move towards a predetermined point or follow a path

        // Check if the destination has been reached
        //if (/* condition to check if destination is reached */)
        //{
        //    DestinationReached(id);
        //}
    }

    void RatMovement_v2_0::DestinationReached(EntityID id)
    {
        isMoving = false;                                                               // Stop moving
        hasReachedDestination = true;                                                   // Mark destination as reached

        // Implement any logic that should occur when the destination is reached
        // For example: trigger an event, start an attack, etc.
    }

    // Implement any additional functions needed for movement logic
}
