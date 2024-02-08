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

        /*!***********************************************************************************
         \brief Unsubscribes from the collision events
        *************************************************************************************/
        virtual void StateCleanUp();

        // Logic for Rat idle state
        void PatrolLogic(EntityID id, float deltaTime);
        void MoveTowards(EntityID id, const vec2& target, float deltaTime);
        bool HasReachedDestination(EntityID id, const vec2& target);
        void InitializePatrolPoints();
        void SetPatrolPoints(const std::vector<PE::vec2>& points);

        // Getter for the state name with version
        virtual std::string_view GetName() { return "Idle_v2_0"; }


        // --- COLLISION DETECTION --- // 

        /*!***********************************************************************************
         \brief Checks if any cats entered or executed the rat's detection radius during 
                the last execution phase and decides whether to swap to the attacking or 
                hunting states respectively.
        *************************************************************************************/
        void CheckIfShouldChangeStates();

        /*!***********************************************************************************
         \brief Called when a trigger enter or stay event has occured. If an event has
          occurred between this script's rat's detection collider and a cat, the parent rat
          is notified.

         \param[in,out] r_TE - Trigger event data.
        *************************************************************************************/
        void OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE);

        /*!***********************************************************************************
         \brief Called when a trigger exit event has occured. If an event has occurred
          between this script's rat's detection collider and a cat, the parent rat
          is notified.

         \param[in,out] r_TE - Trigger event data.
        *************************************************************************************/
        void OnTriggerExit(const Event<CollisionEvents>& r_TE);

    private:
        // Idle Planning specific variables
        RatScript_v2_0_Data* p_data;
        RatType m_type;

        GameStateController_v2_0* gameStateController{ nullptr };
        GameStates_v2_0 m_previousGameState; // The game state in the previous frame

        // Event listener IDs 
        int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};

        // ----- PRIVATE METHODS ---- //
    private:
        /*!***********************************************************************************
          \brief Returns true if the current game state is different from the game state
                  in the previous frame, false otherwise.
        *************************************************************************************/
        inline bool StateJustChanged() const
        {
            return m_previousGameState != gameStateController->currentState;
        }
    };


} // namespace PE
