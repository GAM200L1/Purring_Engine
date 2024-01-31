#include "prpch.h"
#include "RatIdle_v2_0.h"
#include "Physics/CollisionManager.h" // Ensure this path is correct
#include "Animation/Animation.h"
#include "Logic/GameStateController_v2_0.h"
#include "Logic/Rat/RatScript_v2_0.h"
#include "Math/Transform.h"
#include "Math/MathCustom.h"

namespace PE
{
    // ---------- RAT IDLE PLAN STATE v2.0 ---------- //

    RatIdlePLAN_v2_0::RatIdlePLAN_v2_0() { }

    RatIdlePLAN_v2_0::~RatIdlePLAN_v2_0() { }

    void RatIdlePLAN_v2_0::StateEnter_v2_0(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);

        // Reset Rat animaition to its first frame.
        EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);

        // Position the ratTelegraph entity where the said rat is.
        RatScript_v2_0::PositionEntity(p_data->ratTelegraphID, EntityManager::GetInstance().Get<PE::Transform>(id).position);

        // Make the detection radius visual indicator visible
        RatScript_v2_0::ToggleEntity(p_data->redTelegraphEntityID, true);

        // This retrieve the rat's and the main cat's transform components (for position, scale, rotation)
        Transform const& ratObject_v2_0 = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
        Transform const& catObject_v2_0 = PE::EntityManager::GetInstance().Get<PE::Transform>(p_data->mainCatID);

        // Calculate and store the absolute scales of the rat and cat (ensuring positive values)
        vec2 absRatScale_v2_0 = vec2{ abs(ratObject_v2_0.width), abs(ratObject_v2_0.height) };
        vec2 absCatScale_v2_0 = vec2{ abs(catObject_v2_0.width), abs(catObject_v2_0.height) };

        // Calculate the distance from the rat to the cat, adjusting for their sizes
        p_data->ratPlayerDistance = RatScript_v2_0::GetEntityPosition(id).Distance(catObject_v2_0.position) - (absCatScale_v2_0.x * 0.5f) - (absRatScale_v2_0.x * 0.5f);

        // Calculate the direction vector from the rat to the cat
        p_data->directionFromRatToPlayerCat = RatScript_v2_0::GetEntityPosition(p_data->mainCatID) - RatScript_v2_0::GetEntityPosition(id);

        // Normalize the direction vector to have a length of 1
        p_data->directionFromRatToPlayerCat.Normalize();

        // Check if the cat is within the rat's detection radius and the cat entity is active
        if (p_data->ratPlayerDistance <= ((RatScript_v2_0::GetEntityScale(p_data->redTelegraphEntityID).x * 0.5f) - (absCatScale_v2_0.x * 0.5f) - (absRatScale_v2_0.x * 0.5f)) && EntityManager::GetInstance().Get<EntityDescriptor>(p_data->mainCatID).isActive)
        {
            if (p_data->ratPlayerDistance <= (absCatScale_v2_0.x * 0.5f))
            {
                // If the rat is very close to the cat, consider them in direct contact
                p_data->ratPlayerDistance = 0.f;
            }
            else
            {
                // Configure the arrow indicator to point towards the cat
                EntityManager::GetInstance().Get<Transform>(p_data->telegraphArrowEntityID).relPosition.x = catObject_v2_0.position.Distance(ratObject_v2_0.position) * 0.5f;
                EntityManager::GetInstance().Get<Transform>(p_data->telegraphArrowEntityID).width = p_data->ratPlayerDistance;

                // Calculate and apply the rotation needed to point the arrow towards the cat
                float rotation = atan2(p_data->directionFromRatToPlayerCat.y, p_data->directionFromRatToPlayerCat.x);
                PE::EntityManager::GetInstance().Get<PE::Transform>(p_data->ratTelegraphID).orientation = rotation;

                // Make the arrow indicator visible
                RatScript_v2_0::ToggleEntity(p_data->telegraphArrowEntityID, true);
            }

            // Adjust the rat's scale to ensure it faces the direction of the cat
            vec2 newScale{ RatScript_v2_0::GetEntityScale(id) };
            newScale.x = std::abs(newScale.x) * (((catObject_v2_0.position - ratObject_v2_0.position).Dot(vec2{ 1.f, 0.f }) >= 0.f) ? 1.f : -1.f);
            RatScript_v2_0::ScaleEntity(id, newScale.x, newScale.y);

            // Position and display the attack indicator at the cat's location
            RatScript_v2_0::PositionEntity(p_data->attackTelegraphID, catObject_v2_0.position);
            RatScript_v2_0::ToggleEntity(p_data->attackTelegraphID, true);
        }
        else
        {
            // If the cat is not within the detection radius, no action is needed
            p_data->ratPlayerDistance = 0.f;
        }
    }

    void RatIdlePLAN_v2_0::StateUpdate_v2_0(EntityID id, float deltaTime)
    {
        // Access GameStateController_v2_0 instance
        GameStateController_v2_0* gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;
        }

        // other things need to update for idle
    }

    void RatIdlePLAN_v2_0::StateExit_v2_0(EntityID id)
    {
        RatScript_v2
    }

    std::string_view RatIdlePLAN_v2_0::GetName_v2_0() const
    {
        return "IdlePLAN_v2_0";
    }



    // ---------- RAT IDLE EXECUTE STATE v2.0 ---------- //

    RatIdleEXECUTE_v2_0::RatIdleEXECUTE_v2_0() { }

    RatIdleEXECUTE_v2_0::~RatIdleEXECUTE_v2_0() { }

    void RatIdleEXECUTE_v2_0::StateEnter_v2_0(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript, id);

        // Initialize or reset values for the execution state
        EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0); // Reset animation frame index
    }

    void RatIdleEXECUTE_v2_0::StateUpdate_v2_0(EntityID id, float deltaTime)
    {
        // Access GameStateController_v2_0 instance
        GameStateController_v2_0* gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;
        }
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
