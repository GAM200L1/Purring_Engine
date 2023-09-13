/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     EntityFactor.cpp
 \date     11-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "EntityFactory.h"

namespace Engine
{
	EntityFactory* g_entityFactory{ nullptr };

	EntityFactory::EntityFactory() : p_entityManager(Engine::g_entityManager)
	{ 
		if (g_entityFactory != nullptr)
			throw;
		g_entityFactory = this;
	};

	EntityFactory::~EntityFactory()
	{
		for (std::pair<const ComponentID, ComponentCreator*>& cmt : m_componentMap)
		{
			delete cmt.second;
		}
	}



	EntityID EntityFactory::Clone(EntityID id)
	{
		if (p_entityManager->IsEntityValid(id))
		{
			EntityID clone = CreateEntity();
			for (std::pair<const ComponentID, ComponentCreator*>& componentCreator : m_componentMap)
			{
				if (p_entityManager->Has(id, componentCreator.first))
				{
					p_entityManager->CopyComponent(id, clone, componentCreator.first);
				}
			}
			return clone;
		}
		else
		{
			throw;
		}
	}

	void EntityFactory::AddComponentCreator(const ComponentID& name, ComponentCreator* creator)
	{
		m_componentMap[name] = creator;
	}
	
}