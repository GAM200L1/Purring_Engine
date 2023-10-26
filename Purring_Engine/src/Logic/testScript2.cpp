#include "prpch.h"
#include "testScript2.h"
#include "Input/InputSystem.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
# define M_PI           3.14159265358979323846 

namespace PE 
{
	void testScript2::Init(EntityID id)
	{
		m_ScriptData[id].m_stateMachine.ChangeState(new TestScript2IDLE());
	}
	void testScript2::Update(EntityID id, float deltaTime)
	{
		if (InputSystem::IsKeyTriggered(GLFW_KEY_SPACE))
		{
			if(m_ScriptData[id].m_stateMachine.GetStateName() != "IDLE")
				m_ScriptData[id].m_stateMachine.ChangeState(new TestScript2IDLE());
			else
				m_ScriptData[id].m_stateMachine.ChangeState(new TestScript2JIGGLE());
		}

		m_ScriptData[id].m_stateMachine.Update();
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