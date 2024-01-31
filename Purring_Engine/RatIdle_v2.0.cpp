#include "prpch.h"
#include "RatIdle_v2.0.h"

#include "Physics/CollisionManager.h"


namespace PE
{
    // ---------- RAT IDLE PLAN STATE v2.0 ---------- //

    RatIdlePLAN_v2_0::RatIdlePLAN_v2_0() { }

    RatIdlePLAN_v2_0::~RatIdlePLAN_v2_0() { }

    void RatIdlePLAN_v2_0::StateEnter_v2_0(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript, id);

        // Reset values for the planning state
        EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0); // Reset animation frame index
    }

    void RatIdlePLAN_v2_0::StateUpdate_v2_0(EntityID id, float deltaTime)
    {
        // Update logic for the Idle Planning state
    }

    void RatIdlePLAN_v2_0::StateExit_v2_0(EntityID id)
    {
        // Cleanup logic for exiting the Idle Planning state
    }

    std::string_view RatIdlePLAN_v2_0::GetName_v2_0() const {
        return "IdlePLAN_v2_0";
    }



    // ---------- RAT IDLE EXECUTE STATE v2.0 ---------- //

    RatIdleEXECUTE_v2_0::RatIdleEXECUTE_v2_0() { }

    RatIdleEXECUTE_v2_0::~RatIdleEXECUTE_v2_0() { }

    void RatIdleEXECUTE_v2_0::StateEnter_v2_0(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript, id);

        //  Reset values for the execution state
        EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0); // Reset animation frame index
    }

    void RatIdleEXECUTE_v2_0::StateUpdate_v2_0(EntityID id, float deltaTime)
    {
        // Update logic for the Idle Execution state
    }

    void RatIdleEXECUTE_v2_0::StateExit_v2_0(EntityID id)
    {
        // Cleanup logic for exiting the Idle Execution state
    }

    std::string_view RatIdleEXECUTE_v2_0::GetName_v2_0() const 
    {
        return "IdleEXECUTE_v2_0";
    }

} // namespace PE
