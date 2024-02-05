#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"

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
        void AttackLogic(EntityID id, float deltaTime);

        // Collider event handlers from ax unity.
        void OnTriggerEnter(EntityID colliderId);
        void OnTriggerStay(EntityID colliderId);
        void OnTriggerExit(EntityID colliderId);

        // Utility function to check if an entity ID corresponds to a cat
        bool IsCat(EntityID id);

        // Getter for the state name with version
        virtual std::string_view GetName() { return "Attack_v2_0"; }

    private:
        GameStateController_v2_0* gameStateController{ nullptr };

        // Attack state specific variables and data
        RatScript_v2_0_Data* p_data;

        bool catInRange{ false };                                   // Flag to indicate if a cat is in attack range
        bool hasActed{ false };                                     // Flag to indicate if the rat has already acted this turn/cycle
        EntityID targetCat{ 0 };                                    // Entity ID of the target cat
    };
} // namespace PE
