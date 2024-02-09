#pragma once

#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/LogicSystem.h"
#include "ECS/Entity.h"
#include "../StateManager.h"
#include "Events/EventHandler.h"

namespace PE
{
    // ----- RAT ATTACK STATE v2.0 ----- //
    class RatAttack_v2_0 : public State
    {
    public:
        // ----- Constructor ----- //
        RatAttack_v2_0();

        // ----- Destructor ----- //
        virtual ~RatAttack_v2_0() { p_data = nullptr; };

        virtual void StateEnter(EntityID id) override;
        virtual void StateUpdate(EntityID id, float deltaTime) override;
        virtual void StateCleanUp();
        virtual void StateExit(EntityID id) override;

        void RatHitCat(const Event<CollisionEvents>& r_TE);


        // --- COLLISION DETECTION --- // 

        /*!***********************************************************************************
         \brief Called when a trigger enter or stay event has occured. If an event has
          occurred between this script's rat's detection collider or attack collider and a cat, 
          the parent rat is notified.

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

        // Getter for the state name with version
        virtual std::string_view GetName() { return "Attack_v2_0"; }

    private:
        GameStateController_v2_0* gameStateController{ nullptr };

        // Attack state specific variables and data
        RatScript_v2_0_Data* p_data;
        float m_delay{};

        bool attacksoundonce{};
        bool telegraphEnabled{ false };

        // Event listener IDs 
        int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};

        // ----- PRIVATE METHODS ----- //
    private:
        /*!***********************************************************************************
         \brief Checks if any cats entered or executed the rat's detection radius during
                the last execution phase and decides whether to swap to the attacking or
                hunting states respectively.
        *************************************************************************************/
        void CheckIfShouldChangeStates();
    };

} // namespace PE
