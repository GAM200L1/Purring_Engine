#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatAttack_v2_0.h"
#include "Events/EventHandler.h"

namespace PE
{
    // Constructor
    RatAttack_v2_0::RatAttack_v2_0() : gameStateController(nullptr), p_data(nullptr), hasAttacked(false) {}


    void RatAttack_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);       // Get GSM instance
        hasAttacked = false;

        m_collisionEnterListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatAttack_v2_0::OnTriggerEnter, this);
        m_collisionExitListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatAttack_v2_0::OnTriggerExit, this);
        std::cout << "RatAttack_v2_0::StateEnter - Rat ID: " << id << " has entered the attack state." << std::endl;
    }

    void RatAttack_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;                                                                     // Pause state, do nothing
        }

        if (!hasAttacked)
        {
            Attack(id);
            hasAttacked = true;  // Ensure attack logic runs once per state entry

            std::cout << "RatAttack_v2_0::StateUpdate - Rat ID: " << id << " performed an attack." << std::endl;
        }
    }

    void RatAttack_v2_0::StateExit(EntityID id)
    {
        // Cleanup attack-specific data here
        gameStateController = nullptr;
        std::cout << "RatAttack_v2_0::StateExit - Rat ID: " << id << " is exiting the attack state." << std::endl;

        m_collisionEnterListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, &RatAttack_v2_0::OnTriggerEnter, this);
        m_collisionExitListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, &RatAttack_v2_0::OnTriggerExit, this);

    }

    void RatAttack_v2_0::OnTriggerEnter(const Event<CollisionEvents>& event)
    {
        const auto& enterEvent = static_cast<const OnTriggerEnterEvent&>(event);

        EntityID colliderId = enterEvent.OtherEntity;

        // Check if colliderId is a cat and in range
        if (IsCat(colliderId))
        {
            // Perform attack logic here
            Attack(p_data->myID);
        }
    }

    void RatAttack_v2_0::OnTriggerExit(const Event<CollisionEvents>& event)
    {
        const auto& exitEvent = static_cast<const OnTriggerExitEvent&>(event);

        EntityID colliderId = exitEvent.OtherEntity;

        // Handle cat leaving attack range
        if (IsCat(colliderId))
        {
            // Logic for cat exiting attack range
        }
    }

    bool RatAttack_v2_0::IsCat(EntityID id)
    {
        // Implement logic to determine if the entity is a cat
        // Example: Check the entity's tag or component
        return EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Cat") != std::string::npos;
    }

    void RatAttack_v2_0::Attack(EntityID id)
    {
        // Attack logic here
    }

} // namespace PE
