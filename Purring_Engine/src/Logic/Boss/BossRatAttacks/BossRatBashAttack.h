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

namespace PE
{
	class BossRatBashAttack : public BossRatAttack
	{
		virtual void DrawTelegraphs();
		virtual void EnterAttack();
		virtual void UpdateAttack();
		virtual void ExitAttack();
		virtual ~BossRatBashAttack();
	};
}