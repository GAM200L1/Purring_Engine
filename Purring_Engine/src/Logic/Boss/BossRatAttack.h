/*********************************************************************************
\project  Purring Engine
\module   CSD2401 - A
\file     BossRatAttack.h
\date     24 - 02 - 2024

\author   Jarran Tan Yan Zhi
\par      email : jarranyanzhi.tan@digipen.edu

\brief
virtual class for boss rat attack

All content(c) 2024 DigiPen Institute of Technology Singapore.All rights reserved.

* ************************************************************************************/
#pragma once

namespace PE
{
	class BossRatAttack
	{
	public:
		virtual void DrawTelegraphs() = 0;
		virtual void EnterAttack() = 0;
		virtual ~BossRatAttack() {}
	};

}