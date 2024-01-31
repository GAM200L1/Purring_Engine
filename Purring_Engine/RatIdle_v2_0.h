#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
//#include "Logic/RatScript.h"  // @KRYSTAL depends if you still using this RatScript file or a new file for the base class you working on.
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"

//#include "Logic/StateManager.h"
namespace PE {

    // ----- RAT IDLE PLANNING STATE v2.0 ----- //
    class RatIdlePLAN_v2_0 : public State
    {
    public:
        RatIdlePLAN_v2_0();

        // ----- Destructor ----- //
        virtual ~RatIdlePLAN_v2_0() { p_data = nullptr; }

        // Override functions from the base State class for Planning state
        virtual void StateEnter_v2_0(EntityID id) override;

        virtual void StateUpdate_v2_0(EntityID id, float deltaTime) override;

        virtual void StateExit_v2_0(EntityID id) override;

        // Getter for the state name with version
        virtual std::string_view GetName_v2_0() const override { return "IdlePLAN_v2_0"; }

    private:
        // Idle Planning specific variables
        RatScript_v2_0_Data* p_data;
    };



    // ----- RAT IDLE EXECUTE STATE v2.0 ----- //
    class RatIdleEXECUTE_v2_0 : public State
    {
    public:
        RatIdleEXECUTE_v2_0();

        // ----- Destructor ----- //
        virtual ~RatIdleEXECUTE_v2_0() { p_data = nullptr; }

        // Override functions from the base State class for Execution state
        virtual void StateEnter_v2_0(EntityID id) override;

        virtual void StateUpdate_v2_0(EntityID id, float deltaTime) override;

        virtual void StateExit_v2_0(EntityID id) override;

        // Getter for the state name with version
        virtual std::string_view GetName_v2_0() const override { return "IdleEXECUTE_v2_0"; }

    private:
        // Idle Execution specific variables
        RatScript_v2_0_Data* p_data;
    };

} // namespace PE
