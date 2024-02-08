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

    RatIdle_v2_0::RatIdle_v2_0(RatType type)
        : m_type(type), gameStateController(nullptr), p_data(nullptr)
    { }

    void RatIdle_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);                   // Get GSM instance


        m_collisionEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatIdle_v2_0::OnTriggerEnterAndStay, this);
        m_collisionStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatIdle_v2_0::OnTriggerEnterAndStay, this);
        m_collisionExitEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerExit, RatIdle_v2_0::OnTriggerExit, this);

        // ----- Reset -----
        if (m_type == RatType::PATROL)
        {
            InitializePatrolPoints();                                                               // Initialize patrol points if the rat type is PATROL
        }

        if (EntityManager::GetInstance().Has<AnimationComponent>(id))
        {
            EntityManager::GetInstance().Get<AnimationComponent>(id).SetCurrentFrameIndex(0);           // Reset Rat animaition to first frame.
        }

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
        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            return;
        }

        //gameStateController->currentState = GameStates_v2_0::EXECUTE;

        if (gameStateController->currentState == GameStates_v2_0::EXECUTE)
        {
            switch (m_type)
            {
            case RatType::IDLE:
                // Idle logic doing nothing

                //std::cout << "Idle";
                break;
            case RatType::PATROL:
                //std::cout << "Patrolling Update\n";
                PatrolLogic(id, deltaTime);
                break;
            default: break;
            }
        }
        else if(gameStateController->currentState == GameStates_v2_0::PLANNING)
        {
            switch (m_type)
            {
            case RatType::IDLE:
            case RatType::PATROL:
                // Check if any cats have entered or exited
                // the rat's detection range and change state accordingly
                if (StateJustChanged()) 
                { 
                    CheckIfShouldChangeStates(); 
                }
                break;
            default: break;
            }
        }
    }

    void RatIdle_v2_0::PatrolLogic(EntityID id, float deltaTime)
    {
        if (!p_data->shouldPatrol || p_data->patrolPoints.empty()) return;

        const vec2& currentTarget = p_data->patrolPoints[p_data->patrolIndex];
        MoveTowards(id, currentTarget, deltaTime);

        if (HasReachedDestination(id, currentTarget))
        {
            //std::cout << "Reached patrol point index: " << p_data->patrolIndex << std::endl;

            if (p_data->returnToFirstPoint)
            {
                p_data->patrolIndex++;
                if (p_data->patrolIndex >= p_data->patrolPoints.size())
                {
                    p_data->patrolIndex = static_cast<int>(p_data->patrolPoints.size()) - 2;
                    p_data->returnToFirstPoint = false;
                }
            }
            else
            {
                p_data->patrolIndex--;
                if (p_data->patrolIndex < 0)
                {
                    p_data->patrolIndex = 1;
                    p_data->returnToFirstPoint = true;
                }
            }
        }

        //const Transform& ratTransform = EntityManager::GetInstance().Get<Transform>(id);
        //std::cout << "Rat Position: X=" << ratTransform.position.x << ", Y=" << ratTransform.position.y << std::endl;
    }


    void RatIdle_v2_0::StateCleanUp()
    {
        // Unsubscribe from events
        REMOVE_KEY_COLLISION_LISTENER(m_collisionEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionStayEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_collisionExitEventListener);
    }


    void RatIdle_v2_0::MoveTowards(EntityID id, const vec2& target, float deltaTime)
    {
        Transform& ratTransform = EntityManager::GetInstance().Get<Transform>(id);
        vec2 direction = target - ratTransform.position;
        float distanceToMove = p_data->movementSpeed * deltaTime;

        if (direction.LengthSquared() > 0.0f)
        {
            vec2 normalizedDirection = direction.GetNormalized();
            float distanceToTarget = direction.Length();

            if (distanceToMove > distanceToTarget)
            {
                ratTransform.position = target;
            }
            else {
                ratTransform.position += normalizedDirection * distanceToMove;
            }
        }
        else
        {
            std::cout << "Direction vector is zero, cannot normalize" << std::endl;
        }
    }

    bool RatIdle_v2_0::HasReachedDestination(EntityID id, const vec2& target)
    {
        Transform& ratTransform = EntityManager::GetInstance().Get<Transform>(id);
        vec2 diff = ratTransform.position - target;

        if (diff.Length() <= 1.0f)
        {
            return true;
        }
        return false;
    }

    void RatIdle_v2_0::InitializePatrolPoints()
    {
        if (p_data->patrolPoints.size() < 2)
        {
            p_data->patrolPoints.clear();
            p_data->patrolPoints.push_back(vec2(0.0f, 0.0f));
            p_data->patrolPoints.push_back(vec2(100.0f, 0.0f));
        }
        p_data->patrolIndex = 0;
    }

    void RatIdle_v2_0::SetPatrolPoints(const std::vector<vec2>& points)
    {
        p_data->patrolPoints.clear();
        for (const auto& point : points)
        {
            p_data->patrolPoints.push_back(point);
        }
        p_data->patrolIndex = 0;
    }

    void RatIdle_v2_0::StateExit(EntityID id)
    {
        // disables all the UI from showing up
        RatScript_v2_0::ToggleEntity(p_data->telegraphArrowEntityID, false);
        RatScript_v2_0::ToggleEntity(p_data->redTelegraphEntityID, false);
        RatScript_v2_0::ToggleEntity(p_data->attackTelegraphEntityID, false);

        if (EntityManager::GetInstance().Has<AnimationComponent>(id))
            EntityManager::GetInstance().Get<Graphics::Renderer>(p_data->attackTelegraphEntityID).SetEnabled(false);

        gameStateController = nullptr;
    }


    void RatIdle_v2_0::CheckIfShouldChangeStates()
    {
        // Check if there are any cats in the detection range
        if (!(p_data->catsInDetectionRadius.empty()))
        {
            // Change to aggressive state
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToAttack(p_data->myID);
        }
        // Check if any cats exited the radius during execution
        else if (!(p_data->catsExitedDetectionRadius.empty()))
        {
            // Check for the closest cat
            EntityID closestCat{ RatScript_v2_0::GetCloserTarget(RatScript_v2_0::GetEntityPosition(p_data->myID), p_data->catsExitedDetectionRadius) };

            // Change to hunting state
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->ChangeStateToHunt(p_data->myID, closestCat);
        }
    }


    void RatIdle_v2_0::OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }

        if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
        {
            OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
            // check if entity1 is the rat's detection collider and entity2 is cat
            if ((OTEE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity2);
            }
            // check if entity2 is the rat's detection collider and entity1 is cat
            else if ((OTEE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity1);
            }
        }
        else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
        {
            OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
            // check if entity1 is the rat's detection collider and entity2 is cat
            if ((OTSE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity2) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity2).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity2);
            }
            // check if entity2 is the rat's detection collider and entity1 is cat
            else if ((OTSE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity1) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity1);
            }
        }
    }


    void RatIdle_v2_0::OnTriggerExit(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }

        OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
        // check if entity1 is the rat's detection collider and entity2 is cat
        if ((OTEE.Entity1 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
            EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity2);
        }
        // check if entity2 is the rat's detection collider and entity1 is cat
        else if ((OTEE.Entity2 == p_data->detectionRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
            EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity1);
        }
    }

} // namespace PE
