#include "prpch.h"
#include "UndoStack.h"

namespace PE
{
	void UndoStack::AddChange(EditorChanges&& ec)
	{
		m_undoStack.push_front(ec);
		++currentcount;
		if (currentcount == 20)
		{
			m_undoStack.pop_back();
		}

	}
	void UndoStack::UndoChange()
	{
		if (!m_undoStack.empty())
		{
			m_undoStack.front().Undo();
			m_undoStack.pop_front();
			--currentcount;
		}

	}
	void UndoStack::RedoChange()
	{
	}

	template<typename T>
	ValueChange<T>::ValueChange(value_type old_, value_type new_)
	{
	}
	template<typename T>
	void ValueChange<T>::Undo()
	{
		p_value = m_oldVal;
	}
	template<typename T>
	void ValueChange<T>::Redo()
	{
		p_value = m_newVal;
	}
}