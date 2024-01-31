#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"

//#include "Logic/StateManager.h"
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

        virtual void StateEnter_v2_0(EntityID id) override;

        virtual void StateUpdate_v2_0(EntityID id, float deltaTime) override;

        virtual void StateExit_v2_0(EntityID id) override;

        // Getter for the state name with version
        virtual std::string_view GetName_v2_0() const override { return "Idle_v2_0"; }

    private:
        // Idle Planning specific variables
        RatScript_v2_0_Data* p_data;
        RatType m_type;
    };


} // namespace PE
