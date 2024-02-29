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

namespace PE
{
	struct BossRatScriptData
	{
		~BossRatScriptData()
		{
			delete p_stateManager;
			delete p_currentAttack;
		}

		//other boss stats

		int health = 20;
		StateMachine* p_stateManager;
		BossRatAttack* p_currentAttack;
		bool finishExecution{true};

		//bash attack variables
		float TelegraphRaadius;
		float attackDelay{.25f};
		float activationTime{1.f};
	};

	class BossRatScript : public Script
	{
		// If you wish to have persistent data, store it in the script class

		// ----- Public Members ----- //
	public:
		std::map<EntityID, BossRatScriptData> m_scriptData;
		EntityID currentBoss;

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

		void TakeDamage(int damage);

		EntityID FindFurthestCat();

		void FindAllObstacles();

		std::vector<EntityID> GetAllObstacles();

		// ----- Private Members ----- //
	private:
		// --- STATE CHANGE --- //

		/*!***********************************************************************************
		 \brief Creates the state manager if it has not been created.

		 \param[in] id EntityID of the entity to create the state manager for.
		*************************************************************************************/
		void CreateCheckStateManager(EntityID id);



	private:
		// ----- Private Variables ----- //
		GameStateController_v2_0* p_gsc = nullptr;
		std::vector<EntityID> m_Obstacles;
		//do i want to store all the obstacles here?
	}; // end of class 

} // End of namespace PE
