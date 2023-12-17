#include "HierarchyManager.h"
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     HierarchyManager.cpp
 \date     17-12-2023

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   huh

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
	}

	void Hierarchy::DetachChild(const EntityID& child)
	{
		if (EntityManager::GetInstance().Get<EntityDescriptor>(child).parent)
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.value()).children.erase(child);
		}
		EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.reset();
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
			if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.has_value())
			{
				parentOrder.emplace_back(id);
			}
			// otherwise it has no children, can ignore
		}
	}
	void Hierarchy::UpdateTransform()
	{
		/*for (const auto& id : SceneView<Transform>())
		{
			Transform& trans = EntityManager::GetInstance().Get<Transform>(id);
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.has_value())
			{
				const Transform& parent = EntityManager::GetInstance().Get<Transform>(EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.value());
				vec3 tmp{ trans.relPosition, 1.f };
				tmp = parent.GetTransformMatrix3x3() * tmp;
				trans.position.x = tmp.x;
				trans.position.y = tmp.y;
				trans.orientation = parent.orientation + trans.relOrientation;
			}
		}*/
		for (const EntityID& parentID : parentOrder)
		{
			UpdateHelper(parentID);
		}
	}

	void Hierarchy::UpdateETC()
	{
	}

	void Hierarchy::UpdateRenderOrder()
	{
	}
}