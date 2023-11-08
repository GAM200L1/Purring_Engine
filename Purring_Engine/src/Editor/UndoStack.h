#pragma once
#include <deque>


namespace PE
{
	class EditorChanges
	{
	public:
		virtual void Undo() = 0;
		virtual void Redo() = 0;
	};

	class UndoStack
	{
	public:
		void AddChange(EditorChanges&& change);
		//pop the stack here
		void UndoChange();
		void RedoChange();

	private:
		std::deque<EditorChanges&&> m_undoStack;
		std::deque<EditorChanges&&> redoStack; // might not do this yet ill see
		int currentcount;
	};

	template <typename T>
	class ValueChange : public EditorChanges
	{
	public:
		using value_type = T;
	public:
		ValueChange(value_type old_, value_type new_);
		virtual void Undo() override;
		virtual void Redo() override;

	private:
		value_type* p_value;
		value_type m_newVal;
		value_type m_oldVal;
	};



}