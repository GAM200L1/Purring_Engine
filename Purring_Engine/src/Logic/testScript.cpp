#include "prpch.h"
#include "testScript.h"
# define M_PI           3.14159265358979323846 

namespace PE 
{
	void testScript::Init(EntityID id)
	{
	}
	void testScript::Update(EntityID id, float deltaTime)
	{
		PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation += static_cast<float>(180 * (M_PI / 180) * deltaTime * PE::EntityManager::GetInstance().Get <TestScriptData>(id).m_rotationSpeed);
	}
	void testScript::Destroy(EntityID id)
	{
	}

	void testScript::OnAttach(EntityID id)
	{
		if (!EntityManager::GetInstance().Has(id, EntityManager::GetInstance().GetComponentID<TestScriptData>()))
			EntityFactory::GetInstance().Assign(id, { EntityManager::GetInstance().GetComponentID<TestScriptData>() });
	}

}