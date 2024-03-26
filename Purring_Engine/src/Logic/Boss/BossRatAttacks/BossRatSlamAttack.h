/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatSlamAttack.h
\date     27 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Declaration for BossRatSlamAttack class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "BossRatAttack.h"
#include "Events/EventHandler.h"
#include "Logic/Boss/BossRatScript.h"

namespace PE
{
	class BossRatSlamAttack : public BossRatAttack
	{
		//constructors
	public:
		/*!***********************************************************************************
		 \brief default constructor for the Boss Rat Slam Attack
		*************************************************************************************/
		BossRatSlamAttack();

		//public functions
	public:
		/*!***********************************************************************************
		 \brief virtual function to draw telegraphs for attacks at the start of planning phase
		 \param[in] id - id of the boss
		 \return void
		*************************************************************************************/
		virtual void DrawTelegraphs(EntityID);
		/*!***********************************************************************************
		 \brief virtual function that is called at entering boss attack state
		 \param[in] id - id of the boss
		 \return void
		*************************************************************************************/
		virtual void EnterAttack(EntityID);
		/*!***********************************************************************************
		 \brief virtual function that is called on update of boss attack state
		 \param[in] id - id of the boss
		 \param[in] float - delta time
		 \return void
		*************************************************************************************/
		virtual void UpdateAttack(EntityID,float);
		/*!***********************************************************************************
		\brief virtual function that is called at exiting the boss attack state
		\param[in] id - id of the boss
		\return void
		*************************************************************************************/
		virtual void ExitAttack(EntityID);
		/*!***********************************************************************************
		 \brief virtual destructor for the boss rat slam attack
		*************************************************************************************/
		virtual ~BossRatSlamAttack();
	private:
		/*!***********************************************************************************
		\brief  function to decide which side the boss is going to land
		*************************************************************************************/
		void DecideSide();
		/*!***********************************************************************************
		\brief Jump Function for the boss
		\param[in] id - id of the boss
		\param[in] float - delta time
		*************************************************************************************/
		void JumpUp(EntityID,float);
		/*!***********************************************************************************
		\brief Slam Function for the boss
		\param[in] id - id of the boss
		\param[in] float - delta time
		*************************************************************************************/
		void SlamDown(EntityID,float);
		/*!***********************************************************************************
		\brief function to spawn rats
		\param[in] id - id of the boss
		*************************************************************************************/
		void SpawnRat(EntityID);
		/*!***********************************************************************************
		\brief function to hide the initial slam attack telegraph
		\param[in] id - id of the boss
		*************************************************************************************/
		void HideTelegraph(EntityID);
		/*!***********************************************************************************
		\brief function to draw the slam damaging telegraph
		\param[in] id - id of the boss
		*************************************************************************************/
		void DrawDamageTelegraph(EntityID);
		/*!***********************************************************************************
		\brief function to update the damaging area telegraph
		\param[in] id - id of the boss
		*************************************************************************************/
		void UpdateDamageTelegraph(EntityID);
		/*!***********************************************************************************
		\brief function to update the slam area telegraph
		\param[in] id - id of the boss
		*************************************************************************************/
		void UpdateSlamTelegraph(EntityID,float);
		/*!***********************************************************************************
		\brief function to hide the damaging area telegraph and the slam area telegraph
		\param[in] id - id of the boss
		*************************************************************************************/
		void HideDamageTelegraph(EntityID);
		/*!***********************************************************************************
		\brief check if any cat in the landing area
		\param[in] id - id of the boss
		*************************************************************************************/
		void CheckDamage(EntityID);
		/*!***********************************************************************************
		\brief enable any damage animation
		\param[in] id - id of the boss
		*************************************************************************************/
		void EnableAnimation(EntityID);
		/*!***********************************************************************************
		\brief disable any damage animation
		\param[in] id - id of the boss
		*************************************************************************************/
		void DisableAnimation(EntityID);
	private:
		BossRatScript* p_script;
		BossRatScriptData* p_data;
		std::string m_telegraphPrefab{"BossRatSlamAttackTelegraph.prefab"};
		float m_screenHeight{ 1080 };
		vec2 m_slamLandLocation{};
		float m_slamSize{ 384 };
		float m_slamMinSize{ 256};
		bool m_slamTelegraphAnimated{ false };
		float m_slamShrinkSpeed{ 350 };
		bool m_attackIsLeft{};
		float m_slamAttackDelay{ 0.5f };
		bool m_ratSpawned{ false };
		bool m_playedSlamAudio{ false };
		bool m_hasJumped{ false };


		bool m_hasSpawnedRats{ false };
		bool m_hasMovedRats{ false };
		float spawnRatDelay{ 0.f };
		std::vector<int> m_ratSpawnedKeys;
	};
}