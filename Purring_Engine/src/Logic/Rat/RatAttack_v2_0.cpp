#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatAttack_v2_0.h"

namespace PE
{
    // Constructor
    RatAttack_v2_0::RatAttack_v2_0()
        : gameStateController(nullptr), p_data(nullptr), catInRange(false), hasActed(false)
    {
    }

    void RatAttack_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);       // Get GSM instance
        catInRange = false;                                                             // Reset cat in range flag
        hasActed = false;                                                               // Reset action flag
    }

    void RatAttack_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;                                                                     // Pause state, do nothing
        }

        if (gameStateController->currentState == GameStates_v2_0::EXECUTE && catInRange && !hasActed)
        {
            AttackLogic(id, deltaTime);                                                 // Execute the attack logic
        }
    }

    void RatAttack_v2_0::StateExit(EntityID id)
    {
        // Cleanup attack-specific data here
        gameStateController = nullptr;
        catInRange = false;                                                             // Reset cat in range flag
        hasActed = false;                                                               // Reset action flag
    }

    // Attack logic specific to the RatAttack state
    void RatAttack_v2_0::AttackLogic(EntityID id, float deltaTime)
    {
        if (!hasActed)
        {
            // Trigger attack animation
            // p_data->SetAnimatorTrigger("attack");

            hasActed = true; // Mark as having acted this turn

            // Implement attack logic, such as dealing damage to the cat
            if (catInRange && targetCat)
            {
                // Damage logic here
                // targetCat->GetComponent<CatScript>()->HurtCat();

                // Play attack sound
                // AudioManager::PlaySFX("RatAttackSound");
            }
        }
    }

    // Collider event handlers (simulated from Unity's OnTriggerEnter2D, OnTriggerStay2D, OnTriggerExit2D)
    void RatAttack_v2_0::OnTriggerEnter(EntityID colliderId)
    {
        // Check if collider is a cat and if we're in EXECUTION phase
        if (IsCat(colliderId) && gameStateController->currentState == GameStates_v2_0::EXECUTE)
        {
            catInRange = true;
            targetCat = colliderId; // Assuming you have a way to store the target cat entity ID
        }
    }

    void RatAttack_v2_0::OnTriggerStay(EntityID colliderId)
    {
        // Similar logic to OnTriggerEnter
        OnTriggerEnter(colliderId);
    }

    void RatAttack_v2_0::OnTriggerExit(EntityID colliderId)
    {
        if (IsCat(colliderId))
        {
            catInRange = false;
            targetCat = 0; // Reset target cat entity ID
        }
    }

    // Utility function to check if an entity ID corresponds to a cat
    //bool RatAttack_v2_0::IsCat(EntityID id)
    //{
    //    // Implement logic to determine if the entity is a cat
    //    // Example: Check the entity's tag or component
    //}
} // namespace PE
