/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     UndoStack.cpp
 \date     09-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definition of Undostack		

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "UndoStack.h"

namespace PE
{
	UndoStack::UndoStack()
	{
		m_currentcount = 0;
	}
	void UndoStack::AddChange(EditorChanges* p_change)
	{
		m_undoStack.push_front(p_change);
		++m_currentcount;
		if (m_currentcount >= 21)
		{
			m_undoStack.back()->OnStackLeave();
			--m_currentcount;
			delete m_undoStack.back();
			m_undoStack.pop_back();
		}

	}
	void UndoStack::UndoChange()
	{
		if (!m_undoStack.empty())
		{
			m_undoStack.front()->Undo();
			delete m_undoStack.front();
			m_undoStack.pop_front();
			--m_currentcount;
		}

	}
	void UndoStack::RedoChange()
	{
	}

	void UndoStack::ClearStack()
	{
		for(auto& a:m_undoStack)
		{
			a->OnStackLeave();
			delete a;			
		}
		m_undoStack.clear();
		m_currentcount=0;
	}

	UndoStack::~UndoStack()
	{
		for (auto p : m_undoStack)
		{
			delete p;
		}
	}


}