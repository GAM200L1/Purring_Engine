/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatBashAttack.h
\date     27 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
Declaration for BossRatBashAttack class

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#include "BossRatAttack.h"
#include "Events/EventHandler.h"
#include "Logic/Boss/BossRatScript.h"

namespace PE
{
	class BossRatBashAttack : public BossRatAttack
	{
		//constructors
	public:
		BossRatBashAttack() = delete;
		BossRatBashAttack(EntityID furthestCat);

		//public functions
	public:
		virtual void DrawTelegraphs(EntityID);
		virtual void EnterAttack(EntityID);
		virtual void UpdateAttack(EntityID,float);
		virtual void ExitAttack(EntityID);
		virtual ~BossRatBashAttack();
	private:
		bool CheckOutsideOfWall(vec2 Position);
		bool CheckCollisionWithTelegraphs();
	private:
		//need to take a snapshot of the current player positions here
		//actually no, only need the furtest cat
		EntityID m_FurthestCat{static_cast<EntityID>(-1)};
		BossRatScript* p_script;
		BossRatScriptData* p_data;
		std::string m_telegraphPrefab{"RatBossBashAttackTelegraph_Prefab.json"};
		std::vector<EntityID> m_telegraphPoitions;
		std::vector<EntityID> m_attackAnimations;
		int m_noOfAttack{};
		int m_attacksActivated{};

		float m_attackActivationTime;
		float m_attackDelay;

		float endExecutionTime{ 1 };
		float endExecutionTimer{ endExecutionTime };
	};
}