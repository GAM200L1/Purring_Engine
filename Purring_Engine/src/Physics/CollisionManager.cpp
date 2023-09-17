/*!*****************************************************************************
	@file       CollisionManager.cpp
	@author     Liew Yeni
	@co-author
	@par        DP email: yeni.l\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       9/9/2023

	@brief

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/

#include "prpch.h"
#include "CollisionManager.h"
#include "ECS/Entity.h"
#include "ECS/SceneView.h"

namespace PE
{
	// ----- Public Getters ----- //
	CollisionManager* CollisionManager::GetInstance()
	{
		if (!m_ptrInstance)
		{
			m_ptrInstance = new CollisionManager();
		}
		return m_ptrInstance;
	}

	void CollisionManager::UpdateColliders()
	{
		for (EntityID ColliderID : SceneView<Collider, Transform>())
		{
			Transform const& transform = g_entityManager->Get<Transform>(ColliderID);
			Collider& collider = g_entityManager->Get<Collider>(ColliderID);
			std::visit([&](auto& col)
			{

				Update(col, transform.position, transform.scale);
			
			}, collider.colliderVariant);
		}
	}

	void CollisionManager::TestColliders()
	{
		for (EntityID ColliderID_1 : SceneView<Collider>())
		{
			Collider const& collider1 = g_entityManager->Get<Collider>(ColliderID_1);

			for (EntityID ColliderID_2 : SceneView<Collider>())
			{

				Collider const& collider2 = g_entityManager->Get<Collider>(ColliderID_2);

				if (ColliderID_1 == ColliderID_2) { continue; }

				std::visit([&](auto const& col1)
					{
						std::visit([&](auto const& col2)
							{

								CollisionIntersection(col1, col2);

							}, collider2.colliderVariant);

					}, collider1.colliderVariant);

			}
		}
	}

	bool CollisionIntersection(AABBCollider const& AABB1, AABBCollider const& AABB2)
	{
		
	}
}
