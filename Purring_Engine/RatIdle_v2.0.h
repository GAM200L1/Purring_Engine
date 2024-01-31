#pragma once
#include "Logic/RatScript.h"  // @KRYSTAL depends if you still using this RatScript file or a new file for the base class you working on.

//#include "Logic/StateManager.h"

namespace PE
{

    // ----- RAT IDLE PLANNING STATE ----- //
    class RatIdlePLAN_v2_0 : public State
    {
    public:
        RatIdlePLAN_v2_0();

        // ----- Destructor ----- //
        virtual ~RatIdlePLAN_v2_0() { p_data = nullptr; }

        virtual void StateEnter_v2_0(EntityID id) override;
        virtual void StateUpdate_v2_0(EntityID id, float deltaTime) override;
        virtual void StateExit_v2_0(EntityID id) override;

        // ----- Getter ----- //
        virtual std::string_view GetName_v2_0() const override { return "IdlePLAN"; }

    private:
        RatScriptData* p_data;

    };

    // ----- RAT IDLE EXECUTE STATE ----- //
    class RatIdleEXECUTE_v2_0 : public State
    {
    public:
        RatIdleEXECUTE_v2_0();

        // ----- Destructor ----- //
        virtual ~RatIdleEXECUTE_v2_0() { p_data = nullptr; }

        virtual void StateEnter_v2_0(EntityID id) override;
        virtual void StateEnter_v2_0(EntityID id, float deltaTime) override;
        virtual void StateEnter_v2_0(EntityID id) override;

        // ----- Getter ----- //
        virtual std::string_view GetName_v2_0() const override { return "IdleEXECUTE"; }

    private:
        RatScriptData* p_data;
    };

} // namespace PE
