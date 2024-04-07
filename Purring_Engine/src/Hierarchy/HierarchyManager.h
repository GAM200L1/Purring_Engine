/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     HierarchyManager.h
 \date     17-12-2023

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   This file contains the decleration of the Hierarchy class (singleton), that 
 		   manages the Transform Hierarchy (any additional behaviour TBD) of the 
		   entities in the scene.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#pragma once

#include "Singleton.h"

// Entity Component System (ECS)
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/SceneView.h"

// for accessing transform information & methods
#include "Math/Transform.h"

#include <optional>

namespace PE
{
	/*!***********************************************************************************
	 \brief Hierarchy class for use in managing transform (and potentially other 
	 additional hierarchy behaviours) hierarchy.
	 
	*************************************************************************************/
	class Hierarchy : public Singleton<Hierarchy>
	{
	// ----- Public Variables ----- //
	public:
		friend class Singleton<Hierarchy>;

	// ----- Public Getters ----- //
	public:
		/*!***********************************************************************************
		 \brief Get the cached Render Order vector object to assist with rendering
		 
		 \return const std::vector<EntityID>&  the cached vector
		*************************************************************************************/
		const std::vector<EntityID>& GetRenderOrder() const { return m_renderOrder; }

		/*!***********************************************************************************
		 \brief Get the cached Render Order UI vector object to assist with rendering
		 
		 \return const std::vector<EntityID>&  the cached vector
		*************************************************************************************/
		const std::vector<EntityID>& GetRenderOrderUI() const { return m_renderOrderUI; }

		/*!***********************************************************************************
		 \brief Get the Parent Order vector object mainly used in Editor.cpp by the object
		 window
		 
		 \return const std::vector<EntityID>& the cached vector
		*************************************************************************************/
		const std::vector<EntityID>& GetParentOrder() const { return m_parentOrder; }

		/*!***********************************************************************************
		 \brief Get the Hierarchy Order vector object mainly used in Editor.cpp by the object
		 window

		 \return const std::vector<EntityID>& the cached vector
		*************************************************************************************/
		const std::vector<EntityID>& GetHierarchyOrder() const { return m_hierarchyOrder; }

		/*!***********************************************************************************
		 \brief Helper function to get the children of the input entity
		 
		 \param[in] r_parent 	EntityID of the parent to fetch children of
		 \return const std::set<EntityID>& 	The children
		*************************************************************************************/
		inline const std::set<EntityID>& GetChildren(const EntityID& r_parent) const
		{
				return EntityManager::GetInstance().Get<EntityDescriptor>(r_parent).children;
		}

		/*!***********************************************************************************
		 \brief Get the immediate parent (std::optional, treat properly!) of the current entity 
		 
		 \param[in] r_child 	The child to request parent (might not have a parent)
		 \return const std::optional<EntityID>& The potential parent
		*************************************************************************************/
		inline const std::optional<EntityID>& Hierarchy::GetParent(const EntityID& r_child) const
		{
				return EntityManager::GetInstance().Get<EntityDescriptor>(r_child).parent;
		}

		/*!***********************************************************************************
		 \brief Returns true if the object is childed to another object, false otherwise
		 
		 \param[in] child 	The child to check
		 \return bool Returns true if the object is childed to another object, false otherwise
		*************************************************************************************/
		bool HasParent(const EntityID& child) const;

		/*!***********************************************************************************
		 \brief Returns false if the object's immediate parent is inactive, true otherwise. 
						If the object has no parent, its active status is returned.
		 
		 \param[in] child 	The child to check the parents of
		 \return bool Returns false if the object's immediate parent is inactive, true otherwise. 
						If the object has no parent, its active status is returned.
		*************************************************************************************/
		bool IsImmediateParentActive(EntityID child) const;

		/*!***********************************************************************************
		 \brief Returns false if any of the object's parents is inactive, true otherwise.
						If the object has no parent, its active status is returned.
		 
		 \param[in] child 	The child to check
		 \return bool Returns false if any of the object's parents is inactive, true otherwise.
						If the object has no parent, its active status is returned.
		*************************************************************************************/
		bool AreParentsActive(EntityID child) const;

		/*!***********************************************************************************
		 \brief Get the absolute parent (std::optional, treat properly!) of the current entity 
		 
		 \param[in] child 	The child to request parent (might not have a parent)
		 \return const std::optional<EntityID>& The potential absolute parent
		*************************************************************************************/
		const std::optional<EntityID>& GetAbsoluteParent(EntityID child) const;

	// ----- Public Methods ----- //
	public:
		/*!***********************************************************************************
		\brief Update function for updating various Hierarchy functions (only public update 
		interface) to be called in coreapplication
		
		*************************************************************************************/
		void Update();

		/*!***********************************************************************************
		 \brief Helper function to attach a entity to a parent entity
		 
		 \param[in] r_parent 	ID of the parent entity
		 \param[in] r_child 	ID of the child entity
		*************************************************************************************/
		void AttachChild(const EntityID& r_parent, const EntityID& r_child);
		
		/*!***********************************************************************************
		 \brief Helper function to detach a child entity from its parent
		 
		 \param[in] child	ID of the child entity to detach
		*************************************************************************************/
		void DetachChild(const EntityID& r_child);

		
		
	
	// ----- Private Methods ----- //
	private: 
		/*!***********************************************************************************
		 \brief Update function helper to allow recursion for parent/child processing to 
		 ensure correct transforms are computed(from root, being the parent outward)
		 
		 \param[in] r_id 	Parent EntityID
		*************************************************************************************/
		void TransformUpdateHelper(const EntityID& r_id);

		/*!***********************************************************************************
		 \brief Updates the parentOrder vector (grabs all the true parents of the hierarchy, 
		 i.e. only those that do not have parents)
		 
		*************************************************************************************/
		void UpdateParentList();

		/*!***********************************************************************************
		 \brief Updates the transforms of the entities in the scene, utilizes UpdateHelper()
		 
		*************************************************************************************/
		void UpdateTransform();

		/*!***********************************************************************************
		 \brief Updates misc stuff (currently empty)
		 
		*************************************************************************************/
		void UpdateETC(); // for any other misc behaviour that we may want to add for inheriting stuff

		/*!***********************************************************************************
		 \brief Helper function to allow for easy recursion to update parents & it's children
		 		entities in the correct order.
		 
		 \param[in] r_id 		Target entity ID
		 \param[in] min 	Minimum value for sorting purposes in the sceneOrder map
		 \param[in] max 	Maximum value for sorting purposes in the sceneOrder map
		*************************************************************************************/
		void RenderOrderUpdateHelper(const EntityID& r_id, float min, float max);

		/*!***********************************************************************************
		 \brief Updates the renderOrder vector (TODO)
		 
		*************************************************************************************/
		void UpdateRenderOrder(EntityID targetID = ULLONG_MAX);

	// ----- Private Variables ----- //
	private: 
		std::vector<EntityID> m_renderOrder;
		std::vector<EntityID> m_renderOrderUI;

		std::map<EntityID, EntityID> m_sceneOrder;
		std::vector<EntityID> m_parentOrder; // wiped every frame? used to keep track of update order for parents, might change to list if i start inserting more...
		std::vector<EntityID> m_hierarchyOrder;

		std::map<float, EntityID> m_sceneHierarchy;
	};
}
