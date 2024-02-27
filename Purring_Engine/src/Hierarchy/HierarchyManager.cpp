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
#include "Graphics/Text.h"
#include "Layers/LayerManager.h"

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

	void Hierarchy::AttachChild(const EntityID& r_parent, const EntityID& r_child)
	{
		// entity DNE, return
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(r_child))
			return;
		
		if (RecursionHelper(r_parent, EntityManager::GetInstance().Get<EntityDescriptor>(r_child)))
		{
			return;
		}
		if (EntityManager::GetInstance().Get<EntityDescriptor>(r_child).parent)
		{
			EntityManager::GetInstance().Get<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(r_child).parent.value()).children.erase(r_child);
		}
		
		EntityManager::GetInstance().Get<EntityDescriptor>(r_parent).children.emplace(r_child);
		EntityManager::GetInstance().Get<EntityDescriptor>(r_child).parent = r_parent;
		vec3 tmpc (EntityManager::GetInstance().Get<Transform>(r_child).position, 1.f);
		// compute the relative position of the current object to the parent to get the proper relative position to set
		tmpc = EntityManager::GetInstance().Get<Transform>(r_parent).GetTransformMatrix3x3().Inverse() * tmpc;

		// set the relative positon to the computed values
		EntityManager::GetInstance().Get<Transform>(r_child).relPosition = vec2(tmpc.x, tmpc.y);
		EntityManager::GetInstance().Get<Transform>(r_child).relOrientation = EntityManager::GetInstance().Get<Transform>(r_child).orientation - EntityManager::GetInstance().Get<Transform>(r_parent).orientation;
		if (!EntityManager::GetInstance().Get<EntityDescriptor>(r_parent).isActive)
			EntityManager::GetInstance().Get<EntityDescriptor>(r_child).DisableEntity();
		
		auto& pr = EntityManager::GetInstance().Get<EntityDescriptor>(r_parent).children;
		auto& ch = EntityManager::GetInstance().Get<EntityDescriptor>(r_child).children;
		
		UpdateRenderOrder(r_parent);
	}

	void Hierarchy::DetachChild(const EntityID& r_child)
	{
		// entity DNE, return
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(r_child) || !EntityManager::GetInstance().Get<EntityDescriptor>(r_child).isAlive)
			return;
		if (EntityManager::GetInstance().Get<EntityDescriptor>(r_child).parent)
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(r_child).parent.value()))
				EntityManager::GetInstance().Get<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(r_child).parent.value()).children.erase(r_child);
		}
		EntityManager::GetInstance().Get<EntityDescriptor>(r_child).parent.reset();
		if (EntityManager::GetInstance().Has<Transform>(r_child))
		{
			EntityManager::GetInstance().Get<Transform>(r_child).relPosition.Zero();
			EntityManager::GetInstance().Get<Transform>(r_child).relOrientation = 0;
		}
		UpdateRenderOrder(r_child);
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
			if(EntityManager::GetInstance().Has<EntityDescriptor>(childrenID))
			if (EntityManager::GetInstance().Get<EntityDescriptor>(childrenID).children.size())
			{
				TransformUpdateHelper(childrenID);
			}
		}
	}

	void Hierarchy::UpdateParentList()
	{
		m_parentOrder.clear();
		m_sceneOrder.clear();
		m_hierarchyOrder.clear();
		try
		{
			for (const auto& layer : LayerView<EntityDescriptor, Transform>(true))
			{
				for (const EntityID& id : InternalView(layer))
				{
					// id 0 is default camera, ignore it
					if (id == 0)
						continue;
					// only if it is base layer
					if (!HasParent(id))
					{
						m_sceneOrder[EntityManager::GetInstance().Get<EntityDescriptor>(id).sceneID] = id;
					}
				}
			}
			for (auto [k, v] : m_sceneOrder)
				m_hierarchyOrder.emplace_back(v);

			m_sceneOrder.clear();
			for (const auto& layer : LayerView<EntityDescriptor, Transform>())
			{
				for (const EntityID& id : InternalView(layer))
				{
					// id 0 is default camera, ignore it
					if (id == 0)
						continue;
					// only if it is base layer
					if (!HasParent(id))
					{
						m_sceneOrder[EntityManager::GetInstance().Get<EntityDescriptor>(id).sceneID] = id;
					}
				}
			}
			for (auto [k, v] : m_sceneOrder)
				m_parentOrder.emplace_back(v);
		}
		catch (...)
		{
			engine_logger.AddLog(false, "Failed to access entities", __FUNCTION__);
		}
	}

	void Hierarchy::UpdateTransform()
	{
		for (const EntityID& parentID : m_parentOrder)
		{
			TransformUpdateHelper(parentID);
		}
	}

	void Hierarchy::UpdateETC()
	{
		// empty for now
	}

	void Hierarchy::RenderOrderUpdateHelper(const EntityID& r_parent, float min, float max)
	{
		auto& pr = EntityManager::GetInstance().Get<EntityDescriptor>(r_parent).children;
		const float delta = (max - min) / (EntityManager::GetInstance().Get<EntityDescriptor>(r_parent).children.size() + 1);
		EntityID prevSceneID{ ULLONG_MAX };
		std::map<EntityID, EntityDescriptor*> descs;
		for (const auto& id : EntityManager::GetInstance().Get<EntityDescriptor>(r_parent).children)
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
				m_sceneHierarchy.erase(desc->renderOrder);
			desc->renderOrder = order;
			m_sceneHierarchy[desc->renderOrder] = desc->sceneID;

			if (desc->children.size())
			{
				RenderOrderUpdateHelper(desc->sceneID, desc->renderOrder, desc->renderOrder + delta);
			}
			++cnt;
		}
	}


	void Hierarchy::UpdateRenderOrder(EntityID targetID)
	{
		// update specific entity
		float delta = 100.f * m_parentOrder.size();

		if (targetID != ULLONG_MAX)
		{
			EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(targetID);
			if (!desc.parent.has_value())
			{
				m_sceneHierarchy.erase(desc.renderOrder);
				desc.renderOrder = static_cast<float>(desc.sceneID) + (desc.layer * delta);
				m_sceneHierarchy[desc.renderOrder] = targetID;
			}

			// recursively update children
			if (desc.children.size())
			{
				RenderOrderUpdateHelper(targetID, desc.renderOrder, desc.renderOrder + 1.f);
			}
		}
		else // update all the parents
		{
			m_sceneHierarchy.clear();
			for (const EntityID& parentID : m_parentOrder)
			{
				EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(parentID);
				// ignoring UI for now
				if (desc.sceneID == ULLONG_MAX)
					desc.sceneID = parentID;

				float RO = static_cast<float>(desc.sceneID) + (desc.layer * delta);
				if (RO != desc.renderOrder)
					m_sceneHierarchy.erase(desc.renderOrder);

				desc.renderOrder = RO;
				m_sceneHierarchy[desc.renderOrder] = parentID;
				

				// recursively update children
				if (desc.children.size())
				{
					RenderOrderUpdateHelper(parentID, desc.renderOrder, desc.renderOrder + 1.f);
				}
			}
		}
		
		m_renderOrder.clear();
		m_renderOrderUI.clear();
		// to update the logic here
		// main concern is differentiation of UI and world objects will be modified
		for (auto [k, v] : m_sceneHierarchy)
		{
			if (EntityManager::GetInstance().Has<PE::Graphics::Renderer>(v))
			{
				m_renderOrder.emplace_back(v);
			}
			// to change to canvas!!
			else if (EntityManager::GetInstance().Has<PE::Graphics::GUIRenderer>(v))
			{
				if (!GETGUISYSTEM()->AreThereActiveCanvases()) { continue; }

				if(GETGUISYSTEM()->IsChildedToCanvas(v)) // Check if it's childed to a canvas
					m_renderOrderUI.emplace_back(v);
			}
			else if (EntityManager::GetInstance().Has<PE::TextComponent>(v))
			{
				if (GETGUISYSTEM()->IsChildedToCanvas(v)) // Check if it's childed to a canvas
					m_renderOrderUI.emplace_back(v);
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