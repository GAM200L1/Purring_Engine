/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RatScript.h
 \date     20-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains declarations for functions used for a rats mechanics

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Script.h"
#include "StateManager.h"
#include "ECS/Entity.h"
#include "Math/MathCustom.h"
#include "Events/EventHandler.h"

namespace PE
{
	struct RatScriptData
	{
		// reference entities
		EntityID ratID{ 0 };
		EntityID psudoRatID{ 0 };
		EntityID mainCatID{ 8 }; // needs manual setting
		
		// rat stats
		int health{ 5 };
		int maxHealth{ 10 }; // needs manual setting

		// movement variables
		float movementSpeed{ 200.f }; // speed of rat needs manual setting
		float distanceFromPlayer{ 0.f };

		// attack entities and variables
		EntityID arrowTelegraphID{ 0 };
		EntityID attackTelegraphID{ 0 };
		EntityID detectionTelegraphID{ 0 };
		float detectionRadius{ 4.f }; // radius of the detection UI needs manual setting
		float attackDiameter{ 64.f }; // radius of the attack
		float attackDuration{ 0.5f }; // how long the attack is active needs manual setting
		int collisionDamage{ 1 }; // damage when touching the rat needs manual setting
		int attackDamage{ 1 }; // damage when properly attacked by the rat needs manual setting

		// state management
		StateMachine* p_stateManager;
		bool shouldChangeState{};
		bool delaySet{ false };
		float timeBeforeChangingState{ 0.f };
		bool finishedExecution{ false }; // bool to check if rat has finished its movemen and attack executions
	};

	class RatScript : public Script
	{
	public:
		// ----- Public Variables ------ //

		std::map<EntityID, RatScriptData> m_scriptData;

	public:

		// ----- Destructor ----- //
		virtual ~RatScript();

		// ----- Public Functions ----- //
		virtual void Init(EntityID id);

		virtual void Update(EntityID id, float deltaTime);

		virtual void Destroy(EntityID id) {}

		virtual void OnAttach(EntityID id);

		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed after the delay passed in.

		 \param[in] id EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void TriggerStateChange(EntityID id, float const stateChangeDelay = 0.f);

		/*!***********************************************************************************
		 \brief Changes the state if the flag has been set and the delay is zero and below.

		 \param[in] id EntityID of the entity to change the state of.
		 \param[in] deltaTime Time in seconds that have passed since the last frame.

		 \return true - True if the state should change.
		 \return false - False if the state should NOT change.
		*************************************************************************************/
		bool CheckShouldStateChange(EntityID id, float const deltaTime);

		/*!***********************************************************************************
		 \brief Helper function to en/disables an entity.

		 \param[in] id EntityID of the entity to en/disable.
		 \param[in] setToActive Whether this entity should be set to active or inactive.
		*************************************************************************************/
		static void ToggleEntity(EntityID id, bool setToActive);

		/*!***********************************************************************************
		 \brief Adjusts the position of the transform to the value passed in.

		 \param[in] transformId ID of the entity to update the transform of.
		 \param[in] r_position Position to set the transform to.
		*************************************************************************************/
		static void PositionEntity(EntityID const transformId, vec2 const& r_position);

		/*!***********************************************************************************
		 \brief Adjusts the scale of the transform to the value passed in.

		 \param[in] transformId ID of the entity to update the transform of.
		 \param[in] width Width to set the transform to.
		 \param[in] height Height to set the transform to.
		*************************************************************************************/
		static void ScaleEntity(EntityID const transformId, float const width, float const height);


		// ----- Getters/RTTR ----- //

		/*!***********************************************************************************
		 \brief Returns the position of the transform of the entity passed in.

		 \param[in] transformId ID of the entity to retrieve the position of.
		*************************************************************************************/
		static vec2 GetEntityPosition(EntityID const transformId);

		/*!***********************************************************************************
		 \brief Returns the scale of the transform of the entity passed in.

		 \param[in] transformId ID of the entity to retrieve the scale of.
		*************************************************************************************/
		static vec2 GetEntityScale(EntityID const transformId);


		std::map<EntityID, RatScriptData>& GetScriptData() { return m_scriptData; }

		rttr::instance GetScriptData(EntityID id) { return rttr::instance(m_scriptData.at(id)); }

	private:
		// ----- Private Functions ----- //
		void CreateAttackTelegraphs(EntityID id);
	};

	class RatIDLE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~RatIDLE() { p_data = nullptr; }

		// ----- Public Functions ----- //
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id) override;

		// ----- Getter ----- //
		virtual std::string_view GetName() { return "IDLE"; }

	private:
		// ----- Private Variables ----- //
		RatScriptData* p_data;
	};


	class RatMovementEXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~RatMovementEXECUTE() { p_data = nullptr; }

		// ----- Public Functions ----- //
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id) override;

		void RatHitCat(const Event<CollisionEvents>& r_TE);

		// ----- Getter ----- //
		virtual std::string_view GetName() { return "MovementEXECUTE"; }

	private:
		// ----- Private Variables ----- //
		RatScriptData* p_data;
		int m_collisionEventListener{};
	};



	class RatAttackEXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~RatAttackEXECUTE() { p_data = nullptr; }

		// ----- Public Functions ----- //
		virtual void StateEnter(EntityID id) override;

		virtual void StateUpdate(EntityID id, float deltaTime) override;

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id) override;

		void RatHitCat(const Event<CollisionEvents>& r_TE);

		// ----- Getter ----- //
		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		// ----- Private Variables ----- //
		RatScriptData* p_data;
		int m_collisionEventListener{};
	};

}