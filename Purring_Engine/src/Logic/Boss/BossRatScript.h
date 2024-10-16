/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatScript.h
\date     24 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Declaration for BossRatScript class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#pragma once
#include "Logic/StateManager.h"
#include "Logic/Script.h"
#include "Logic/GameStateController_v2_0.h"
#include "Logic/Boss/BossRatAttacks/BossRatAttack.h"
#include "Events/EventHandler.h"

namespace PE
{
	enum class BossRatAttacks
	{
		BASH,
		SLAM,
		CHARGE
	};

	enum class BossRatAnimationsEnum
	{
		IDLE,
		WALK,
		WALKFASTER,
		ATTACK1,
		ATTACK2,
		CHARGE,//channeling animation
		HURT,
		DEATH
	};


	struct BossRatScriptData
	{
		~BossRatScriptData()
		{
			delete p_stateManager;
			delete p_currentAttack;
		}

		//other boss stats

		float maxHealth = 14;
		float currenthealth = maxHealth;
		StateMachine* p_stateManager;
		BossRatAttack* p_currentAttack;
		bool finishExecution{true};

		//bash attack variables
		float telegraphRadius; // incase we want change, but not used for now
		float attackDelay{.25f};
		float activationTime{.5f};

		//slam attack variables
		float jumpSpeed{ 1000 };
		float slamSpeed{ 1000 };
		bool isInSky{ false };
		EntityID leftSideSlam; //need to be serialized
		EntityID rightSideSlam;
		EntityID slamTelegraph;
		EntityID leftSideSlamAnimation;
		EntityID rightSideSlamAnimation;
		EntityID slamAreaTelegraph;

		//charge attack variables
		float chargeSpeed{ 1000 };
		bool isCharging{ false };
		float distanceBetweenPools{ 180 };

		//animation variables
		BossRatAnimationsEnum curr_Anim{ BossRatAnimationsEnum::IDLE };
		std::map<std::string, std::string> animationStates;
		
		int collisionEnterEventKey,collisionStayEventKey;

		int currentAttackSet{ 1 };
		int currentAttackInSet{ 3 };
	};


	class BossRatScript : public Script
	{
		// ----- Public Members ----- //
	public:
		std::map<EntityID, BossRatScriptData> m_scriptData;
		EntityID currentBoss{};
		int currentSlamTurnCounter{};
		std::map<EntityID,int> poisonPuddles;
		float executionTimeOutTime{ 3 };
		float executionTimeOutTimer{ executionTimeOutTime };
		BossRatAttacks bossRatAttackSets[3][3]
		{
			{BossRatAttacks::BASH,BossRatAttacks::CHARGE,BossRatAttacks::SLAM},
			{BossRatAttacks::BASH,BossRatAttacks::SLAM,BossRatAttacks::CHARGE},
			{BossRatAttacks::CHARGE,BossRatAttacks::BASH,BossRatAttacks::SLAM}
		};
		// ----- Constructors ----- //
	public:
		/*!***********************************************************************************
		 \brief Does nothing.
		*************************************************************************************/
		virtual ~BossRatScript();


		// ----- Public Functions ----- //
	public:
		/*!***********************************************************************************
		\brief Called when the object is first spawned into the scene

		\param[in,out] id - ID of the script to update
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Called every frame.

		 \param[in,out] id - ID of the instance to update
		 \param[in,out] deltaTime - delta time used to update the state
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Does nothing
		*************************************************************************************/
		virtual void Destroy(EntityID) {}

		/*!***********************************************************************************
		 \brief Called when the script is attached to an entity (NOTE: even within the editor!).
				DO NOT create objects here.

		 \param[in,out] id - ID of Instance of script to update
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Called when the entity is destroyed and/or this script is removed from
				the entity. Delete the script data and statemanager here.

		 \param[in,out] id - ID of instance of script to clear
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \return std::map<EntityID, nameScriptData>& Map of script data.
		*************************************************************************************/
		std::map<EntityID, BossRatScriptData>& GetScriptData();

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id - ID of the script to get the data from
		 \return rttr::instance Instance of the script to get the data from
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);

		/*!***********************************************************************************
		 \brief Cause the Boss Rat King to take damage
		 \param[in] int damage to take
		 \return void
		*************************************************************************************/
		void TakeDamage(int damage);

		/*!***********************************************************************************
		 \brief Find the furthest cat from the boss currently
		 \return EntityID of the furthest cat
		*************************************************************************************/
		EntityID FindFurthestCat();

		/*!***********************************************************************************
		 \brief Find the closest cat from the boss currently
		 \return EntityID of the closest cat
		*************************************************************************************/
		EntityID FindClosestCat();

		/*!***********************************************************************************
		 \brief update the obstacle vectors
		*************************************************************************************/
		void FindAllObstacles();

		/*!***********************************************************************************
		 \brief get the list of all obstacles
		 \return std::vector<EntityID> of all obstacles
		*************************************************************************************/
		std::vector<EntityID> GetAllObstacles();

		/*!***********************************************************************************
		 \brief Play Animation on the Rat King
		 \param[in] BossRatAnimationsEnum AnimationState animation to play
		 \return void
		*************************************************************************************/
		void PlayAnimation(BossRatAnimationsEnum AnimationState);

		/*!***********************************************************************************
		 \brief Play Attack Audio
		*************************************************************************************/
		void PlayAttackAudio();

		/*!***********************************************************************************
		 \brief Play Hurt Audio
		*************************************************************************************/
		void PlayHurtAudio();

		/*!***********************************************************************************
		 \brief Play Death Audio
		*************************************************************************************/
		void PlayDeathAudio();

		/*!***********************************************************************************
		 \brief Play Poison Puddle Audio
		*************************************************************************************/
		void PlayPoisonPuddleAudio();

		/*!***********************************************************************************
		 \brief Play Slam Audio
		*************************************************************************************/
		void PlaySlamShockWaveAudio();

		/*!***********************************************************************************
		 \brief Play Bash Spike Audio
		*************************************************************************************/
		void PlayBashSpikeAudio();

		/*!***********************************************************************************
		 \brief Play Charge Particles
		 \param[in] vec2 pos position to play the particles
		*************************************************************************************/
		void PlayChargeParticles(vec2 pos);

		/*!***********************************************************************************
		 \brief Stop Charge Particles
		*************************************************************************************/
		void StopChargeParticles();

		/*!***********************************************************************************
		 \brief Reset Charge Particles
		 \param[in] EntityID id of the entity to reset the particles
		*************************************************************************************/
		void ResetChargeParticles(EntityID);

		// ----- Private Members ----- //
	private:
		// --- STATE CHANGE --- //

		/*!***********************************************************************************
		 \brief Creates the state manager if it has not been created.
		 \param[in] id EntityID of the entity to create the state manager for.
		*************************************************************************************/
		void CreateCheckStateManager(EntityID id);

		/*!***********************************************************************************
		 \brief Callback function to be subscribed to collision stay events
		 \param[in] r_collisionStay const reference to the collision event received
		*************************************************************************************/
		void OnCollisionStay(const Event<CollisionEvents>& r_collisionStay);
		/*!***********************************************************************************
		 \brief Callback function to be subscribed to collision enter events
		 \param[in] r_collisionEnter const reference to the collision event received
		*************************************************************************************/
		void OnCollisionEnter(const Event<CollisionEvents>& r_collisionEnter);

		/*!***********************************************************************************
		 \brief Check if an object is a obstacle
		 \param[in] id EntityID to check if it is an obstacle
		 \return bool whether an object is an obstacle
		*************************************************************************************/
		bool IsObstacle(EntityID);

		/*!***********************************************************************************
		 \brief			Activate the given object and its childrens

		 \param[in]		The objects to set active
		*************************************************************************************/
		void ActiveObject(EntityID);

		/*!***********************************************************************************
		 \brief			Deactivate the given object and its childrens

		 \param[in]		The objects to set deactivate
		*************************************************************************************/
		void DeactiveObject(EntityID);

	private:
		// ----- Private Variables ----- //
		GameStateController_v2_0* p_gsc = nullptr;
		std::vector<EntityID> m_obstacles;

		float m_deathDelayTimerBeforeOutro{.75f};
		float m_deathDelayTimeBeforeOutro{ m_deathDelayTimerBeforeOutro };

		EntityID m_chargeParticlePrefabID;
		std::string m_chargeParticlePrefab{ "RatKingChargingParticles.prefab" };

		bool m_hasInitiatedAudioFadeOut = false;				// Tracks if the audio fade out has been initiated
		//do i want to store all the obstacles here?
	}; // end of class 

} // End of namespace PE
