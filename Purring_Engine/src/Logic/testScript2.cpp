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
		m_ScriptData[id].m_stateMachine = new StateMachine();
		m_ScriptData[id].m_stateMachine->ChangeState(new TestScript2IDLE(), id);
	}
	void testScript2::Update(EntityID id, float deltaTime)
	{
		if (InputSystem::IsKeyTriggered(GLFW_KEY_SPACE))
		{
			if(m_ScriptData[id].m_stateMachine->GetStateName() != "IDLE")
				m_ScriptData[id].m_stateMachine->ChangeState(new TestScript2IDLE(), id);
			else
				m_ScriptData[id].m_stateMachine->ChangeState(new TestScript2JIGGLE(), id);
		}

		m_ScriptData[id].m_stateMachine->Update(id,deltaTime);
	}
	void testScript2::Destroy(EntityID)
	{
	}

	void testScript2::OnAttach(EntityID id)
	{
		if (m_ScriptData.find(id) == m_ScriptData.end())
		{
			m_ScriptData[id] = TestScript2Data();
		}
		else
		{
			delete m_ScriptData[id].m_stateMachine;
			m_ScriptData[id] = TestScript2Data();
		}

	}

	void testScript2::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	testScript2::~testScript2()
	{
		for (auto& [key, val] : m_ScriptData)
		{
			delete val.m_stateMachine;
		}
	}
}