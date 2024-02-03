#include "prpch.h"
#include "Logic/Rat/RatIdle_v2_0.h"

#include "Logic/Rat/RatScript_v2_0.h"
//
#include "Physics/CollisionManager.h"
#include "Animation/Animation.h"
#include "Logic/GameStateController_v2_0.h"
#include "Math/MathCustom.h"
#include "Math/Transform.h"
namespace PE
{
    // ---------- RAT IDLE STATE v2.0 ---------- //

    RatIdle_v2_0::RatIdle_v2_0(RatType type) : m_type(type) { }

    void RatIdle_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);

        // ----- Reset -----
        if (m_type == RatType::PATROL)
        {
            InitializePatrolPoints();                                                               // Initialize patrol points if the rat type is PATROL
        }
        EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);           // Reset Rat animaition to first frame.

        // Position the ratTelegraph entity where the said rat is.
        RatScript_v2_0::PositionEntity(p_data->ratTelegraphID, EntityManager::GetInstance().Get<PE::Transform>(id).position);

        // Make the detection radius visual indicator visible
        RatScript_v2_0::ToggleEntity(p_data->redTelegraphEntityID, true);

        // This retrieve the rat's and the main cat's transform components (for position, scale, rotation)
        Transform const& ratObject_v2_0 = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
        Transform const& catObject_v2_0 = PE::EntityManager::GetInstance().Get<PE::Transform>(p_data->mainCatID);

        // Calculate and store the absolute scales of the rat and cat (ensuring positive values)
        PE::vec2 absRatScale_v2_0 = PE::vec2{ abs(ratObject_v2_0.width), abs(ratObject_v2_0.height) };
        PE::vec2 absCatScale_v2_0 = PE::vec2{ abs(catObject_v2_0.width), abs(catObject_v2_0.height) };

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
            PE::vec2 newScale{ RatScript_v2_0::GetEntityScale(id) };
            newScale.x = std::abs(newScale.x) * (((catObject_v2_0.position - ratObject_v2_0.position).Dot(vec2{ 1.f, 0.f }) >= 0.f) ? 1.f : -1.f);
            RatScript_v2_0::ScaleEntity(id, newScale.x, newScale.y);

            // Position and display the attack indicator at the cat's location
            RatScript_v2_0::PositionEntity(p_data->attackTelegraphEntityID, catObject_v2_0.position);
            RatScript_v2_0::ToggleEntity(p_data->attackTelegraphEntityID, true);
        }
        else
        {
            // If the cat is not within the detection radius, no action is needed
            p_data->ratPlayerDistance = 0.f;
        }
    }

    void RatIdle_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        // Access GameStateController_v2_0 instance
        GameStateController_v2_0* gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);

        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;
        }

        switch (m_type)
        {
        case RatType::IDLE:
            // Idle logic doing nothing
            break;
        case RatType::PATROL:
            // Patrol behavior: Execute during the EXECUTE phase
            if (gameStateController->currentState == GameStates_v2_0::EXECUTE)
            {
                PatrolLogic(id, deltaTime);
            }
            break;
        }


    }

    void RatIdle_v2_0::PatrolLogic(EntityID id, float deltaTime)
    {
        if (p_data->patrolPoints.empty()) return; // Ensure there are patrol points defined

        vec3& currentTarget = p_data->patrolPoints[p_data->patrolIndex];
        MoveTowards(id, currentTarget, deltaTime);

        if (HasReachedDestination(id, currentTarget))
        {
            // Logic to switch patrol points based on current index and patrol points size
            if (p_data->patrolPoints.size() == 3)
            {
                switch (p_data->patrolIndex)
                {
                case 0:
                    p_data->patrolIndex += 1;
                    break;
                case 1:
                    p_data->patrolIndex = (p_data->returnToFirstPoint) ? 0 : 2;
                    p_data->returnToFirstPoint = !p_data->returnToFirstPoint;
                    break;
                case 2:
                    p_data->patrolIndex -= 1;
                    p_data->returnToFirstPoint = true;
                    break;
                }
            }
            else if (p_data->patrolPoints.size() == 2)
            {
                // Toggle between the two points
                p_data->patrolIndex = (p_data->patrolIndex == 0) ? 1 : 0;
            }
        }
    }

    void RatIdle_v2_0::MoveTowards(EntityID id, const vec3& target, float deltaTime)
    {
        Transform& ratTransform = EntityManager::GetInstance().Get<Transform>(id);
        PE::vec2 target2D(target.x, target.y);
        PE::vec2 direction = target2D - ratTransform.position;
        PE::vec2 normalizedDirection = direction.GetNormalized();
        ratTransform.position += normalizedDirection * p_data->movementSpeed * deltaTime;
    }

    bool RatIdle_v2_0::HasReachedDestination(EntityID id, const vec3& target)
    {
        Transform& ratTransform = EntityManager::GetInstance().Get<Transform>(id);
        PE::vec2 target2D(target.x, target.y);
        PE::vec2 diff = ratTransform.position - target2D;
        return diff.Length() <= 0.1f;
    }


    void RatIdle_v2_0::InitializePatrolPoints()
    {
        p_data->patrolPoints.clear();                                       // clear existing points.

        p_data->patrolPoints.push_back(vec3(-3.0f, -1.56f, 0.0f));          // Patrol Point A
        p_data->patrolPoints.push_back(vec3(0.0f, 0.0f, 0.0f));             // Patrol Point B

        p_data->patrolIndex = 0;                                // Reset patrol index

        // in the future if more than 2 points can use this.
        //p_data->returnToFirstPoint = false;
    }

    void RatIdle_v2_0::StateExit(EntityID id)
    {
        // disables all the UI from showing up
        RatScript_v2_0::ToggleEntity(p_data->telegraphArrowEntityID, false);
        RatScript_v2_0::ToggleEntity(p_data->redTelegraphEntityID, false);
        RatScript_v2_0::ToggleEntity(p_data->attackTelegraphEntityID, false);
        EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->attackTelegraphEntityID).SetEnabled(false);
    }


} // namespace PE
