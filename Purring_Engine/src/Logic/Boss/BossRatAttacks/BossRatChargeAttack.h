/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatChargeAttack.h
\date     08 - 03 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Declaration for BossRatChargeAttack class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "BossRatAttack.h"
#include "Events/EventHandler.h"
#include "Logic/Boss/BossRatScript.h"

namespace PE
{
	class BossRatChargeAttack : public BossRatAttack
	{
		//constructors
	public:
		/*!***********************************************************************************
		 \brief Delete default constructor as we will always want to take in the furthest cat
		*************************************************************************************/
		BossRatChargeAttack() = delete;
		/*!***********************************************************************************
		 \brief constructor for the boss rat charge attack
		 \param[in] id - furthest cat to target
		*************************************************************************************/
		BossRatChargeAttack(EntityID furthestCat);

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
		 \brief virtual function that is called to stop an attack in the middle of someother place
		 \return void
		*************************************************************************************/
		virtual void StopAttack();
		/*!***********************************************************************************
		 \brief virtual destructor for the boss rat charge attack
		*************************************************************************************/
		virtual ~BossRatChargeAttack();
	private:
		//need to take a snapshot of the current player positions here
		//actually no, only need the furtest cat
		EntityID m_furthestCat{static_cast<EntityID>(-1)};
		BossRatScript* p_script;
		BossRatScriptData* p_data;
		std::string m_telegraphPrefab{"RatBossChargeAttackTelegraph.prefab"};
		vec2 m_chargeDirection{};
		EntityID m_telegraph;


		float m_travelTime{ .05f };
		float m_travelTimer{ m_travelTime };
		bool m_isCharging{};
		float m_activationTime{};
		float m_chargeEndDelay{1.f};
	};
}