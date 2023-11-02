#pragma once
#include "Script.h"
#include "StateManager.h"

namespace PE {
	struct EnemyTestScriptData
	{
		EntityID playerID{ 1 };
		float speed{ 5000 };
		float idleTimer{ 3.0f };
		float alertTimer{ 0 };
		float timerBuffer{ 3.0f };
		float patrolTimer{ 5.0f };
		float patrolBuffer{ 3.0f };
		float distanceFromPlayer{ 0.0f };
		float TargetRange{ 200 };
		bool bounce{ true };
		StateMachine* m_stateManager;
	};

	class EnemyTestScript : public PE::Script
	{
	public:
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);
		virtual void OnAttach(EntityID id);
		virtual void OnDetach(EntityID id);
		std::map<EntityID, EnemyTestScriptData>& GetScriptData();
		rttr::instance GetScriptData(EntityID id);
		virtual ~EnemyTestScript();

	public:
		std::map<EntityID, EnemyTestScriptData> m_ScriptData;

	private:
		float GetDistanceFromPlayer(EntityID id);
	};

	class EnemyTestIDLE : public State
	{
		virtual void StateEnter(EntityID id) override;
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		virtual void StateExit(EntityID id) override;
		virtual std::string_view GetName() override;
		virtual ~EnemyTestIDLE() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};

	class EnemyTestPATROL : public State
	{
		virtual void StateEnter(EntityID id) override;
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		virtual void StateExit(EntityID id) override;
		virtual std::string_view GetName() override;
		virtual ~EnemyTestPATROL() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};

	class EnemyTestALERT : public State
	{
		virtual void StateEnter(EntityID id) override;
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		virtual void StateExit(EntityID id) override;
		virtual std::string_view GetName() override;
		virtual ~EnemyTestALERT() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};

	class EnemyTestTARGET : public State
	{
		virtual void StateEnter(EntityID id) override;
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		virtual void StateExit(EntityID id) override;
		virtual std::string_view GetName() override;
		virtual ~EnemyTestTARGET() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};

	class EnemyTestATTACK : public State
	{
		virtual void StateEnter(EntityID id) override;
		virtual void StateUpdate(EntityID id, float deltaTime) override;
		virtual void StateExit(EntityID id) override;
		virtual std::string_view GetName() override;
		virtual ~EnemyTestATTACK() { p_data = nullptr; }
	private:
		EnemyTestScriptData* p_data;
	};
}
