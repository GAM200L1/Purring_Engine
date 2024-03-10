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

#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"

namespace PE
{
	class BossRatAttack
	{
	public:
		/*!***********************************************************************************
		 \brief virtual function to draw telegraphs for attacks at the start of planning phase
		 \param[in] id - id of the boss
		 \return void
		*************************************************************************************/
		virtual void DrawTelegraphs(EntityID) = 0;
		/*!***********************************************************************************
		 \brief virtual function that is called at entering boss attack state
		 \param[in] id - id of the boss
		 \return void
		*************************************************************************************/
		virtual void EnterAttack(EntityID) = 0;
		/*!***********************************************************************************
		 \brief virtual function that is called on update of boss attack state
		 \param[in] id - id of the boss
		 \param[in] float - delta time
		 \return void
		*************************************************************************************/
		virtual void UpdateAttack(EntityID,float) = 0;
		/*!***********************************************************************************
		 \brief virtual function that is called at exiting the boss attack state
		 \param[in] id - id of the boss
		 \return void
		*************************************************************************************/
		virtual void ExitAttack(EntityID) = 0;
		/*!***********************************************************************************
		 \brief virtual function that is called to stop an attack in the middle of someother place
		 \return void
		*************************************************************************************/
		virtual void StopAttack() {}
		/*!***********************************************************************************
		 \brief virtual destructor for the boss rat attack
		*************************************************************************************/
		virtual ~BossRatAttack() {}
	};

}