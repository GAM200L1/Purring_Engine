#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"

namespace PE
{
    enum class RatType
    {
        IDLE,
        PATROL
    };

    // ----- RAT IDLE STATE v2.0 ----- //
    class RatIdle_v2_0 : public State
    {
    public:
        // ----- Constructor ----- //
        RatIdle_v2_0(RatType type);

        // ----- Destructor ----- //
        virtual ~RatIdle_v2_0() { p_data = nullptr; }

        virtual void StateEnter(EntityID id) override;
        virtual void StateUpdate(EntityID id, float deltaTime) override;
        virtual void StateExit(EntityID id) override;

        // Logic for Rat idle state
        void PatrolLogic(EntityID id, float deltaTime);
        void MoveTowards(EntityID id, const vec2& target, float deltaTime);
        bool HasReachedDestination(EntityID id, const vec2& target);
        void InitializePatrolPoints();
        void SetPatrolPoints(const std::vector<PE::vec2>& points);

        // Getter for the state name with version
        virtual std::string_view GetName() { return "Idle_v2_0"; }

    private:
        // Idle Planning specific variables
        RatScript_v2_0_Data* p_data;
        RatType m_type;

        GameStateController_v2_0* gameStateController{ nullptr };
    };


} // namespace PE
