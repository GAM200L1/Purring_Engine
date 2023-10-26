#pragma once
#include "Script.h"
#include "StateManager.h"
namespace PE
{
	struct TestScript2Data
	{
		StateMachine m_stateMachine;
	};

	class testScript2 : public Script
	{
	public:
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);		
		virtual void OnAttach(EntityID id);
		virtual void OnDetach(EntityID id);
		~testScript2();
	private:
		std::map<EntityID, TestScript2Data> m_ScriptData;
	};

	class TestScript2IDLE : public State
	{
	public:
		virtual void StateEnter(EntityID id) override { std::cout << "Enter Idle" << std::endl; }
		virtual void StateUpdate(EntityID id, float deltaTime) override { std::cout << "Update Idle" << std::endl; }
		virtual void StateExit(EntityID id) override { std::cout << "Exit Idle" << std::endl; }
		virtual std::string_view GetName() override { return "IDLE"; }
	};

	class TestScript2JIGGLE : public State
	{
	public:
		virtual void StateEnter(EntityID id) override { std::cout << "Enter Jiggle" << std::endl; }
		virtual void StateUpdate(EntityID id, float deltaTime) override { std::cout << "Update Jiggle" << std::endl; }
		virtual void StateExit(EntityID id) override { std::cout << "Exit Jiggle" << std::endl; }
		virtual std::string_view GetName() override { return "JIGGLE"; }
	};
}