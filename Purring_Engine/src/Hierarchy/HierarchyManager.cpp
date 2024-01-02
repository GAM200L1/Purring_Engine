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
#include "Logging/Logger.h"

extern Logger engine_logger;



namespace PE
{

	void Hierarchy::Update()
	{
		UpdateParentList();
		UpdateTransform();
		UpdateETC();
		UpdateRenderOrder();
	}

	// used to check if the target id is found in the child
	bool RecursionHelper(EntityID targetID, EntityDescriptor& r_targetDescriptor)
	{
		if (r_targetDescriptor.children.find(targetID) != std::end(r_targetDescriptor.children))
		{
			return true;
		}

		if (EntityManager::GetInstance().Get<EntityDescriptor>(targetID).parent)
			return RecursionHelper(EntityManager::GetInstance().Get<EntityDescriptor>(targetID).parent.value(), r_targetDescriptor);

		return false;
	}

	void Hierarchy::AttachChild(const EntityID parent, const EntityID child)
	{
		// entity DNE, return
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(child))
			return;
		
		if (RecursionHelper(parent, EntityManager::GetInstance().Get<EntityDescriptor>(child)))
		{
			return;
		}
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
		if (!EntityManager::GetInstance().Get<EntityDescriptor>(parent).isActive)
			EntityManager::GetInstance().Get<EntityDescriptor>(child).DisableEntity();
		
		UpdateRenderOrder(parent);
	}

	void Hierarchy::DetachChild(const EntityID& child)
	{
		// entity DNE, return
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(child) || !EntityManager::GetInstance().Get<EntityDescriptor>(child).isAlive)
			return;
		if (EntityManager::GetInstance().Get<EntityDescriptor>(child).parent)
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.value()))
				EntityManager::GetInstance().Get<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.value()).children.erase(child);
		}
		EntityManager::GetInstance().Get<EntityDescriptor>(child).parent.reset();
		if (EntityManager::GetInstance().Has<Transform>(child))
		{
			EntityManager::GetInstance().Get<Transform>(child).relPosition.Zero();
			EntityManager::GetInstance().Get<Transform>(child).relOrientation = 0;
		}
		UpdateRenderOrder(child);
	}

	inline const std::set<EntityID>& Hierarchy::GetChildren(const EntityID& parent) const
	{
		return EntityManager::GetInstance().Get<EntityDescriptor>(parent).children;
	}

	inline const std::optional<EntityID>& Hierarchy::GetParent(const EntityID& child) const
	{
		return EntityManager::GetInstance().Get<EntityDescriptor>(child).parent;
	}

	// recursive function to help with parent child ordering
	void Hierarchy::TransformUpdateHelper(const EntityID& r_parentID)
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
				TransformUpdateHelper(childrenID);
			}
		}
	}

	void Hierarchy::UpdateParentList()
	{
		parentOrder.clear();
		sceneOrder.clear();
		for (const EntityID& id : SceneView<EntityDescriptor, Transform>())
		{
			// id 0 is default camera, ignore it
			if (id == 0)
				continue;
			// only if it is base layer
			if (!EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.has_value())
			{
				sceneOrder[EntityManager::GetInstance().Get<EntityDescriptor>(id).sceneID] = id;
			}
		}
		for (auto [k,v] : sceneOrder)
			parentOrder.emplace_back(v);
	}

	void Hierarchy::UpdateTransform()
	{
		for (const EntityID& parentID : parentOrder)
		{
			TransformUpdateHelper(parentID);
		}
	}

	void Hierarchy::UpdateETC()
	{
		// empty for now
	}

	void Hierarchy::RenderOrderUpdateHelper(const EntityID& parent, float min, float max)
	{
		const float delta = (max - min) / (EntityManager::GetInstance().Get<EntityDescriptor>(parent).children.size() + 1);
		EntityID prevSceneID{ ULLONG_MAX };
		std::map<EntityID, EntityDescriptor*> descs;
		for (const auto& id : EntityManager::GetInstance().Get<EntityDescriptor>(parent).children)
		{
			EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(id);
			if (desc.sceneID == ULLONG_MAX || desc.sceneID == prevSceneID)
				desc.sceneID = prevSceneID + 1;
			descs[desc.sceneID] = &desc;
			prevSceneID = desc.sceneID;
		}

		unsigned cnt{ 1 };
		for (auto [k,desc] : descs)
		{

			// how to handle when the layer of the child is diff from parents??
			// currently ignoring
			const float order = min + (delta * cnt);

			if (desc->renderOrder <= min || desc->renderOrder >= max || (desc->renderOrder != order))
				sceneHierarchy.erase(desc->renderOrder);
			desc->renderOrder = order;
			sceneHierarchy[desc->renderOrder] = desc->oldID;

			if (desc->children.size())
			{
				RenderOrderUpdateHelper(desc->oldID, desc->renderOrder, desc->renderOrder + delta);
			}
			++cnt;
		}
	}


	void Hierarchy::UpdateRenderOrder(EntityID targetID)
	{
		// update specific entity
		float delta = 100.f * parentOrder.size();

		if (targetID != ULLONG_MAX)
		{
			EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(targetID);
			if (!desc.parent.has_value())
			{
				sceneHierarchy.erase(desc.renderOrder);
				desc.renderOrder = static_cast<float>(desc.sceneID) + (desc.layer * delta);
				sceneHierarchy[desc.renderOrder] = targetID;
			}

			// recursively update children
			if (desc.children.size())
			{
				RenderOrderUpdateHelper(targetID, desc.renderOrder, desc.renderOrder + 1.f);
			}
		}
		else // update all the parents
		{
			sceneHierarchy.clear();
			for (const EntityID& parentID : parentOrder)
			{
				EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(parentID);
				// ignoring UI for now
				if (desc.sceneID == ULLONG_MAX)
					desc.sceneID = parentID;

				float RO = static_cast<float>(desc.sceneID) + (desc.layer * delta);
				if (RO != desc.renderOrder)
					sceneHierarchy.erase(desc.renderOrder);

				desc.renderOrder = RO;
				sceneHierarchy[desc.renderOrder] = parentID;
				

				// recursively update children
				if (desc.children.size())
				{
					RenderOrderUpdateHelper(parentID, desc.renderOrder, desc.renderOrder + 1.f);
				}
			}
		}
		
		renderOrder.clear();
		renderOrderUI.clear();
		// to update the logic here
		// main concern is differentiation of UI and world objects will be modified
		for (auto [k, v] : sceneHierarchy)
		{
			if (EntityManager::GetInstance().Has<PE::Graphics::Renderer>(v))
			{
				renderOrder.emplace_back(v);
			}
			// to change to canvas!!
			else if (EntityManager::GetInstance().Has<PE::Graphics::GUIRenderer>(v))
			{
				renderOrderUI.emplace_back(v);
			}
			
		}
		//std::cout << "-- Scene Hierarchy --" << std::endl;
		//for (auto[k,v] : sceneHierarchy)
		//{
		//	std::cout << k << ", " << v << std::endl;
		//}
		/*std::cout << "-- Object Render Order --" << std::endl;
		for (const auto& id : renderOrder)
		{
			std::cout << id << std::endl;
		}*/

		/*std::cout << "-- UI Render Order --" << std::endl;
		for (const auto& id : renderOrderUI)
		{
			std::cout << id << std::endl;
		}*/
	}
}