/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     HierarchyManager.cpp
 \date     17-12-2023

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   This file contains the defenitions of the Hierarchy class (singleton), that 
 		   manages the Transform Hierarchy (any additional behaviour TBD) of the 
		   entities in the scene.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"

#include "HierarchyManager.h"



namespace PE
{
	void Hierarchy::Update()
	{
		UpdateParentList();
		UpdateTransform();
		UpdateETC();
		UpdateRenderOrder();
	}

	void Hierarchy::AttachChild(const EntityID& parent, const EntityID& child)
	{
		if (EntityManager::GetInstance().Get<EntityDescriptor>(child).parent)
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.value()).children.erase(child);
		}
		EntityManager::GetInstance().Get<EntityDescriptor>(parent).children.emplace(child);
		EntityManager::GetInstance().Get<EntityDescriptor>(child).parent = parent;
		vec3 tmpc (EntityManager::GetInstance().Get<Transform>(child).position, 1.f);
		// compute the relative position of the current object to the parent to get the proper relative position to set
		tmpc = EntityManager::GetInstance().Get<Transform>(parent).GetTransformMatrix3x3().Inverse() * tmpc;

		// set the relative positon to the computed values
		EntityManager::GetInstance().Get<Transform>(child).relPosition = vec2(tmpc.x, tmpc.y);
		EntityManager::GetInstance().Get<Transform>(child).relOrientation = EntityManager::GetInstance().Get<Transform>(child).orientation - EntityManager::GetInstance().Get<Transform>(parent).orientation;
	}

	void Hierarchy::DetachChild(const EntityID& child)
	{
		if (EntityManager::GetInstance().Get<EntityDescriptor>(child).parent)
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.value()).children.erase(child);
		}
		EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.reset();
		EntityManager::GetInstance().Get<Transform>(child).relPosition.Zero();
		EntityManager::GetInstance().Get<Transform>(child).relOrientation = 0;
	}

	const std::set<EntityID>& Hierarchy::GetChildren(const EntityID& parent) const
	{
		return EntityManager::GetInstance().Get<EntityDescriptor>(parent).children;
	}

	const std::optional<EntityID>& Hierarchy::GetParent(const EntityID& child) const
	{
		return EntityManager::GetInstance().Get<EntityDescriptor>(child).parent;
	}

	bool Hierarchy::HasParent(const EntityID& child) const
	{
		return EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.has_value();
	}

	bool Hierarchy::IsImmediateParentActive(EntityID child) const
	{
		auto const& parent{ EntityManager::GetInstance().Get<EntityDescriptor>(child).parent };
		return (parent.has_value() ? 
				EntityManager::GetInstance().Get<EntityDescriptor>(parent.value()).isActive : // return active status of parent
				EntityManager::GetInstance().Get<EntityDescriptor>(child).isActive);		// return active status of obj
	}

	bool Hierarchy::AreParentsActive(EntityID child) const
	{
		// Loop through all the parents to check their active statues
		while (HasParent(child))
		{
			if (!EntityManager::GetInstance().Get<EntityDescriptor>(GetParent(child).value()).isActive)
				return false;

			child = GetParent(child).value();
		}
		return EntityManager::GetInstance().Get<EntityDescriptor>(child).isActive;
	}

	const std::optional<EntityID>& Hierarchy::GetAbsoluteParent(EntityID child) const
	{
		while (HasParent(child))
		{
			// If its parent has a parent, store its ID so we can check that its parent has a parent
			EntityID parentId{ EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.value() };
			if (HasParent(EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.value()))
				child = parentId;
			else
				break;
		}

		return EntityManager::GetInstance().Get<EntityDescriptor>(child).parent;
	}

	// recursive function to help with parent child ordering
	void Hierarchy::UpdateHelper(const EntityID& r_parentID)
	{
		const Transform& parent = EntityManager::GetInstance().Get<Transform>(r_parentID);

		for (const EntityID& childrenID : EntityManager::GetInstance().Get<EntityDescriptor>(r_parentID).children)
		{
			if (EntityManager::GetInstance().Has<Transform>(childrenID))
			{
				Transform& trans = EntityManager::GetInstance().Get<Transform>(childrenID);
				vec3 tmp{ trans.relPosition, 1.f };
				tmp = parent.GetTransformMatrix3x3() * tmp;
				trans.position.x = tmp.x;
				trans.position.y = tmp.y;
				trans.orientation = parent.orientation + trans.relOrientation;
			}
			// if it has children recursively call this function, with the current ID as the input
			if (EntityManager::GetInstance().Get<EntityDescriptor>(childrenID).children.size())
			{
				UpdateHelper(childrenID);
			}
		}
	}

	void Hierarchy::UpdateParentList()
	{
		parentOrder.clear();
		for (const EntityID& id : SceneView<EntityDescriptor, Transform>())
		{
			// only if it is base layer
			if (!HasParent(id))
			{
				parentOrder.emplace_back(id);
			}
		}
	}

	void Hierarchy::UpdateTransform()
	{
		for (const EntityID& parentID : parentOrder)
		{
			UpdateHelper(parentID);
		}
	}

	void Hierarchy::UpdateETC()
	{
		// empty for now
	}

	void Hierarchy::UpdateRenderOrder()
	{
	}
}