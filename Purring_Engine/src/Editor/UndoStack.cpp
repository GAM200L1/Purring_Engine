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
		m_undoCount = 0;
	}
	void UndoStack::AddChange(EditorChanges* p_change)
	{
		m_undoStack.push_front(p_change);
		++m_undoCount;

		//clear redo stack here
		for (auto i : m_redoStack)
		{
			i->OnRedoStackLeave();
			delete i;
		}
		m_redoStack.clear();

		if (m_undoCount >= 21)
		{
			m_undoStack.back()->OnUndoStackLeave();
			--m_undoCount;
			delete m_undoStack.back();
			m_undoStack.pop_back();
		}

	}
	void UndoStack::UndoChange()
	{
		if (!m_undoStack.empty())
		{
			//undo the change
			m_undoStack.front()->Undo();

			//push it into the redo stack
			m_redoStack.emplace_front(m_undoStack.front());
			//remove the pointer from the undo stack
			m_undoStack.pop_front();
			//reduce undo count
			--m_undoCount;
		}

	}
	void UndoStack::RedoChange()
	{
		if (!m_redoStack.empty())
		{
			m_redoStack.front()->Redo();
			//send back to undo stack
			m_undoStack.emplace_front(m_redoStack.front());
			//remove the pointer from the redo stack
			m_redoStack.pop_front();
			++m_undoCount;
		}
	}

	void UndoStack::ClearStack()
	{
		//clear redo stack here
		for (auto i : m_redoStack)
		{
			i->OnRedoStackLeave();
			delete i;
		}
		m_redoStack.clear();

		for(auto& a:m_undoStack)
		{
			a->OnUndoStackLeave();
			delete a;			
		}
		m_undoStack.clear();
		m_undoCount=0;
	}

	UndoStack::~UndoStack()
	{
		for (auto p : m_undoStack)
		{
			delete p;
		}
	}


}