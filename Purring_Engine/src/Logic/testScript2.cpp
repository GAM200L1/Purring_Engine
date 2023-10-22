#include "prpch.h"
#include "testScript2.h"
#include "Input/InputSystem.h"
# define M_PI           3.14159265358979323846 

namespace PE 
{

	void testScript2::Init(EntityID id)
	{
		id;
	}
	void testScript2::Update(EntityID id, float deltaTime)
	{
		//PE::EntityManager::GetInstance().Get<PE::Transform>(id).position.x += 100 * deltaTime;
		deltaTime;
		Transform& currentObject = PE::EntityManager::GetInstance().Get<PE::Transform>(id);
		Transform& targetObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_ScriptData[id].targetID);
		//Transform& colliderObject = PE::EntityManager::GetInstance().Get<PE::Transform>(m_ScriptData[id].CollisionTarget);
					
		////only doing this for aabb for now ill think abt circle somehow next time
		//AABBCollider* p_ab;
		//try 
		//{
		//	p_ab = &std::get<AABBCollider>(EntityManager::GetInstance().Get<Collider>(PE::EntityManager::GetInstance().Get<TestScript2Data>(id).CollisionTarget).colliderVariant);
		//}
		//catch (...)
		//{
		//	std::cout << "no aabb collider";
		//}
		//distance between target and current
		float dx = targetObject.position.x - currentObject.position.x;
		float dy = targetObject.position.y - currentObject.position.y;
			
		////need to calculate the 2 points of the collider closer to current object
		//
		//	//get the 4 points of the collider
		//	vec2 tr,br,tl,bl;
		//	//top right
		//	tr = p_ab->max;
		//	//get top left
		//	tl = tr;
		//	tl.x -= p_ab->scale.x;
		//	//bottom left
		//	bl = p_ab->min;
		//	//get bottom right
		//	br = tr;
		//	br.y -= p_ab->scale.y;
		//	//calculate distance vector of the 4 points from the current object
		//	vec2 d_tr = tr - currentObject.position;
		//	vec2 d_br = br - currentObject.position;
		//	vec2 d_tl = tl - currentObject.position;
		//	vec2 d_bl = bl - currentObject.position;
		//	PE::EntityManager::GetInstance().Get<TestScript2Data>(id).points.push_back(d_tr);
		//	PE::EntityManager::GetInstance().Get<TestScript2Data>(id).points.push_back(d_br);
		//	PE::EntityManager::GetInstance().Get<TestScript2Data>(id).points.push_back(d_tl);
		//	PE::EntityManager::GetInstance().Get<TestScript2Data>(id).points.push_back(d_bl);
		//	//calculate which 2 points is the closest		
		//	vec2 p1, p2;

		//	float d1 = d_tr.Distance(vec2(0, 0));
		//	float d2 = d_br.Distance(vec2(0, 0));
		//	float d3 = d_tl.Distance(vec2(0, 0));
		//	float d4 = d_bl.Distance(vec2(0, 0));

		//	// Find the shortest distances
		//	std::vector<float> distances;
		//	distances.push_back(d1);
		//	distances.push_back(d2);
		//	distances.push_back(d3);
		//	distances.push_back(d4);
		//	//sorts in ascending so shortest distances is [0] and [1]
		//	std::sort(distances.begin(),distances.end());

		//then calculate if player is behind those 2 points somehow (I might just use center instead of the entire range)
			
		//then calculate player is closer to which point

		//then take that angle as the angle to rotate towards

		//if object not behind collider


		// Calculate the angle between the sprite and the target
		float rotation = atan2(dy, dx);
		if(InputSystem::IsKeyTriggered(GLFW_KEY_SPACE))
		PE::EntityManager::GetInstance().Get<PE::Transform>(id).orientation = rotation;
	}
	void testScript2::Destroy(EntityID id)
	{
		id;
	}

	void testScript2::OnAttach(EntityID id)
	{
		m_ScriptData[id] = TestScript2Data();
	}

	void testScript2::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	testScript2::~testScript2()
	{
	}
}