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
		BossRatSlamAttack() = delete;
		BossRatSlamAttack(EntityID furthestCat);

		//public functions
	public:
		virtual void DrawTelegraphs(EntityID);
		virtual void EnterAttack(EntityID);
		virtual void UpdateAttack(EntityID,float);
		virtual void ExitAttack(EntityID);
		virtual ~BossRatSlamAttack();
	private:
		void DecideSide();
		void JumpUp(EntityID,float);
		void SlamDown(EntityID,float);
		void SpawnRat(EntityID);
		void HideTelegraph(EntityID);
		void DrawDamageTelegraph(EntityID);
		void UpdateDamageTelegraph(EntityID);
		void UpdateSlamTelegraph(EntityID,float);
		void HideDamageTelegraph(EntityID);
		void CheckDamage(EntityID);
		void EnableAnimation(EntityID);
		void DisableAnimation(EntityID);
	private:
		//need to take a snapshot of the current player positions here
		//actually no, only need the furtest cat
		EntityID m_FurthestCat{static_cast<EntityID>(-1)};
		BossRatScript* p_script;
		BossRatScriptData* p_data;
		std::string m_telegraphPrefab{"BossRatSlamAttackTelegraph.prefab"};
		float m_screenHeight{ 1080 };
		vec2 m_slamLandLocation{};
		float m_slamSize{ 384 };
		float m_slamMinSize{ 256};
		bool m_slamTelegraphAnimated{ false };
		float m_slamShrinkSpeed{ 500 };
		bool m_attackIsLeft{};


		bool m_ratSpawned{ false };

	};
}