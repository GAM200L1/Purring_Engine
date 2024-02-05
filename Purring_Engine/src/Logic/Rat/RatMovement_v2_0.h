#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"

namespace PE
{
    // ----- RAT MOVEMENT STATE v2.0 ----- //
    class RatMovement_v2_0 : public State
    {
    public:
        // ----- Constructor ----- //
        RatMovement_v2_0();
        // ----- Destructor ----- //
        virtual ~RatMovement_v2_0() { p_data = nullptr; };

        // Override State class functions
        virtual void StateEnter(EntityID id) override;
        virtual void StateUpdate(EntityID id, float deltaTime) override;
        virtual void StateCleanUp();
        virtual void StateExit(EntityID id) override;

        virtual std::string_view GetName() { return "Movement_v2_0"; }
        bool IsWithinAttackRange(EntityID ratID, const vec2& catPosition);

        void RatHitCat(const Event<CollisionEvents>& r_TE);

    private:
        void CalculateMovement(EntityID id, float deltaTime);
        bool CheckDestinationReached(const vec2& newPosition, const vec2& targetPosition);

        GameStateController_v2_0* gameStateController{ nullptr };

        // Movement state specific variables and data
        RatScript_v2_0_Data* p_data{ nullptr };

        float minDistanceToTarget{ 1.0f };

        int m_collisionEventListener{};
        int m_collisionStayEventListener{};

    };
}
