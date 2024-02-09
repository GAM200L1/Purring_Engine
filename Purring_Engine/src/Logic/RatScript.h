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
#include "GameStateController_v2_0.h"

namespace PE
{
	enum class EnumOldRatType
	{
		GUTTER,
		BRAWLER,
		SNIPER,
		HERALD
	};

	struct RatScriptData
	{
		// reference entities
		EntityID ratID{ 0 }; // id of the rat with this data
		EntityID psudoRatID{ 0 }; // id of an additional invisible object with transform for rotating the arrow telegraph
		EntityID mainCatID{ 3 }; // needs manual setting
		
		// rat stats
		int health{ 1 }; // health of the rat, needs manual setting

		// movement variables
		float movementSpeed{ 200.f }; // speed of rat needs manual setting
		float distanceFromPlayer{ 0.f }; // stores distance of rat from player cat to determine movement

		// attack entities and variables
		EntityID arrowTelegraphID{ 0 }; // id of arrow telegraph
		EntityID attackTelegraphID{ 0 }; // id of cross attack telegraph
		EntityID detectionTelegraphID{ 0 }; // id of red detection telegraph
		float detectionRadius{ 4.f }; // radius of the detection UI needs manual setting
		float attackDiameter{ 64.f * 2.f }; // radius of the attack
		float attackDuration{ 0.5f }; // how long the attack is active needs manual setting
		int collisionDamage{ 1 }; // damage when touching the rat needs manual setting
		int attackDamage{ 1 }; // damage when properly attacked by the rat needs manual setting
		float attackDelay{ 0.f }; // delay before attacking cat, needs manual setting
		vec2 directionToTarget{ 0.f, 0.f }; // stores the normalized vector pointing at player cat
		bool attacking{ false }; // a check for whether the rat is close enough to the player to attack
		bool hitCat{ false }; // a check for whether the rat has hit the player once in the entire execution sequence

		// state management
		StateMachine* p_stateManager; // pointer to rat's state machine
		bool shouldChangeState{}; // check to change state
		bool delaySet{ false }; // check if there is state change delay
		float timeBeforeChangingState{ 0.f }; // delay before state change
		bool finishedExecution{ false }; // bool to check if rat has finished its movemen and attack executions

		std::map<std::string, std::string> animationStates;

		std::vector<EntityID> targetCats;

		EnumOldRatType ratType{ EnumOldRatType::GUTTER };
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
		/*!***********************************************************************************
		 \brief Initializes the rat's variables, including creating the attack telegraphs and
		 		creating its state machine and starting it at the Rat's IDLE if one has not
				been made
		 
		 \param[in] id - EntityID of the rat to initialize
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Called every frame. Updates rat's current state and animations depending on state
		 
		 \param[in] id - EntityID of the rat to update
		 \param[in] deltaTime - difference in time between the current frame and the last frame
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Does nothing
		 
		 \param[in] id - EntityID of rat to destroy
		*************************************************************************************/
		virtual void Destroy(EntityID id) {}

		/*!***********************************************************************************
		 \brief Called when first attached to an entity. Used to add components and reset to 
		 		default values
		 
		 \param[in] id - EntityID of entity to attach to 
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Called when removed from an entity. Used to clean up the script
		 
		 \param[in] id - EntityID of the entity in this instance of the script is attached to
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Called when the entity with this script has been hit. Decrements variable 'health'
		 
		 \param[in] id - EntityID of rat that has been hit
		 \param[in] damageTaken - amount of health loss
		*************************************************************************************/
		void LoseHP(EntityID id, int damageTaken);

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

		/*!***********************************************************************************
		 \brief Helper function checks if the rat has hit a cat based on triggerenter and trigger stay events
		 
		 \param[in] id - Entity of rat involved in collision event
		 \param[in] r_TE - const reference to the collision event received
		*************************************************************************************/
		void RatHitCat(EntityID id, const Event<CollisionEvents>& r_TE);

		/*!***********************************************************************************
		 \brief Helper function for RatHitCat. Checks if the collided entities is main cat or
		 		follower cat and rat's attack or body. Reduces cat HP accordingly
		 
		 \param[in] mainCat - EntityID of the Player cat
		 \param[in] collidedCat - EntityID of the cat rat has collided with (following or main cat)
		 \param[in] damagingID - EntityID of attacking entity (rat body or rat's attack)
		 \param[in] rat - EntityID of rat performing the attack
		*************************************************************************************/
		void CheckFollowOrMain(EntityID mainCat, EntityID collidedCat, EntityID damagingID, EntityID rat);

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

		/*!***********************************************************************************
		 \brief Get the Script Data object
		 
		 \return std::map<EntityID, RatScriptData>& - m_scriptData object
		*************************************************************************************/
		std::map<EntityID, RatScriptData>& GetScriptData() { return m_scriptData; }

		/*!***********************************************************************************
		 \brief Get the Script Data object
		 
		 \param[in] id - EntityID of entity with this script
		 \return rttr::instance - script data of object with 'id'
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id) { return rttr::instance(m_scriptData.at(id)); }

	private:
		// ----- Private Variables ----- //
		GameStateController_v2_0* p_gsc;

		// ----- Private Functions ----- //
		/*!***********************************************************************************
		 \brief Create entities for the attack telegraphs for the rat from prefabs. Adjusts 
		 		transforms and other components based on the rat's transform
		 
		 \param[in] id - EntityID of the rat to create telegraphs for
		*************************************************************************************/
		void CreateAttackTelegraphs(EntityID id);
	};

	class RatIDLE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~RatIDLE() { p_data = nullptr; }

		// ----- Public Functions ----- //
		/*!***********************************************************************************
		 \brief Displays detection telegraph. Checks if main cat is in the detection radius.
		 		If it is, adjusts transform of arrow and attack telegraphs to show rat is going
				to attack. Resets the animation loop it uses
		 
		 \param[in] id - EntityID of rat
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;

		/*!***********************************************************************************
		 \brief Does nothing
		 
		 \param[in] id - EntityID of rat
		 \param[in] deltaTime - difference in time between current and previous frame
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;

		/*!***********************************************************************************
		 \brief Does nothing
		 
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
		 \brief Deactivates all telegraphs so they do not cause collision and are not rendered
		 		yet
		 
		 \param[in] id - EntityID of rat
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;

		// ----- Getter ----- //
		/*!***********************************************************************************
		 \brief Get the name of the state
		 
		 \return std::string_view - name of the state
		*************************************************************************************/
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
		/*!***********************************************************************************
		 \brief Subscribes to collision events to check if cat collides rat with this script
		 		during movement execution. Checks if rat will make an attack. Resets the
				animation loop it uses.
		 
		 \param[in] id - EntityID of the rat
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;

		/*!***********************************************************************************
		 \brief Updates the rat's position based on the direction and distance it was planned
		 		to travel. Triggers state change to execute attack once movement is complete
		 
		 \param[in] id - EntityID of rat
		 \param[in] deltaTime - difference in time between current and last frame. used to 
		 						calculate position to set rat to
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		 
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
		 \brief Resets distance from player
		 
		 \param[in] id - EntityID of rat
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;

		/*!***********************************************************************************
		 \brief Callback function for collision event. Checks if rat collided with any cats in cat chain
		 
		 \param[in] r_TE const reference to the collision event received
		*************************************************************************************/
		void RatHitCat(const Event<CollisionEvents>& r_TE);

		// ----- Getter ----- //
		/*!***********************************************************************************
		 \brief Get the name of the state
		 
		 \return std::string_view - name of the state
		*************************************************************************************/
		virtual std::string_view GetName() { return "MovementEXECUTE"; }

	private:
		// ----- Private Variables ----- //
		RatScriptData* p_data;
		int m_collisionEventListener{};
		int m_collisionStayEventListener{};
	};



	class RatAttackEXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~RatAttackEXECUTE() { p_data = nullptr; }

		// ----- Public Functions ----- //
		/*!***********************************************************************************
		 \brief Subscribes to collision events to check if rat or rat's attack collides with
		 		any cat in player's cat chain. Resets the animation used for this state.
		 
		 \param[in] id - EntityID of rat
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;

		/*!***********************************************************************************
		 \brief Toggles on and off the collider representing the rat's attack for it to 
		 		perform the attack
		 
		 \param[in] id - EntityID of rat
		 \param[in] deltaTime - Time difference between previous and current frame
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		 
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
		 \brief Toggles off the collider for rat's attack and re-enable rendering
		 
		 \param[in] id - EntityID of the rat
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;
		
		/*!***********************************************************************************
		 \brief Callback function for collision event. Checks if rat or rat's attack collided
		 		with any cats in cat chain
		 
		 \param[in] r_TE const reference to the collision event received
		*************************************************************************************/
		void RatHitCat(const Event<CollisionEvents>& r_TE);

		// ----- Getter ----- //
		/*!***********************************************************************************
		 \brief Gets the name of the state
		 
		 \return std::string_view - name of the state
		*************************************************************************************/
		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		// ----- Private Variables ----- //
		RatScriptData* p_data;
		int m_collisionEventListener{};
		int m_collisionStayEventListener{};
		float m_delay{};

		bool attacksoundonce{};
	};

}