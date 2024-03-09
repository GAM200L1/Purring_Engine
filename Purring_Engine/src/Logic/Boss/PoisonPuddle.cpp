/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     PoisonPuddle.cpp 
 \date     09-03-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definitions of PoisonPuddle

 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "PoisonPuddle.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "Logic/GameStateController_v2_0.h"
#include "Logic/LogicSystem.h"
#include "Logic/Cat/CatController_v2_0.h"


namespace PE 
{
	void PoisonPuddle::Init(EntityID id)
	{
		//m_scriptData[id].currentTurn = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->CurrentTurn;
	}
	void PoisonPuddle::Update(EntityID id, float)
	{
		//if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->CurrentTurn == m_scriptData[id].currentTurn + 2)
		//{
		//	EntityManager::GetInstance().RemoveEntity(id);
		//}
	}
	void PoisonPuddle ::Destroy(EntityID)
	{
	}

	PoisonPuddle::~PoisonPuddle()
	{
	}

	void PoisonPuddle::OnAttach(EntityID id)
	{
		m_scriptData[id] = PoisonPuddleData();
	}

	void PoisonPuddle::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);
	}

	std::map<EntityID, PoisonPuddleData>& PoisonPuddle::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance PoisonPuddle::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

}