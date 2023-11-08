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
		ValueChange(value_type old_, value_type new_, value_type* p_location) : m_oldVal(old_), m_newVal(new_), p_value(p_location) {}
		virtual void Undo() override {
			*p_value = m_oldVal;
		}
		virtual void Redo() override {
			*p_value = m_newVal;
		}
		virtual ~ValueChange() {}
	private:
		value_type* p_value;
		value_type m_newVal;
		value_type m_oldVal;
	};

	template <typename T>
	class ValueChange2 : public EditorChanges
	{
	public:
		using value_type = T;
	public:
		ValueChange2(value_type old1_, value_type new1_, value_type* p_location1, value_type old2_, value_type new2_, value_type* p_location2) : m_oldVal1(old1_), m_newVal1(new1_), p_value1(p_location1) , m_oldVal2(old2_), m_newVal2(new2_), p_value2(p_location2)
		{

		}
		virtual void Undo() override {
			*p_value1 = m_oldVal1;
			*p_value2 = m_oldVal2;
		}
		virtual void Redo() override {
			*p_value1 = m_newVal1;
			*p_value2 = m_newVal2;
		}
		virtual ~ValueChange2() {}
	private:
		value_type* p_value1;
		value_type m_newVal1;
		value_type m_oldVal1;

		value_type* p_value2;
		value_type m_newVal2;
		value_type m_oldVal2;
	};
}