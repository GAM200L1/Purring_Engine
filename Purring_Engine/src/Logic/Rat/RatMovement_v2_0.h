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

        void RatHitCat(const Event<CollisionEvents>& r_TE);

        // ----- COLLISION DETECTION ----- //
        void OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE);


        // ----- PRIVATE MEMBERS ----- //
    private:

        GameStateController_v2_0* gameStateController{ nullptr };
        GameStates_v2_0 m_previousGameState{GameStates_v2_0::PLANNING}; // The game state in the previous frame

        // Movement state specific variables and data
        RatScript_v2_0_Data* p_data{ nullptr };

        float minDistanceToTarget{ 1.0f };

        // Event listeners
        int m_collisionEventListener{};
        int m_collisionStayEventListener{};

        EntityID m_targetId{}; // ID of the cat to target
        bool m_planningRunOnce{}; // True if the planning phase has been run once


        // ----- PRIVATE METHODS ----- //
    private:
        /*!***********************************************************************************
          \brief Returns true if the current game state is different from the game state
                  in the previous frame, false otherwise.
        *************************************************************************************/
        inline bool StateJustChanged() const
        {
            return m_previousGameState != gameStateController->currentState;
        }

        /*!***********************************************************************************
          \brief Sets the animation to be played based on whether the rat is moving or not.
        *************************************************************************************/
        void UpdateAnimation(bool const isRatMoving);

        /*!***********************************************************************************
          \brief Pick the position that the rat should move toward (in a straight line).

          \return Returns a next viable target for the rat.
        *************************************************************************************/
        vec2 PickTargetPosition();
    };
}
