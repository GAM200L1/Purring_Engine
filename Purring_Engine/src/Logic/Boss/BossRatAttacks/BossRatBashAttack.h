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
		/*!***********************************************************************************
		 \brief Delete default constructor as we will always want to take in the closest cat
		*************************************************************************************/
		BossRatBashAttack() = delete;
		/*!***********************************************************************************
		 \brief constructor for the boss rat bash attack
		 \param[in] id - closest cat to target
		*************************************************************************************/
		BossRatBashAttack(EntityID closestCat);

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
		 \brief virtual destructor for the boss bash attack
		*************************************************************************************/
		virtual ~BossRatBashAttack();
	private:
		/*!***********************************************************************************
		 \brief Check if the telegraph will be drawn inside of outside of a wall
		 \param[in] vec2 Position - position of the telegraph
		 \return boolean , true if outside of wall , false if inside of wall
		*************************************************************************************/
		bool CheckOutsideOfWall(vec2 Position);
		/*!***********************************************************************************
		 \brief Check if cat is in the telegraph of a spike animation attack
		 \return boolean , true if cat is in the telegraph , false if cat is not in the telegraph
		*************************************************************************************/
		bool CheckCollisionWithTelegraphs();
	private:
		//need to take a snapshot of the current player positions here
		//actually no, only need the furtest cat
		EntityID m_closestCat{static_cast<EntityID>(-1)};
		BossRatScript* p_script;
		BossRatScriptData* p_data;
		std::string m_telegraphPrefab{"RatBossBashAttackTelegraphwAnim.prefab"};
		std::vector<EntityID> m_telegraphPoitions;
		std::vector<EntityID> m_attackAnimations;
		int m_noOfAttack{};
		int m_attacksActivated{};

		float m_attackActivationTime;
		float m_attackDelay;

		float m_endExecutionTime{ 1 };
		float m_endExecutionTimer{ m_endExecutionTime };
	};
}