/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     UndoStack.h
 \date     09-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the declaration of Undostack and virtual class EditorChanges
		This file also contains the declaration and definition of the derive classes of EditorChanges.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include <deque>
#include "ECS/EntityFactory.h"
typedef unsigned long long EntityID;
namespace PE
{
	class EditorChanges
	{
		// ----- Public Functions ----- //
	public:
		/*!***********************************************************************************
		 \brief     Undo Changes to be overriden
		*************************************************************************************/
		virtual void Undo() = 0;
		/*!***********************************************************************************
		 \brief     Redo Changes to be overriden
		*************************************************************************************/
		virtual void Redo() = 0;
		/*!***********************************************************************************
		 \brief     When a change leaves the undo stack
		*************************************************************************************/
		virtual void OnUndoStackLeave() {}
		/*!***********************************************************************************
		 \brief     When a change leaves the redo stack
		*************************************************************************************/
		virtual void OnRedoStackLeave() {}
		/*!***********************************************************************************
		 \brief     virtual destructor to ensure proper destruction of derived classes
		*************************************************************************************/
		virtual ~EditorChanges() {}
	};

	class UndoStack
	{
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief		Constructor for Undo Stack
		*************************************************************************************/
		UndoStack();
		/*!***********************************************************************************
		 \brief     Destructor for Undo Stack
		*************************************************************************************/
		~UndoStack();
		// ----- Public Functions ----- // 
	public:
		/*!***********************************************************************************
		 \brief			Adding New Change to the stack, clear Redo Stack
		 \param [In]	pointer to the new change
		*************************************************************************************/
		void AddChange(EditorChanges* p_change);
		/*!***********************************************************************************
		 \brief			Undo the change and pop it out of stack then send it to the redo stack
		*************************************************************************************/
		void UndoChange();
		/*!***********************************************************************************
		 \brief			Redo the change, pop it out of the stack send it back into undo stack
		*************************************************************************************/
		void RedoChange();
		/*!***********************************************************************************
		 \brief			Redo the change, pop it out of the stack send it back into undo stack
		*************************************************************************************/
		void ClearStack();
		// ----- Private Variables ----- // 
	private:
		std::deque<EditorChanges*> m_undoStack;
		std::deque<EditorChanges*> m_redoStack; // might not do this yet ill see
		int m_undoCount;
	};

	template <typename T>
	class ValueChange : public EditorChanges
	{
		// ----- Public Variables ----- // 
	public:
		using value_type = T;

		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief			deleted default constructor
		*************************************************************************************/
		ValueChange() = delete;
		/*!***********************************************************************************
		 \brief										constructor taking in values
		 \param [In]	value_type old_				old value
		 \param [In]	value_type new_				new value
		 \param [In]	value_type p_location		the location to change back
		*************************************************************************************/
		ValueChange(value_type old_, value_type new_, value_type* p_location) : m_oldVal(old_), m_newVal(new_), p_value(p_location) {}
		/*!***********************************************************************************
		 \brief			destructor for the value change
		*************************************************************************************/
		virtual ~ValueChange() {}

		// ----- Public Functions ----- // 
	public:
		/*!***********************************************************************************
		 \brief			overriden Undo Function
		*************************************************************************************/
		virtual void Undo() override 
		{
			*p_value = m_oldVal;
		}
		/*!***********************************************************************************
		 \brief			overriden Redo Function
		*************************************************************************************/
		virtual void Redo() override 
		{
			*p_value = m_newVal;
		}
		// ----- Private Variables ----- // 
	private:
		value_type* p_value;
		value_type m_newVal;
		value_type m_oldVal;
	};

	template <typename T>
	class ValueChange2 : public EditorChanges
	{
		// ----- Public Variables ----- // 
	public:
		using value_type = T;
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief			deleted default constructor
		*************************************************************************************/
		ValueChange2() = delete;
		/*!***********************************************************************************
		 \brief										constructor taking in values
		 \param [In]	value_type old1_			old value 1
		 \param [In]	value_type new1_			new value 1
		 \param [In]	value_type p_location1		location 1 to change
		 \param [In]	value_type old2_			old value 2
		 \param [In]	value_type new2_			new value 2
		 \param [In]	value_type p_location2		location 2 to change
		*************************************************************************************/
		ValueChange2(value_type old1_, value_type new1_, value_type* p_location1, value_type old2_, value_type new2_, value_type* p_location2) : 
			m_oldVal1(old1_), m_newVal1(new1_), p_value1(p_location1) , m_oldVal2(old2_), m_newVal2(new2_), p_value2(p_location2) {}
		/*!***********************************************************************************
		 \brief			destructor for the value change
		*************************************************************************************/
		virtual ~ValueChange2() {}
	public:
		// ----- Public Functions ----- // 
		/*!***********************************************************************************
		 \brief			overriden Undo Function
		*************************************************************************************/
		virtual void Undo() override 
		{
			*p_value1 = m_oldVal1;
			*p_value2 = m_oldVal2;
		}
		/*!***********************************************************************************
		 \brief			overriden Redo Function
		*************************************************************************************/
		virtual void Redo() override 
		{
			*p_value1 = m_newVal1;
			*p_value2 = m_newVal2;
		}
		// ----- Private Variables ----- // 
	private:
		value_type* p_value1;
		value_type m_newVal1;
		value_type m_oldVal1;

		value_type* p_value2;
		value_type m_newVal2;
		value_type m_oldVal2;
	};


	class DeleteObjectUndo : public EditorChanges
	{
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief			deleted default constructor
		*************************************************************************************/
		DeleteObjectUndo() = delete;
		/*!***********************************************************************************
		 \brief										constructor taking in values
		 \param [In]	EntityID id					id that is edited
		*************************************************************************************/
		DeleteObjectUndo(EntityID id) : ObjectDeleted(id) {}
	public:
		// ----- Public Functions ----- // 
		/*!***********************************************************************************
		 \brief			overriden Undo Function
		*************************************************************************************/
		virtual void Undo() override
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(ObjectDeleted).UnHandicapEntity();
		}
		/*!***********************************************************************************
		 \brief			overriden Redo Function
		*************************************************************************************/
		virtual void Redo() override
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(ObjectDeleted).HandicapEntity();
		}
		/*!***********************************************************************************
		 \brief     When a change leaves the undo stack
		*************************************************************************************/
		virtual void OnUndoStackLeave() override 
		{
			EntityManager::GetInstance().RemoveEntity(ObjectDeleted);
		}

		// ----- Private Variables ----- // 
	private:
		EntityID ObjectDeleted;
	};

	class CreateObjectUndo : public EditorChanges
	{
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief			deleted default constructor
		*************************************************************************************/
		CreateObjectUndo() = delete;
		/*!***********************************************************************************
		 \brief										constructor taking in values
		 \param [In]	EntityID id					id that is edited
		*************************************************************************************/
		CreateObjectUndo(EntityID id): ObjectCreated(id) {}
	public:
		// ----- Public Functions ----- // 
		/*!***********************************************************************************
		 \brief			overriden Undo Function
		*************************************************************************************/
		virtual void Undo() override
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(ObjectCreated).HandicapEntity();
		}
		/*!***********************************************************************************
		 \brief			overriden Redo Function
		*************************************************************************************/
		virtual void Redo() override
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(ObjectCreated).UnHandicapEntity();
		}
		/*!***********************************************************************************
		 \brief     When a change leaves the redo stack
		*************************************************************************************/
		virtual void OnRedoStackLeave() override
		{
			EntityManager::GetInstance().RemoveEntity(ObjectCreated);
		}
		// ----- Private Variables ----- // 
	private:
		EntityID ObjectCreated;
	};
}