/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatController_v2_0.cpp
 \date     15-1-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains declarations for functions used for a grey player cat's states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"

#include "CatController_v2_0.h"
#include "CatHelperFunctions.h"
#include "FollowScript_v2_0.h"
#include "CatMovementStates_v2_0.h"
#include "CatPlanningState_v2_0.h"

#include "Layers/LayerManager.h"
#include "Logic/LogicSystem.h"
#include "Logic/FollowScript.h"
#include <cstdlib>   // For srand and rand
#include <ctime>     // For time

#include "GUISystem.h"

namespace PE
{
	CatController_v2_0::CatController_v2_0()
	{
		REGISTER_UI_FUNCTION(UndoCatPlanButtonCall, PE::CatController_v2_0);
	}

	CatController_v2_0::~CatController_v2_0()
	{

	}

	void CatController_v2_0::Init(EntityID id)
	{
		m_lostGame = false;
		UpdateCurrentCats(id);
		UpdateCachedCats(id);
		m_mouseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonPressed, CatController_v2_0::OnMouseClick, this);
		m_mouseReleaseEventListener = ADD_MOUSE_EVENT_LISTENER(PE::MouseEvents::MouseButtonReleased, CatController_v2_0::OnMouseRelease, this);
		
	}

	void CatController_v2_0::Update(EntityID id, float deltaTime)
	{
		if (GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->currentState == GameStates_v2_0::EXECUTE)
		{
			ClearCatUndoStack(); 
			return;
		}
		// undo if right mouse button triggered
		if (m_mouseClick && !m_mouseClickPrev)
			UndoCatPlan();
		m_mouseClickPrev = m_mouseClick;
	}

	void CatController_v2_0::OnAttach(EntityID id)
	{
		mainInstance = id;
		m_scriptData[id] = CatController_v2_0Data{};
		m_currentCats.clear();
		ClearCatUndoStack();
	}

	void CatController_v2_0::OnDetach(EntityID id)
	{
		auto iter = m_scriptData.find(id);
		if (iter != m_scriptData.end())
		{
			REMOVE_MOUSE_EVENT_LISTENER(m_mouseEventListener);
			REMOVE_MOUSE_EVENT_LISTENER(m_mouseReleaseEventListener);
			m_scriptData.erase(id);
		}
	}

	void CatController_v2_0::Destroy(EntityID id)
	{
		if (!m_lostGame)
			m_cachedCats = m_currentCats;
		ClearCatUndoStack();
	}

	// getters
	void CatController_v2_0::UpdateDeployableCats(EntityID mainInstanceID)
	{
		m_deployableCats.clear();
		for (auto [id, type] : m_currentCats)
		{
			if (IsCatAndNotCaged(id) && !IsFollowCat(id))
				m_deployableCats.emplace_back(type);
		}
		for (auto [id, type] : m_currentCats)
		{
			if (IsFollowCat(id))
				m_deployableCats.emplace_back(type);
		}
	}

	void CatController_v2_0::UpdateCurrentCats(EntityID)
	{
		m_currentCats.clear();
		for (const auto& layer : LayerView<ScriptComponent>())
		{
			for (EntityID catID : InternalView(layer))
			{
				auto const& r_scripts = EntityManager::GetInstance().Get<ScriptComponent>(catID).m_scriptKeys;
				//if (IsCat(catID))
				for (auto& [scriptname, state] : r_scripts)
				{
					if (scriptname == "CatScript_v2_0")
					{
						EnumCatType const& r_catType = *GETSCRIPTDATA(CatScript_v2_0, catID).catType;
						m_currentCats.emplace_back(std::pair{ catID, r_catType }); // saves all cats, including the caged ones
						if (r_catType == EnumCatType::MAINCAT)
							m_mainCatID = catID;
					}
				}
			}
		}
		std::sort(m_currentCats.begin(), m_currentCats.end(), [](std::pair<EntityID, EnumCatType> const& lhs, std::pair<EntityID, EnumCatType> const& rhs)
			{
				return lhs.second < rhs.second;
			});
	}

	void CatController_v2_0::KillCat(EntityID id)
	{
		if (IsCatAndNotCaged(id))
		{
			EntityID catToRemove = id;
		
			// if cat was part of the following chain, kill just that cat and remove from followers vector
			if ((m_mainCatID == id && !(GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.empty()) || IsFollowCat(id))
			{
				catToRemove = (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.back();
				(GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.pop_back();
			}
			else if (id == m_mainCatID && (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.empty())
			{
				m_lostGame = true;
			}

			// toggle death animation for cat that has been hit
			(GETSCRIPTDATA(CatScript_v2_0, catToRemove))->toggleDeathAnimation = true;
			RemoveCatFromCurrent(catToRemove);
		}
	}

	void CatController_v2_0::RemoveCatFromCurrent(EntityID id)
	{
		EnumCatType const& r_catType = (GETSCRIPTDATA(CatScript_v2_0, id))->catType;
		m_currentCats.erase(std::find(m_currentCats.begin(), m_currentCats.end(), std::pair{id, r_catType}));
	}

	int CatController_v2_0::GetCurrentMovementEnergy(EntityID catID)
	{
		return (GETSCRIPTDATA(CatScript_v2_0, catID))->catCurrentEnergy;
	}
	
	int CatController_v2_0::GetMaxMovementEnergy(EntityID catID)
	{
		return (GETSCRIPTDATA(CatScript_v2_0, catID))->catMaxMovementEnergy;
	}

	bool CatController_v2_0::IsFollowCat(EntityID catID)
	{
		return (std::find((GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.begin(), 
						  (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.end(), catID) != (GETSCRIPTDATA(FollowScript_v2_0, m_mainCatID))->followers.end());
	}
	
	void CatController_v2_0::AddToUndoStack(EntityID catID, EnumUndoType undoType)
	{
		m_catUndoStack.push(std::make_pair(catID, undoType));
	}

	void CatController_v2_0::UndoCatPlan()
	{
		if (m_catUndoStack.empty()) { return; }
		// get the id of the cat to undo and which planning to undo
		auto const& toUndo = m_catUndoStack.top();
		// pop the stack
		m_catUndoStack.pop();

		// get the planning state and call reset functions based on the undo type
		Cat_v2_0PLAN* planState = dynamic_cast<Cat_v2_0PLAN*>((GETSCRIPTDATA(CatScript_v2_0, toUndo.first))->p_stateManager->GetCurrentState());
		if (toUndo.second == EnumUndoType::UNDO_MOVEMENT)
		{ planState->ResetMovement(toUndo.first);}
		else if (toUndo.second == EnumUndoType::UNDO_ATTACK)
		{ planState->ResetAttack(toUndo.first); }
	}
	
	void CatController_v2_0::UndoCatPlanButtonCall(EntityID)
	{
		UndoCatPlan();
	}

	void CatController_v2_0::ClearCatUndoStack()
	{
		while (!m_catUndoStack.empty())
			m_catUndoStack.pop();
	}

	void CatController_v2_0::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		if (r_ME.GetType() == MouseEvents::MouseButtonPressed)
		{
			MouseButtonPressedEvent MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);
			if (MBPE.button == 1)
				m_mouseClick = true;
		}
	}

	void CatController_v2_0::OnMouseRelease(const Event<MouseEvents>& r_ME)
	{
		if (r_ME.GetType() == MouseEvents::MouseButtonReleased)
		{
			MouseButtonReleaseEvent MBRE = dynamic_cast<const MouseButtonReleaseEvent&>(r_ME);
			if (MBRE.button == 1)
				m_mouseClick = false;
		}
	}
}