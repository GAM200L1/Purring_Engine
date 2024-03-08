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
		BossRatChargeAttack() = delete;
		BossRatChargeAttack(EntityID furthestCat);

		//public functions
	public:
		virtual void DrawTelegraphs(EntityID);
		virtual void EnterAttack(EntityID);
		virtual void UpdateAttack(EntityID,float);
		virtual void ExitAttack(EntityID);
		virtual void StopAttack();
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