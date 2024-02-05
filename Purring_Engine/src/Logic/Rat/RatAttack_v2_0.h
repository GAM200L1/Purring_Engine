#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"
#include "Events/EventHandler.h"

namespace PE
{
    // ----- RAT ATTACK STATE v2.0 ----- //
    class RatAttack_v2_0 : public State
    {
    public:
        // ----- Constructor ----- //
        RatAttack_v2_0();

        // ----- Destructor ----- //
        virtual ~RatAttack_v2_0() { p_data = nullptr; };

        virtual void StateEnter(EntityID id) override;
        virtual void StateUpdate(EntityID id, float deltaTime) override;
        virtual void StateExit(EntityID id) override;

        // Attack logic specific to the RatAttack state
        void Attack(EntityID id);

        void OnTriggerEnter(const Event<CollisionEvents>& event);  // Handle collision enter events
        void OnTriggerExit(const Event<CollisionEvents>& event);   // Handle collision exit events
        bool IsCat(EntityID id);

        // Getter for the state name with version
        virtual std::string_view GetName() { return "Attack_v2_0"; }

    private:
        GameStateController_v2_0* gameStateController{ nullptr };

        // Attack state specific variables and data
        RatScript_v2_0_Data* p_data;

        bool hasAttacked{ false };
        int m_collisionEnterListener; // Listener ID for collision enter events
        int m_collisionExitListener;  // Listener ID for collision exit events

    };

} // namespace PE
