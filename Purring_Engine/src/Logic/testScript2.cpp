#include "prpch.h"
#include "testScript2.h"
# define M_PI           3.14159265358979323846 

namespace PE 
{
	void testScript2::Init(EntityID id)
	{
	}
	void testScript2::Update(EntityID id, float deltaTime)
	{
		PE::EntityManager::GetInstance().Get<PE::Transform>(id).position.x += 100 * deltaTime;
	}
	void testScript2::Destroy(EntityID id)
	{
	}


}