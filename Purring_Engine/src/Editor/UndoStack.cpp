#include "prpch.h"
#include "UndoStack.h"

namespace PE
{
	UndoStack::UndoStack()
	{
		m_currentcount = 0;
	}
	void UndoStack::AddChange(EditorChanges* ec)
	{
		m_undoStack.push_front(ec);
		++m_currentcount;
		if (m_currentcount == 20)
		{
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

	UndoStack::~UndoStack()
	{
		for (auto p : m_undoStack)
		{
			delete p;
		}
	}


}