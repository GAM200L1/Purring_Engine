#pragma once
#include <deque>


namespace PE
{
	class EditorChanges
	{
	public:
		EditorChanges() {}
		virtual void Undo() = 0;
		virtual void Redo() = 0;
		virtual ~EditorChanges() {}
	};

	class UndoStack
	{
	public:
		UndoStack();
		void AddChange(EditorChanges* change);
		//pop the stack here
		void UndoChange();
		void RedoChange();
		~UndoStack();
	private:
		std::deque<EditorChanges*> m_undoStack;
		std::deque<EditorChanges*> m_redoStack; // might not do this yet ill see
		int m_currentcount;
	};

	template <typename T>
	class ValueChange : public EditorChanges
	{
	public:
		using value_type = T;
	public:
		ValueChange(value_type old_, value_type new_ , value_type* p_location);
		virtual void Undo() override;
		virtual void Redo() override;
		virtual ~ValueChange();
	private:
		value_type* p_value;
		value_type m_newVal;
		value_type m_oldVal;
	};



	template<typename T>
	inline ValueChange<T>::ValueChange(value_type old_, value_type new_, value_type* p_location): m_oldVal(old_), m_newVal(new_), p_value(p_location)
	{
	}
	template<typename T>
	inline void ValueChange<T>::Undo()
	{
		*p_value = m_oldVal;
	}
	template<typename T>
	inline void ValueChange<T>::Redo()
	{
		*p_value = m_newVal;
	}
	template<typename T>
	inline ValueChange<T>::~ValueChange()
	{
	}
}