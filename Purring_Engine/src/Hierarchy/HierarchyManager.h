/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     HierarchyManager.h
 \date     17-12-2023

 \author               FOONG Jun Wei
 \par      email:      f.junwei@digipen.edu

 \brief	   huh

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

namespace PE
{
	class Hierarchy : public Singleton<Hierarchy>
	{
	// public member variables
	public:
		friend class Singleton<Hierarchy>;

	// public methods
	public:
		void Update();
		
		inline const std::vector<EntityID>& GetRenderOrder() const { return renderOrder; }
		inline const std::vector<EntityID>& GetParentOrder() const { return parentOrder; }

		void AttachChild(const EntityID& parent, const EntityID& child);
		void DetachChild(const EntityID& child);

		inline const std::set<EntityID>& GetChildren(const EntityID& parent) const;
		inline const std::optional<EntityID>& GetParent(const EntityID& child) const;

	// private methods
	private: 
		void UpdateHelper(const EntityID& id);

		void UpdateParentList();

		void UpdateTransform();

		void UpdateETC(); // for any other misc behaviour that we may want to add for inheriting stuff

		void UpdateRenderOrder();

	// private member variables
	private: 
		std::vector<EntityID> renderOrder;
		std::vector<EntityID> parentOrder; // wiped every frame? used to keep track of update order for parents, might change to list if i start inserting more...
	};
}
