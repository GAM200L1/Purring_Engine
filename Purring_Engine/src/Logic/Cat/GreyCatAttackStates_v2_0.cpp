/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackStates_v2_0.cpp
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Attack Base class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "GreyCatAttackStates_v2_0.h"
#include "CatScript_v2_0.h"
#include "Hierarchy/HierarchyManager.h"

namespace PE
{

	// ----- ATTACK PLAN ----- //

	void GreyCatAttack_v2_0PLAN::StateEnter(EntityID id)
	{
		p_data = &std::get<GreyCatAttackVariables>((GETSCRIPTDATA(CatScript_v2_0, id))->attackVariables);


	}

	void GreyCatAttack_v2_0PLAN::StateUpdate(EntityID id, float deltaTime)
	{

	}

	void GreyCatAttack_v2_0PLAN::StateCleanUp()
	{

	}

	void GreyCatAttack_v2_0PLAN::StateExit(EntityID id)
	{

	}

	void GreyCatAttack_v2_0PLAN::CreateProjectileTelegraphs(EntityID id, float bulletRange, std::map<EnumCatAttackDirection_v2_0, EntityID>& r_telegraphIDs)
	{
		auto CreateOneTelegraph = 
			[&](bool isXAxis, bool isNegative)
			{
				Transform const& catTransform = EntityManager::GetInstance().Get<Transform>(id);

				SerializationManager serializationManager;

				EntityID telegraphID = serializationManager.LoadFromFile("PlayerAttackTelegraph_Prefab.json");
				Transform& telegraphTransform = EntityManager::GetInstance().Get<Transform>(telegraphID);

				//EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).parent = id; // telegraph follows the cat entity
				Hierarchy::GetInstance().AttachChild(id, telegraphID); // new way of attatching parent child
				telegraphTransform.relPosition.Zero();
				EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).isActive = false; // telegraph to not show until attack planning
				EntityManager::GetInstance().Get<EntityDescriptor>(telegraphID).toSave = false; // telegraph to not show until attack planning


				// set size of telegraph
				telegraphTransform.height = catTransform.height * 0.75f;
				telegraphTransform.width = catTransform.width * bulletRange;
				EnumCatAttackDirection_v2_0 dir;
				AABBCollider telegraphCollider;

				// Set the dimensions of the telegraph based on the axis it's on
				if (isXAxis)
				{
					telegraphTransform.relPosition.x = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);
					dir = (isNegative) ? EnumCatAttackDirection_v2_0::WEST : EnumCatAttackDirection_v2_0::EAST;
				}
				else
				{
					telegraphTransform.relOrientation = PE_PI * 0.5f;
					telegraphTransform.relPosition.y = ((isNegative) ? -1.f : 1.f) * ((telegraphTransform.width * 0.5f) + (catTransform.width * 0.5f) + 10.f);

					telegraphCollider.scaleOffset.x = telegraphTransform.height / telegraphTransform.width;
					telegraphCollider.scaleOffset.y = telegraphTransform.width / telegraphTransform.height;

					dir = (isNegative) ? EnumCatAttackDirection_v2_0::SOUTH : EnumCatAttackDirection_v2_0::NORTH;
				}

				// Configure the collider
				EntityManager::GetInstance().Get<Collider>(telegraphID).colliderVariant = telegraphCollider;
				EntityManager::GetInstance().Get<Collider>(telegraphID).isTrigger = true;

				r_telegraphIDs.emplace(dir, telegraphID);
			};

		CreateOneTelegraph(true, false); // east
		CreateOneTelegraph(true, true); // west
		CreateOneTelegraph(false, false); // north
		CreateOneTelegraph(false, true); // south
	}

	void GreyCatAttack_v2_0PLAN::OnMouseClick(const Event<MouseEvents>& r_ME)
	{

	}

	void GreyCatAttack_v2_0PLAN::ResetSelection()
	{

	}

	// ----- ATTACK EXECUTE ----- //

	void GreyCatAttack_v2_0EXECUTE::StateEnter(EntityID id)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateUpdate(EntityID id, float deltaTime)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateCleanUp()
	{

	}

	void GreyCatAttack_v2_0EXECUTE::StateExit(EntityID id)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::ProjectileHitCat(const Event<CollisionEvents>& r_CE)
	{

	}

	void GreyCatAttack_v2_0EXECUTE::ProjectileHitRat(const Event<CollisionEvents>& r_CE)
	{

	}
}