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
#include "Editor.h"
#include "Singleton.h"
#include "ECS/EntityFactory.h"
#include "Hierarchy/HierarchyManager.h"
#include <glm/glm.hpp>
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

	class UndoStack : public Singleton<UndoStack>
	{
		// ----- Singleton ----- // 
		friend class Singleton<UndoStack>;

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
		DeleteObjectUndo(EntityID id) : m_objectDeleted(id) 
		{
		}
	public:
		// ----- Public Functions ----- // 
		/*!***********************************************************************************
		 \brief			overriden Undo Function
		*************************************************************************************/
		virtual void Undo() override
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).UnHandicapEntity();
			if (EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).parent)
			{
				Hierarchy::GetInstance().AttachChild(EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).parent.value(), m_objectDeleted);
			}
			for (const auto& id : EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).savedChildren)
			{
				Hierarchy::GetInstance().AttachChild(m_objectDeleted, id);
			}
			EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).savedChildren.clear();
		}
		/*!***********************************************************************************
		 \brief			overriden Redo Function
		*************************************************************************************/
		virtual void Redo() override
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).children.size())
			{
				for (auto cid : EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).children)
				{
					EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).savedChildren.emplace_back(cid);
				}

				for (const auto& cid : EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).savedChildren)
				{
					if (EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).parent.has_value())
						Hierarchy::GetInstance().AttachChild(EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).parent.value(), cid);
					else
						Hierarchy::GetInstance().DetachChild(cid);
				}
			}
			EntityManager::GetInstance().Get<EntityDescriptor>(m_objectDeleted).HandicapEntity();
			Editor::GetInstance().ResetSelectedObject();
		}
		/*!***********************************************************************************
		 \brief     When a change leaves the undo stack
		*************************************************************************************/
		virtual void OnUndoStackLeave() override 
		{
			EntityManager::GetInstance().RemoveEntity(m_objectDeleted);
		}

		// ----- Private Variables ----- // 
	private:
		EntityID m_objectDeleted;
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
		CreateObjectUndo(EntityID id): m_objectCreated(id) {}
	public:
		// ----- Public Functions ----- // 
		/*!***********************************************************************************
		 \brief			overriden Undo Function
		*************************************************************************************/
		virtual void Undo() override
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(m_objectCreated).HandicapEntity();
			Editor::GetInstance().ResetSelectedObject();
		}
		/*!***********************************************************************************
		 \brief			overriden Redo Function
		*************************************************************************************/
		virtual void Redo() override
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(m_objectCreated).UnHandicapEntity();
		}
		/*!***********************************************************************************
		 \brief     When a change leaves the redo stack
		*************************************************************************************/
		virtual void OnRedoStackLeave() override
		{
			EntityManager::GetInstance().RemoveEntity(m_objectCreated);
		}
		// ----- Private Variables ----- // 
	private:
		EntityID m_objectCreated;
	};

	class GuizmoUndo : public EditorChanges
	{
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief			deleted default constructor
		*************************************************************************************/
		GuizmoUndo() = delete;
		/*!***********************************************************************************
		 \brief										constructor taking in values
		 \param [In]	EntityID id					id that is edited
		*************************************************************************************/
		GuizmoUndo(Transform old_t, Transform new_t, Transform* p_t, bool hasParent) :m_oldTransform(old_t),m_newTransform(new_t), p_location(p_t),m_hasParent(hasParent)
		{		
		}
	public:
		// ----- Public Functions ----- // 
		/*!***********************************************************************************
		 \brief			overriden Undo Function
		*************************************************************************************/
		virtual void Undo() override
		{
			p_location->width = m_oldTransform.width;
			p_location->height = m_oldTransform.height;
			p_location->orientation = m_oldTransform.orientation;
			p_location->position = m_oldTransform.position;
			p_location->relOrientation = m_oldTransform.relOrientation;
			p_location->relPosition = m_oldTransform.relPosition;
		}
		/*!***********************************************************************************
		 \brief			overriden Redo Function
		*************************************************************************************/
		virtual void Redo() override
		{
			p_location->width = m_newTransform.width;
			p_location->height = m_newTransform.height;
			p_location->orientation = m_newTransform.orientation;
			p_location->position = m_newTransform.position;
			p_location->relOrientation = m_newTransform.relOrientation;
			p_location->relPosition = m_newTransform.relPosition;
		}
		// ----- Private Variables ----- // 
	private:
		Transform m_oldTransform{};
		Transform m_newTransform{};
		Transform* p_location;
		bool m_hasParent;
	};
}